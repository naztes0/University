import { removeImageBackground } from '../../services/imageService.js';
import userModel from '../../models/userModel.js';
import axios from 'axios';
import fs from 'fs';
import FormData from 'form-data';
import config from '../../configs/appConfig.js';

// Mock dependencies
jest.mock('../../models/userModel.js');
jest.mock('axios');
jest.mock('fs');
jest.mock('../../configs/appConfig.js');

describe('imageService tests', () => {
    beforeEach(() => {
        jest.clearAllMocks();

        FormData.prototype.append = jest.fn();
        fs.createReadStream.mockReturnValue('mock-file-stream');

        // Mock config
        config.apiKeys = {
            clipdrop: 'test-api-key'
        };
        config.credits = {
            max: 5,
            refreshTimer: 2
        };
    });

    describe('removeImageBackground function', () => {
        test('should successfully process image and return base64 data with decremented credits', async () => {
            // Arrange
            const mockUser = {
                _id: 'user123',
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null,
                mime: 'image/jpeg'
            };

            const mockImagePath = '/path/to/image.jpg';
            const mockApiResponse = Buffer.from('fake-image-data');

            axios.post.mockResolvedValue({ data: mockApiResponse });
            userModel.findByIdAndUpdate.mockResolvedValue({});

            // Act
            const result = await removeImageBackground(mockUser, mockImagePath);

            // Assert
            expect(fs.createReadStream).toHaveBeenCalledWith(mockImagePath);
            expect(axios.post).toHaveBeenCalledWith(
                'https://clipdrop-api.co/remove-background/v1',
                expect.anything(),
                expect.objectContaining({
                    headers: {
                        'x-api-key': 'test-api-key'
                    },
                    responseType: 'arraybuffer'
                })
            );

            expect(userModel.findByIdAndUpdate).toHaveBeenCalledWith(
                'user123',
                expect.objectContaining({
                    creditBalance: 2,
                    timerActive: true,
                    nextCreditAt: expect.any(Date)
                })
            );

            expect(result).toEqual({
                resultImage: 'data: image/jpeg;base64,ZmFrZS1pbWFnZS1kYXRh',
                creditBalance: 2,
                timerActive: true,
                nextCreditAt: expect.any(Date)
            });
        });

        test('should activate credit refill timer when credits drop below maximum', async () => {
            // Arrange
            const mockUser = {
                _id: 'user123',
                creditBalance: 5,
                timerActive: false,
                nextCreditAt: null,
                mime: 'image/png'
            };

            const mockImagePath = '/path/to/image.png';
            const mockApiResponse = Buffer.from('fake-image-data');

            axios.post.mockResolvedValue({ data: mockApiResponse });
            userModel.findByIdAndUpdate.mockResolvedValue({});

            // Act
            await removeImageBackground(mockUser, mockImagePath);

            // Assert
            expect(userModel.findByIdAndUpdate).toHaveBeenCalledWith(
                'user123',
                expect.objectContaining({
                    creditBalance: 4,
                    timerActive: true,
                    nextCreditAt: expect.any(Date),
                })
            );
        });

        test('should preserve existing timer settings when timer is already active', async () => {
            // Arrange
            const futureDate = new Date();
            futureDate.setHours(futureDate.getHours() + 1);

            const mockUser = {
                _id: 'user123',
                creditBalance: 3,
                timerActive: true,
                nextCreditAt: futureDate,
                mime: 'image/jpeg'
            };

            const mockImagePath = '/path/to/image.jpg';
            const mockApiResponse = Buffer.from('fake-image-data');

            axios.post.mockResolvedValue({ data: mockApiResponse });
            userModel.findByIdAndUpdate.mockResolvedValue({});

            // Act
            const result = await removeImageBackground(mockUser, mockImagePath);

            // Assert
            expect(userModel.findByIdAndUpdate).toHaveBeenCalledWith(
                'user123',
                expect.objectContaining({
                    creditBalance: 2,
                    timerActive: true,
                    nextCreditAt: futureDate
                })
            );

            expect(result.timerActive).toBe(true);
            expect(result.nextCreditAt).toEqual(futureDate);
        });

        test('should propagate and enhance error information from API failures', async () => {
            // Arrange
            const mockUser = {
                _id: 'user123',
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null,
                mime: 'image/jpeg'
            };

            const mockImagePath = '/path/to/image.jpg';
            const errorMessage = 'API request failed';

            axios.post.mockRejectedValue(new Error(errorMessage));

            // Act & Assert
            await expect(removeImageBackground(mockUser, mockImagePath))
                .rejects
                .toThrow(`Error removing background: ${errorMessage}`);

            expect(userModel.findByIdAndUpdate).not.toHaveBeenCalled();
        });

        test('should format base64 data according to user\'s mime type', async () => {
            // Arrange
            const mockUser = {
                _id: 'user123',
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null,
                mime: 'image/png'
            };

            const mockImagePath = '/path/to/image.png';
            const mockApiResponse = Buffer.from('fake-image-data');

            axios.post.mockResolvedValue({ data: mockApiResponse });
            userModel.findByIdAndUpdate.mockResolvedValue({});

            // Act
            const result = await removeImageBackground(mockUser, mockImagePath);

            // Assert
            expect(result.resultImage).toBe('data: image/png;base64,ZmFrZS1pbWFnZS1kYXRh');
        });

        test('should update user\'s lastCreditUpdate timestamp when processing image', async () => {
            // Arrange
            const mockUser = {
                _id: 'user123',
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null,
                mime: 'image/jpeg'
            };

            const mockImagePath = '/path/to/image.jpg';
            const mockApiResponse = Buffer.from('fake-image-data');

            axios.post.mockResolvedValue({ data: mockApiResponse });
            userModel.findByIdAndUpdate.mockResolvedValue({});

            // Store time before operation
            const beforeTime = new Date();

            // Act
            await removeImageBackground(mockUser, mockImagePath);

            // Assert
            expect(userModel.findByIdAndUpdate).toHaveBeenCalledWith(
                'user123',
                expect.objectContaining({
                    lastCreditUpdate: expect.any(Date)
                })
            );

            // Extract the date from the call arguments
            const updateCallArgs = userModel.findByIdAndUpdate.mock.calls[0][1];
            const updatedDate = updateCallArgs.lastCreditUpdate;

            // Verify the date is after our reference time
            expect(updatedDate.getTime()).toBeGreaterThanOrEqual(beforeTime.getTime());
        });
    });
});