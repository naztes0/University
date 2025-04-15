import { userCredits } from '../../controllers/UserController.js';
import userModel from '../../models/userModel.js';
import mongoose from 'mongoose';
import jestConfig from '../../jest.config.js';

//mock the userModel
jest.mock('../models/userModel.js');

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
    })
})