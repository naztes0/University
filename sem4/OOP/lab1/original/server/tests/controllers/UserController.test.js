import { userCredits, clerkWebhooks, checkCreditTimer } from '../../controllers/UserController.js';
import userModel from '../../models/userModel.js';
import { Webhook } from 'svix';
import mongoose from 'mongoose';
import jestConfig from '../../jest.config.js';

//mock the userModel
jest.mock('../../models/userModel.js');
jest.mock('svix')

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

    })

    describe('userCredits function', () => {
        test('should return user credits when timer is inactive', async () => {

            const mockUser = {
                creditBalance: 5,
                timerActive: false,
                nextCreditAt: null,
                lastCreditUPdate: new Date(),
                save: jest.fn()
            };
            userModel.findOne.mockResolvedValue(mockUser);

            // Act
            await userCredits(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(res.json).toHaveBeenCalledWith({
                success: true,
                credits: 5,
                timerActive: false,
                nextCreditAt: null,
                timeRemaining: null
            });
            expect(mockUser.save).not.toHaveBeenCalled();
        });
        test('should not add credits when timer is active but not expired', async () => {

            const futureDate = new Date();
            futureDate.setHours(futureDate.getHours() + 1); // Set the future date to 1 hour from now

            const mockUser = {
                creditBalance: 2,
                timerActive: true,
                nextCreditAt: futureDate,
                lastCreditUPdate: new Date(),
                save: jest.fn()
            }
            userModel.findOne.mockResolvedValue(mockUser);
            //Act
            await userCredits(req, res);

            //Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(mockUser.save).not.toHaveBeenCalled();
            expect(res.json).toHaveBeenCalledWith({
                success: true,
                credits: 2,
                timerActive: true,
                nextCreditAt: futureDate,
                timeRemaining: expect.any(Number)
            });

        });
        test('should add credits when timer has expired', async () => {
            const pastDate = new Date();
            pastDate.setMinutes(pastDate.getMinutes() - 4); // Set to  4 Minutes in past

            const lastUpdate = new Date();
            lastUpdate.setMinutes(lastUpdate.getMinutes() - 6); // Last update 6 Minutes ago
            const mockUser = {
                creditBalance: 2,
                timerActive: true,
                nextCreditAt: pastDate,
                lastCreditUpdate: lastUpdate,
                save: jest.fn()
            }

            userModel.findOne.mockResolvedValue(mockUser);
            //Act
            await userCredits(req, res);

            //Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.creditBalance).toBe(4);
            expect(mockUser.timerActive).toBe(true);
            expect(mockUser.lastCreditUpdate).toEqual(expect.any(Date));
            expect(mockUser.nextCreditAt).toEqual(expect.any(Date));
        })

        test('error handling', async () => {
            const errormessage = 'Error occurred';
            userModel.findOne.mockRejectedValue(new Error(errormessage));
            //Act
            await userCredits(req, res);
            //Assert
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errormessage
            })
        });
        test('timer deactivated when max credits reached', async () => {
            const pastDate = new Date();
            pastDate.setMinutes(pastDate.getMinutes() - 4); // Set to 4 Minutes in past
            const lastUpdate = new Date();
            lastUpdate.setMinutes(lastUpdate.getMinutes() - 9);

            const mockUser = {
                creditBalance: 4,
                timerActive: true,
                nextCreditAt: pastDate,
                lastCreditUpdate: lastUpdate,
                save: jest.fn()
            }
            userModel.findOne.mockResolvedValue(mockUser);

            //Act
            await userCredits(req, res);

            //Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(mockUser.save).toHaveBeenCalled()
            expect(mockUser.creditBalance).toBe(5);
            expect(mockUser.timerActive).toBe(false);
            expect(mockUser.nextCreditAt).toBe(null);
            expect(mockUser.lastCreditUpdate).toEqual(expect.any(Date));


        })
    })

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

            // Act
            await clerkWebhooks(req, res);

            // Assert
            expect(Webhook).toHaveBeenCalledWith(process.env.CLERK_WEBHOOK_SECRET);
            expect(verifyMock).toHaveBeenCalled();
            expect(userModel.create).toHaveBeenCalledWith({
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

            // Act
            await clerkWebhooks(req, res);

            // Assert
            expect(Webhook).toHaveBeenCalledWith(process.env.CLERK_WEBHOOK_SECRET);
            expect(verifyMock).toHaveBeenCalled();
            expect(userModel.findOneAndUpdate).toHaveBeenCalledWith(
                { clerkId: 'clerk-123' },
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
            req.body = {
                type: 'user.deleted',
                data: {
                    id: 'clerk-123'
                }
            }

            const verifyMock = jest.fn()
            Webhook.mockImplementation(() => ({
                verify: verifyMock,
            }));
            // Act
            await clerkWebhooks(req, res);
            // Assert
            expect(Webhook).toHaveBeenCalledWith(process.env.CLERK_WEBHOOK_SECRET)
            expect(verifyMock).toHaveBeenCalled();
            expect(userModel.findOneAndDelete).toHaveBeenCalledWith({ clerkId: 'clerk-123' })
            expect(res.json).toHaveBeenCalledWith({});
        })

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
            expect(Webhook).toHaveBeenCalledWith(process.env.CLERK_WEBHOOK_SECRET);
            expect(verifyMock).toHaveBeenCalled();
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errorMessage
            });
        });
    });

    describe('checkCreditTimer function', () => {
        test('should return max credits message when credits are full', async () => {
            // Arrange
            const mockUser = {
                creditBalance: 5,
                timerActive: true,
                nextCreditAt: new Date(),
                save: jest.fn()
            };

            userModel.findOne.mockResolvedValue(mockUser);

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.timerActive).toBe(false);
            expect(mockUser.nextCreditAt).toBe(null);
            expect(res.json).toHaveBeenCalledWith({
                success: true,
                message: 'Max credits reached, timer disabled',
                timerActive: false,
                creditBalance: 5
            });
        });

        test('should activate timer when credits are below max and timer is inactive', async () => {
            // Arrange
            const mockUser = {
                creditBalance: 3,
                timerActive: false,
                nextCreditAt: null,
                save: jest.fn()
            };

            userModel.findOne.mockResolvedValue(mockUser);

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(mockUser.save).toHaveBeenCalled();
            expect(mockUser.timerActive).toBe(true);
            expect(mockUser.nextCreditAt).toEqual(expect.any(Date));
            expect(mockUser.lastCreditUpdate).toEqual(expect.any(Date));
            expect(res.json).toHaveBeenCalledWith({
                success: true,
                message: 'Timer activated',
                timerActive: true,
                nextCreditAt: expect.any(Date),
                creditBalance: 3
            });
        });

        test('should return timer status when timer is already running', async () => {
            // Arrange
            const nextCreditAt = new Date();
            nextCreditAt.setMinutes(nextCreditAt.getMinutes() + 30);

            const mockUser = {
                creditBalance: 3,
                timerActive: true,
                nextCreditAt: nextCreditAt,
                save: jest.fn()
            };

            userModel.findOne.mockResolvedValue(mockUser);

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(mockUser.save).not.toHaveBeenCalled();
            expect(res.json).toHaveBeenCalledWith({
                success: true,
                message: 'Timer status checked',
                timerActive: true,
                nextCreditAt: nextCreditAt,
                creditBalance: 3
            });
        });

        test('should handle user not found error', async () => {
            // Arrange
            userModel.findOne.mockResolvedValue(null);

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: 'User not found'
            });
        });

        test('should handle general errors', async () => {
            // Arrange
            const errorMessage = 'Database error';
            userModel.findOne.mockRejectedValue(new Error(errorMessage));

            // Act
            await checkCreditTimer(req, res);

            // Assert
            expect(userModel.findOne).toHaveBeenCalledWith({ clerkId: 'test-clerk-id' });
            expect(res.json).toHaveBeenCalledWith({
                success: false,
                message: errorMessage
            });
        });
    });
})