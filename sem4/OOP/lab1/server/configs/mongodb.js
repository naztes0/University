import mongoose from "mongoose";

const connectDB = async () => {
    try {
        mongoose.connection.on("connected", () => {
            console.log("Database connected");
        });
        
        const options = {
            serverSelectionTimeoutMS: 5000,
            connectTimeoutMS: 5000
        };
        
        await mongoose.connect(process.env.MONGODB_URI, options);
        return true;
    } catch (error) {
        console.error("MongoDB connection error:", error);
        return false; 
    }
}
export default connectDB