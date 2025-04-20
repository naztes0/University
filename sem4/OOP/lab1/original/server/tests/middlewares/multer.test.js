import multer from 'multer';
import upload from '../../middlewares/multer.js';

// Mock multer
jest.mock('multer', () => {
    const mockStorage = {
        filename: jest.fn()
    };

    const mockDiskStorage = jest.fn().mockReturnValue(mockStorage);

    const mockMulter = jest.fn().mockReturnValue({
        single: jest.fn().mockReturnValue('mock-middleware')
    });

    mockMulter.diskStorage = mockDiskStorage;

    return mockMulter;
});

describe('Multer Middleware tests', () => {
    test('have to create storage with the correct filename function', () => {
        expect(multer.diskStorage).toHaveBeenCalled();

        const diskStorageArgs = multer.diskStorage.mock.calls[0][0];

        expect(typeof diskStorageArgs.filename).toBe('function');

        const mockReq = {};
        const mockFile = { originalname: 'test.jpg' };
        const mockCallback = jest.fn();

        diskStorageArgs.filename(mockReq, mockFile, mockCallback);

        expect(mockCallback).toHaveBeenCalledWith(
            null,
            expect.stringMatching(/^\d+_test\.jpg$/)
        );
    });

    test('have to return setup middleware multer', () => {
        expect(multer).toHaveBeenCalledWith({
            storage: expect.anything()
        });
    });
});