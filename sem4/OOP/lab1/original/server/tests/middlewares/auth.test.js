import authUser from '../../middlewares/auth.js';
import jwt from 'jsonwebtoken';

// Мокуємо залежності
jest.mock('jsonwebtoken');

describe('Auth Middleware test', () => {
    let req, res, next;

    beforeEach(() => {
        jest.clearAllMocks();

        req = {
            headers: {
                token: 'test-token'
            },
            body: {}
        };

        res = {
            json: jest.fn().mockReturnThis()
        };

        next = jest.fn();
    });

    test('should add clerkId to request body and call next', async () => {
        // Arrange
        jwt.decode.mockReturnValue({ clerkId: 'test-clerk-id' });

        // Act
        await authUser(req, res, next);

        // Assert
        expect(jwt.decode).toHaveBeenCalledWith('test-token');
        expect(req.body.clerkId).toBe('test-clerk-id');
        expect(next).toHaveBeenCalled();
        expect(res.json).not.toHaveBeenCalled();
    });

    test('should return error response when token is missing', async () => {
        // Arrange
        req.headers.token = undefined;

        // Act
        await authUser(req, res, next);

        // Assert
        expect(jwt.decode).not.toHaveBeenCalled();
        expect(next).not.toHaveBeenCalled();
        expect(res.json).toHaveBeenCalledWith({
            success: false,
            message: 'Not authorized login again'
        });
    });

    test('should handle jwt decode error', async () => {
        // Arrange
        const errorMessage = 'Invalid token';
        jwt.decode.mockImplementation(() => {
            throw new Error(errorMessage);
        });

        // Act
        await authUser(req, res, next);

        // Assert
        expect(jwt.decode).toHaveBeenCalledWith('test-token');
        expect(next).not.toHaveBeenCalled();
        expect(res.json).toHaveBeenCalledWith({
            success: false,
            message: errorMessage
        });
    });
});