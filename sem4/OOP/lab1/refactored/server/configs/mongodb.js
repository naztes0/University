import mongoose from "mongoose";
import config from "./appConfig.js";


const connectDB = async () => {
    try {
        mongoose.connection.on("connected", () => {
            console.log("Database connected");
        });
        await mongoose.connect(`${config.db.uri}/${config.db.name}`, config.db.options);
        return true;
    } catch (error) {
        console.error("MongoDB connection error:", error);
        return false;
    }
}
export default connectDB