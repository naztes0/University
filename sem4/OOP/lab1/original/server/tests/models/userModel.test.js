import mongoose from 'mongoose';
import userModel from '../../models/userModel.js';

// Mocking mongoose
jest.mock('mongoose', () => {
    const mockSchemaInstance = {
    };

    const Schema = jest.fn().mockReturnValue(mockSchemaInstance);

    return {
        Schema,
        models: { user: null },
        model: jest.fn().mockImplementation((name, schema) => {
            const mockModel = {
                create: jest.fn().mockImplementation(userData => {
                    if (!userData.clerkId || !userData.email || !userData.photo) {
                        return Promise.reject(new Error('Обов\'язкові поля відсутні'));
                    }

                    return Promise.resolve({
                        ...userData,
                        creditBalance: userData.creditBalance || 5,
                        timerActive: userData.timerActive || false,
                        nextCreditAt: userData.nextCreditAt || null,
                        lastCreditUpdate: userData.lastCreditUpdate || new Date()
                    });
                }),
                findOne: jest.fn().mockImplementation(query => {
                    if (query.clerkId === 'duplicate-clerk-id' || query.email === 'duplicate@example.com') {
                        return Promise.resolve({
                            clerkId: 'duplicate-clerk-id',
                            email: 'duplicate@example.com'
                        });
                    }
                    return Promise.resolve(null);
                }),
                deleteMany: jest.fn().mockResolvedValue({}),
            };

            return mockModel;
        })
    };
});

describe('User Model tests', () => {
    beforeEach(() => {
        jest.clearAllMocks();
    });

    test('have to create new document with correct fields', async () => {
        // Arrange
        const userData = {
            clerkId: 'test-clerk-id',
            email: 'test@example.com',
            photo: 'https://example.com/photo.jpg',
            firstName: 'Test',
            lastName: 'User'
        };

        // Act
        const newUser = await userModel.create(userData);

        // Assert
        expect(newUser.clerkId).toBe(userData.clerkId);
        expect(newUser.email).toBe(userData.email);
        expect(newUser.photo).toBe(userData.photo);
        expect(newUser.firstName).toBe(userData.firstName);
        expect(newUser.lastName).toBe(userData.lastName);
        expect(newUser.creditBalance).toBe(5);
        expect(newUser.timerActive).toBe(false);
        expect(newUser.nextCreditAt).toBe(null);
        expect(newUser.lastCreditUpdate).toBeDefined();
    });

    test('have to reject document that don\'t contain required fields', async () => {
        // Arrange
        const incompleteData = {
            clerkId: 'test-clerk-id',
            firstName: 'Test',
            lastName: 'User'
        };

        // Act & Assert
        await expect(userModel.create(incompleteData)).rejects.toThrow('Обов\'язкові поля відсутні');
    });

    test('should find an existing user by clerkId', async () => {
        // Arrange
        const userData = {
            clerkId: 'duplicate-clerk-id',
            email: 'duplicate@example.com',
            photo: 'https://example.com/photo.jpg',
            firstName: 'Test',
            lastName: 'User'
        };

        // Act
        const existingUser = await userModel.findOne({ clerkId: userData.clerkId });

        // Assert
        expect(existingUser).not.toBeNull();
        expect(existingUser.clerkId).toBe('duplicate-clerk-id');
    });
});