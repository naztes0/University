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
import userModel from '../../models/userModel.js';
import config from '../../configs/appConfig.js';

// Mock dependencies
jest.mock('../../models/userModel.js');
jest.mock('../../configs/appConfig.js');

describe('userService tests', () => {
    beforeEach(() => {
        jest.clearAllMocks();

        // Mock config values
        config.credits = {
            max: 5,
            refreshTimer: 2
        };
    });

    describe('User CRUD operations', () => {
        test('findUserByClerkId should retrieve a user with the specified clerkId', async () => {
            // Arrange
            const mockUser = { clerkId: 'clerk-123', email: 'test@example.com' };
            userModel.findOne.mockResolvedValue(mockUser);

            // Act
            const result = await findUserByClerkId('clerk-123');

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'clerk-123' });
            expect(result).toEqual(mockUser);
        });

        test('findUserByClerkId should return null when no user with the clerkId exists', async () => {
            // Arrange
            userModel.findOne.mockResolvedValue(null);

            // Act
            const result = await findUserByClerkId('nonexistent-id');

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'nonexistent-id' });
            expect(result).toBeNull();
        });

        test('createUser should persist a new user with the provided data', async () => {
            // Arrange
            const userData = {
                clerkId: 'clerk-123',
                email: 'test@example.com',
                firstName: 'Test',
                lastName: 'User',
                photo: 'https://example.com/photo.jpg'
            };

            const mockCreatedUser = { ...userData, _id: 'user-123' };
            userModel.create.mockResolvedValue(mockCreatedUser);

            // Act
            const result = await createUser(userData);

            // Assert
            expect(userModel.create).toHaveBeenCalledWith(userData);
            expect(result).toEqual(mockCreatedUser);
        });

        test('updateUser should modify an existing user with the new data', async () => {
            // Arrange
            const clerkId = 'clerk-123';
            const userData = {
                email: 'updated@example.com',
                firstName: 'Updated',
                lastName: 'User',
                photo: 'https://example.com/updated.jpg'
            };

            const mockUpdatedUser = {
                clerkId,
                ...userData,
                _id: 'user-123'
            };

            userModel.findOneAndUpdate.mockResolvedValue(mockUpdatedUser);

            // Act
            const result = await updateUser(clerkId, userData);

            // Assert
            expect(userModel.findOneAndUpdate).toHaveBeenCalledWith(
                { clerkId },
                userData,
                { new: true }
            );
            expect(result).toEqual(mockUpdatedUser);
        });

        test('deleteUser should remove a user with the specified clerkId', async () => {
            // Arrange
            const clerkId = 'clerk-123';
            const mockDeletedUser = {
                clerkId,
                email: 'test@example.com',
                _id: 'user-123'
            };

            userModel.findOneAndDelete.mockResolvedValue(mockDeletedUser);

            // Act
            const result = await deleteUser(clerkId);

            // Assert
            expect(userModel.findOneAndDelete).toHaveBeenCalledWith({ clerkId });
            expect(result).toEqual(mockDeletedUser);
        });
    });

    describe('Credit management operations', () => {
        test('checkAndUpdateCredits should disable timer when user has maximum credits', async () => {
            // Arrange
            const mockUser = {
                creditBalance: MAX_CREDITS,
                timerActive: true,
                nextCreditAt: new Date(),
                save: jest.fn()
            };

            // Act
            const result = await checkAndUpdateCredits(mockUser);

            // Assert
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.timerActive).toBe(false);
            expect(mockUser.nextCreditAt).toBeNull();
            expect(result).toEqual({
                credits: MAX_CREDITS,
                timerActive: false,
                nextCreditAt: null,
                timeRemaining: null
            });
        });

        test('checkAndUpdateCredits should maintain timer when active but not yet expired', async () => {
            // Arrange
            const futureDate = new Date();
            futureDate.setHours(futureDate.getHours() + 1);

            const mockUser = {
                creditBalance: 3,
                timerActive: true,
                nextCreditAt: futureDate,
                lastCreditUpdate: new Date(),
                save: jest.fn()
            };

            // Act
            const result = await checkAndUpdateCredits(mockUser);

            // Assert
            expect(mockUser.save).not.toHaveBeenCalled();
            expect(result).toEqual({
                credits: 3,
                timerActive: true,
                nextCreditAt: futureDate,
                timeRemaining: expect.any(Number)
            });
            expect(result.timeRemaining).toBeGreaterThan(0);
        });

        test('checkAndUpdateCredits should restore credits to maximum when timer expires and sufficient time has passed', async () => {
            // Arrange
            const dateNowInMinutes = Date.now() / (1000 * 60);
            const lastUpdate = new Date((dateNowInMinutes - CREDIT_REFRESH_TIME * (MAX_CREDITS + 1)) * 60 * 1000);

            const pastDate = new Date(lastUpdate.getTime() + CREDIT_REFRESH_TIME * 60 * 1000);


            const mockUser = {
                creditBalance: 2,
                timerActive: true,
                nextCreditAt: pastDate,
                lastCreditUpdate: lastUpdate,
                save: jest.fn()
            };

            // Act
            const result = await checkAndUpdateCredits(mockUser);

            // Assert
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.creditBalance).toBe(MAX_CREDITS);
            expect(mockUser.timerActive).toBe(false);
            expect(mockUser.nextCreditAt).toBeNull();
            expect(mockUser.lastCreditUpdate).toEqual(expect.any(Date));

            expect(result).toEqual({
                credits: MAX_CREDITS,
                timerActive: false,
                nextCreditAt: null,
                timeRemaining: null
            });
        });

        test('checkAndUpdateCredits should increment credits incrementally based on elapsed time when timer expires', async () => {
            // Arrange
            const dateNowInMinutes = Date.now() / (1000 * 60);
            const lastUpdate = new Date((dateNowInMinutes - CREDIT_REFRESH_TIME * 1.5) * 60 * 1000);

            const pastDate = new Date(lastUpdate.getTime() + CREDIT_REFRESH_TIME * 60 * 1000);


            const mockUser = {
                creditBalance: 2,
                timerActive: true,
                nextCreditAt: pastDate,
                lastCreditUpdate: lastUpdate,
                save: jest.fn()
            };

            // Act
            const result = await checkAndUpdateCredits(mockUser);

            // Assert
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.creditBalance).toBe(3);
            expect(mockUser.timerActive).toBe(true);
            expect(mockUser.nextCreditAt).toEqual(expect.any(Date));
            expect(mockUser.lastCreditUpdate).toEqual(expect.any(Date));

            expect(result).toEqual({
                credits: 3,
                timerActive: true,
                nextCreditAt: expect.any(Date),
                timeRemaining: expect.any(Number)
            });
        });

        test('manageCreditTimer should disable timer when user has maximum credits', async () => {
            // Arrange
            const mockUser = {
                creditBalance: MAX_CREDITS,
                timerActive: true,
                nextCreditAt: new Date(),
                save: jest.fn()
            };

            // Act
            const result = await manageCreditTimer(mockUser);

            // Assert
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.timerActive).toBe(false);
            expect(mockUser.nextCreditAt).toBeNull();

            expect(result).toEqual({
                success: true,
                message: 'Max credits reached, timer disabled',
                timerActive: false,
                creditBalance: MAX_CREDITS
            });
        });

        test('manageCreditTimer should activate timer when credits are below maximum and timer is inactive', async () => {
            // Arrange
            const mockUser = {
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null,
                save: jest.fn()
            };

            // Act
            const result = await manageCreditTimer(mockUser);

            // Assert
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.timerActive).toBe(true);
            expect(mockUser.nextCreditAt).toEqual(expect.any(Date));
            expect(mockUser.lastCreditUpdate).toEqual(expect.any(Date));

            expect(result).toEqual({
                success: true,
                message: 'Timer activated',
                timerActive: true,
                nextCreditAt: expect.any(Date),
                creditBalance: 3
            });
        });

        test('manageCreditTimer should leave timer unchanged when already running', async () => {
            // Arrange
            const futureDate = new Date();
            futureDate.setMinutes(futureDate.getMinutes() + 30);

            const mockUser = {
                creditBalance: 3,
                timerActive: true,
                nextCreditAt: futureDate,
                save: jest.fn()
            };

            // Act
            const result = await manageCreditTimer(mockUser);

            // Assert
            expect(mockUser.save).not.toHaveBeenCalled();

            expect(result).toEqual({
                success: true,
                message: 'Timer status checked',
                timerActive: true,
                nextCreditAt: futureDate,
                creditBalance: 3
            });
        });
    });
});