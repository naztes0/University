import { userCredits, clerkWebhooks, checkCreditTimer } from '../../controllers/UserController.js';
import {
    findUserByClerkId,
    createUser,
    updateUser,
    deleteUser,
    checkAndUpdateCredits,
    manageCreditTimer,
    MAX_CREDITS,
    CREDIT_REFRESH_TIME
} from '../../services/userService.js';
import { Webhook } from 'svix';
import config from '../../configs/appConfig.js';

// Mock dependencies
jest.mock('../../services/userService.js');
jest.mock('svix');
jest.mock('../../configs/appConfig.js', () => ({

    apiKeys: {
        clerk: {
            webhookSecret: 'test-webhook-secret'
        }
    },
    credits: {
        max: 5,
        refreshTimer: 2
    }
}));

describe('UserController test', () => {
    let req, res;

    beforeEach(() => {
        jest.clearAllMocks();
        req = {
            body: {
                clerkId: 'test-clerk-id'
            },
            headers: {
                'svix-id': 'test-svix-id',
                'svix-timestamp': 'test-timestamp',
                'svix-signature': 'test-signature'
            }
        };
        res = {
            json: jest.fn().mockReturnThis()
        };
    });

    describe('userCredits function', () => {
        test('should return user credits when user is found', async () => {
            // Arrange
            const mockUser = {
                _id: 'user-123',
                creditBalance: 3,
                timerActive: true,
                nextCreditAt: new Date(),
                lastCreditUpdate: new Date()
            };

            const mockCreditInfo = {
                credits: 3,
                timerActive: true,
                nextCreditAt: new Date(),
                timeRemaining: 120000 // 2 minutes in ms
            };

            findUserByClerkId.mockResolvedValue(mockUser);
            checkAndUpdateCredits.mockResolvedValue(mockCreditInfo);

            // Act
            await userCredits(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(checkAndUpdateCredits).toHaveBeenCalledWith(mockUser);
            expect(res.json).toHaveBeenCalledWith({
                success: true,
                ...mockCreditInfo
            });
        });

        test('should handle user not found error', async () => {
            // Arrange
            findUserByClerkId.mockResolvedValue(null);

            // Act
            await userCredits(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(checkAndUpdateCredits).not.toHaveBeenCalled();
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: 'User not found'
            });
        });

        test('should handle service errors', async () => {
            // Arrange
            const errorMessage = 'Service error';
            findUserByClerkId.mockRejectedValue(new Error(errorMessage));

            // Act
            await userCredits(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errorMessage
            });
        });
    });

    describe('clerkWebhooks function', () => {
        test('should create new user when webhook type is user.created', async () => {
            // Arrange
            req.body = {
                type: 'user.created',
                data: {
                    id: 'clerk-123',
                    email_addresses: [{ email_address: 'test@example.com' }],
                    first_name: 'Test',
                    last_name: 'User',
                    profile_image_url: 'https://example.com/photo.jpg'
                }
            };

            const verifyMock = jest.fn();
            Webhook.mockImplementation(() => ({
                verify: verifyMock
            }));

            createUser.mockResolvedValue({
                clerkId: 'clerk-123',
                email: 'test@example.com',
                firstName: 'Test',
                lastName: 'User',
                photo: 'https://example.com/photo.jpg'
            });

            // Act
            await clerkWebhooks(req, res);

            // Assert
            expect(Webhook).toHaveBeenCalledWith(config.apiKeys.clerk.webhookSecret);
            expect(verifyMock).toHaveBeenCalledWith(
                JSON.stringify(req.body),
                {
                    'svix-id': 'test-svix-id',
                    'svix-timestamp': 'test-timestamp',
                    'svix-signature': 'test-signature'
                }
            );
            expect(createUser).toHaveBeenCalledWith({
                clerkId: 'clerk-123',
                email: 'test@example.com',
                firstName: 'Test',
                lastName: 'User',
                photo: 'https://example.com/photo.jpg'
            });
            expect(res.json).toHaveBeenCalledWith({});
        });

        test('should update user when webhook type is user.updated', async () => {
            // Arrange
            req.body = {
                type: 'user.updated',
                data: {
                    id: 'clerk-123',
                    email_addresses: [{ email_address: 'updated@example.com' }],
                    first_name: 'Updated',
                    last_name: 'User',
                    profile_image_url: 'https://example.com/updated.jpg'
                }
            };

            const verifyMock = jest.fn();
            Webhook.mockImplementation(() => ({
                verify: verifyMock
            }));

            updateUser.mockResolvedValue({
                clerkId: 'clerk-123',
                email: 'updated@example.com',
                firstName: 'Updated',
                lastName: 'User',
                photo: 'https://example.com/updated.jpg'
            });

            // Act
            await clerkWebhooks(req, res);

            // Assert
            expect(Webhook).toHaveBeenCalledWith(config.apiKeys.clerk.webhookSecret);
            expect(verifyMock).toHaveBeenCalled();
            expect(updateUser).toHaveBeenCalledWith(
                'clerk-123',
                {
                    email: 'updated@example.com',
                    firstName: 'Updated',
                    lastName: 'User',
                    photo: 'https://example.com/updated.jpg'
                }
            );
            expect(res.json).toHaveBeenCalledWith({});
        });

        test('should delete user when webhook type is user.deleted', async () => {
            // Arrange
            req.body = {
                type: 'user.deleted',
                data: {
                    id: 'clerk-123'
                }
            };

            const verifyMock = jest.fn();
            Webhook.mockImplementation(() => ({
                verify: verifyMock
            }));

            deleteUser.mockResolvedValue({ acknowledged: true });

            // Act
            await clerkWebhooks(req, res);

            // Assert
            expect(Webhook).toHaveBeenCalledWith(config.apiKeys.clerk.webhookSecret);
            expect(verifyMock).toHaveBeenCalled();
            expect(deleteUser).toHaveBeenCalledWith('clerk-123');
            expect(res.json).toHaveBeenCalledWith({});
        });

        test('should handle webhook verification error', async () => {
            // Arrange
            const errorMessage = 'Webhook verification failed';
            const verifyMock = jest.fn().mockRejectedValue(new Error(errorMessage));

            Webhook.mockImplementation(() => ({
                verify: verifyMock
            }));

            // Act
            await clerkWebhooks(req, res);

            // Assert
            expect(Webhook).toHaveBeenCalledWith(config.apiKeys.clerk.webhookSecret);
            expect(verifyMock).toHaveBeenCalled();
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errorMessage
            });
        });
    });

    describe('checkCreditTimer function', () => {
        test('should call manageCreditTimer service and return result', async () => {
            // Arrange
            const mockUser = {
                _id: 'user-123',
                creditBalance: 3,
                timerActive: true,
                nextCreditAt: new Date()
            };

            const mockTimerResult = {
                success: true,
                message: 'Timer status checked',
                timerActive: true,
                nextCreditAt: new Date(),
                creditBalance: 3
            };

            findUserByClerkId.mockResolvedValue(mockUser);
            manageCreditTimer.mockResolvedValue(mockTimerResult);

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(manageCreditTimer).toHaveBeenCalledWith(mockUser);
            expect(res.json).toHaveBeenCalledWith(mockTimerResult);
        });

        test('should handle user not found error', async () => {
            // Arrange
            findUserByClerkId.mockResolvedValue(null);

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(manageCreditTimer).not.toHaveBeenCalled();
        });
        test('should handle service errors', async () => {
            // Arrange
            const mockUser = {
                _id: 'user-123',
                creditBalance: 3
            };
            const errorMessage = 'Service error';

            findUserByClerkId.mockResolvedValue(mockUser);
            manageCreditTimer.mockRejectedValue(new Error(errorMessage));

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(findUserByClerkId).toHaveBeenCalledWith('test-clerk-id');
            expect(manageCreditTimer).toHaveBeenCalledWith(mockUser);
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errorMessage
            });
        });
    });
});

