import { removeBgImage } from '../../controllers/ImageController.js';
import userModel from '../../models/userModel.js';
import axios from 'axios';
import fs from 'fs';

// Мокуємо залежності
jest.mock('../../models/userModel.js');
jest.mock('axios');
jest.mock('fs');
jest.mock('form-data', () => {
    return function () {
        return {
            append: jest.fn()
        };
    };
});

describe('ImageController test', () => {
    let req, res;

    beforeEach(() => {
        jest.clearAllMocks();

        req = {
            body: {
                clerkId: 'test-clerk-id'
            },
            file: {
                path: 'test/file/path.jpg',
                mimetype: 'image/jpeg'
            }
        };

        res = {
            json: jest.fn().mockReturnThis()
        };

        // Мокуємо fs.createReadStream
        fs.createReadStream.mockReturnValue('mocked-file-stream');
    });

    describe('removeBgImage function', () => {
        test('should successfully remove background and update credits', async () => {
            // Arrange
            const mockUser = {
                _id: 'user-123',
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null,
                lastCreditUpdate: new Date()
            };

            userModel.findOne.mockResolvedValue(mockUser);
            userModel.findByIdAndUpdate.mockResolvedValue(mockUser);

            // Мокуємо відповідь від API для видалення фону
            const mockBuffer = Buffer.from('fake-image-data');
            axios.post.mockResolvedValue({
                data: mockBuffer
            });

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(fs.createReadStream).toHaveBeenCalledWith('test/file/path.jpg');
            expect(axios.post).toHaveBeenCalledWith(
                'https://clipdrop-api.co/remove-background/v1',
                expect.anything(),
                expect.objectContaining({
                    headers: {
                        'x-api-key': process.env.CLIPDROP_API
                    },
                    responseType: 'arraybuffer'
                })
            );

            expect(userModel.findByIdAndUpdate).toHaveBeenCalledWith(
                'user-123',
                expect.objectContaining({
                    creditBalance: 2,
                    timerActive: true,
                    nextCreditAt: expect.any(Date),
                    lastCreditUpdate: expect.any(Date)
                })
            );

            expect(res.json).toHaveBeenCalledWith(
                expect.objectContaining({
                    success: true,
                    resultImage: expect.stringContaining('data: image/jpeg;base64,'),
                    creditBalance: 2,
                    message: 'Background Removed'
                })
            );
        });

        test('should handle user not found error', async () => {
            // Arrange
            userModel.findOne.mockResolvedValue(null);

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: 'User not found'
            });
            expect(axios.post).not.toHaveBeenCalled();
        });

        test('should handle insufficient credits error', async () => {
            // Arrange
            const mockUser = {
                creditBalance: 0,
                timerActive: false
            };

            userModel.findOne.mockResolvedValue(mockUser);

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: 'No credit balance',
                creditBalance: 0
            });
            expect(axios.post).not.toHaveBeenCalled();
        });

        test('should handle API error', async () => {
            // Arrange
            const mockUser = {
                _id: 'user-123',
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null
            };

            userModel.findOne.mockResolvedValue(mockUser);

            const errorMessage = 'API error';
            axios.post.mockRejectedValue(new Error(errorMessage));

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(fs.createReadStream).toHaveBeenCalledWith('test/file/path.jpg');
            expect(axios.post).toHaveBeenCalled();
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errorMessage
            });
        });

        test('should activate timer when credits go below max', async () => {
            // Arrange
            const mockUser = {
                _id: 'user-123',
                creditBalance: 5, // Max credits
                timerActive: false,
                nextCreditAt: null,
                lastCreditUpdate: new Date()
            };

            userModel.findOne.mockResolvedValue(mockUser);
            userModel.findByIdAndUpdate.mockResolvedValue(mockUser);

            // Мокуємо відповідь від API для видалення фону
            const mockBuffer = Buffer.from('fake-image-data');
            axios.post.mockResolvedValue({
                data: mockBuffer
            });

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(userModel.findByIdAndUpdate).toHaveBeenCalledWith(
                'user-123',
                expect.objectContaining({
                    creditBalance: 4, // Credits reduced by 1
                    timerActive: true, // Timer activated
                    nextCreditAt: expect.any(Date)
                })
            );

            expect(res.json).toHaveBeenCalledWith(
                expect.objectContaining({
                    success: true,
                    creditBalance: 4,
                    timerActive: true
                })
            );
        });
    });
});