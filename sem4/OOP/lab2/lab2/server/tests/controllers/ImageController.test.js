import { removeBgImage } from '../../controllers/ImageController.js';
import { findUserByClerkId } from '../../services/userService.js';
import { removeImageBackground } from '../../services/imageService.js';
import config from '../../configs/appConfig.js';

// Mock dependencies
jest.mock('../../services/userService.js');
jest.mock('../../services/imageService.js');
jest.mock('../../configs/appConfig.js', () => ({

    apiKeys: {
        clipdrop: 'test-clipdrop-api-key'
    },
    credits: {
        max: 5,
        refreshTimer: 2
    }

}));

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

            const mockResult = {
                resultImage: 'base64-image-data',
                creditBalance: 2,
                timerActive: true,
                nextCreditAt: new Date()
            };

            findUserByClerkId.mockResolvedValue(mockUser);
            removeImageBackground.mockResolvedValue(mockResult);

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(removeImageBackground).toHaveBeenCalledWith(
                expect.objectContaining({
                    _id: 'user-123',
                    creditBalance: 3,
                    mime: 'image/jpeg'
                }),
                'test/file/path.jpg'
            );

            expect(res.json).toHaveBeenCalledWith({
                success: true,
                resultImage: 'base64-image-data',
                creditBalance: 2,
                timerActive: true,
                nextCreditAt: expect.any(Date),
                message: 'Background Removed'
            });
        });

        test('should handle user not found error', async () => {
            // Arrange
            findUserByClerkId.mockResolvedValue(null);

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: 'User not found'
            });
            expect(removeImageBackground).not.toHaveBeenCalled();
        });

        test('should handle insufficient credits error', async () => {
            // Arrange
            const mockUser = {
                creditBalance: 0,
                timerActive: false
            };

            findUserByClerkId.mockResolvedValue(mockUser);

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: 'No credit balance',
                creditBalance: 0
            });
            expect(removeImageBackground).not.toHaveBeenCalled();
        });

        test('should handle image service error', async () => {
            // Arrange
            const mockUser = {
                _id: 'user-123',
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null
            };

            findUserByClerkId.mockResolvedValue(mockUser);

            const errorMessage = 'Error removing background';
            removeImageBackground.mockRejectedValue(new Error(errorMessage));

            // Act
            await removeBgImage(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(removeImageBackground).toHaveBeenCalled();
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errorMessage
            });
        });
    });
});