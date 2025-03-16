import mongoose from "mongoose";

const connectDB = async () => {
    const isProduction = process.env.NODE_ENV === 'production';
    
    // Більш жорсткі таймаути для Vercel
    const options = {
        serverSelectionTimeoutMS: isProduction ? 3000 : 5000,
        socketTimeoutMS: isProduction ? 5000 : 30000,
        connectTimeoutMS: isProduction ? 5000 : 30000,
        ssl: true,
        retryWrites: true,
        w: "majority"
    };
    
    try {
        await mongoose.connect(process.env.MONGO_URI, options);
        return true;
    } catch (error) {
        console.error("Failed to connect to MongoDB:", error);
        // В production не завершуємо процес при помилці підключення
        if (!isProduction) {
            process.exit(1);
        }
        return false;
    }
};

export default connectDB;