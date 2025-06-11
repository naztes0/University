import mongoose from "mongoose";
import config from "./appConfig.js";

let connection = null;
let isConnecting = false;

export const getConnection = async () => {
    if (connection && mongoose.connection.readyState === 1) {
        return connection;
    }

    if (isConnecting) {
        // If a connection attempt is in progress, wait until it completes
        while (isConnecting) {
            await new Promise(resolve => setTimeout(resolve, 100));
        }
        return connection;
    }

    isConnecting = true;

    try {
        await mongoose.connect(`${config.db.uri}/${config.db.name}`, config.db.options);
        connection = mongoose.connection;
        console.log("Database connected");
        return connection;
    } finally {
        isConnecting = false;
    }
};