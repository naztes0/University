import { userCredits } from '../../controllers/UserController.js';
import userModel from '../../models/userModel.js';
import mongoose from 'mongoose';
import jestConfig from '../../jest.config.js';

//mock the userModel
jest.mock('../../models/userModel.js');

describe('UserController test', () => {
    let req, res;

    beforeEach(() => {
        jest.clearAllMocks();
        req = {
            body: {
                clerkId: 'test-clerk-id'
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

        test('error habdling', async () => {
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
})