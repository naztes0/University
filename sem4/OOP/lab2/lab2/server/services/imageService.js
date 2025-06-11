import axios from "axios";
import fs from "fs";
import FormData from "form-data";
import userModel from "../models/userModel.js";
import { MAX_CREDITS, CREDIT_REFRESH_TIME } from "./userService.js";
import config from "../configs/appConfig.js";

// Function to remove background from an image 
export const removeImageBackground = async (user, imagePath) => {
    try {
        // File stream for the image
        const imageFile = fs.createReadStream(imagePath);
        const formdata = new FormData();
        formdata.append('image_file', imageFile);

        // Request to ClipDrop API for background removal
        const { data } = await axios.post('https://clipdrop-api.co/remove-background/v1', formdata, {
            headers: {
                'x-api-key': config.apiKeys.clipdrop,
            },
            responseType: "arraybuffer"
        });

        // Reuslt in base64 format
        const base64Image = Buffer.from(data, "binary").toString("base64");
        const resultImage = `data: ${user.mime};base64,${base64Image}`;

        const newCreditBalance = user.creditBalance - 1;
        //Cehck for timer 
        let timerActive = user.timerActive;
        let nextCreditAt = user.nextCreditAt;

        // If credits not maxed out, set timer for next credit
        if (newCreditBalance < MAX_CREDITS && !timerActive) {
            timerActive = true;
            nextCreditAt = new Date(new Date().getTime() + CREDIT_REFRESH_TIME * 60 * 1000);
        }

        // Upd user info
        await userModel.findByIdAndUpdate(user._id, {
            creditBalance: newCreditBalance,
            timerActive,
            nextCreditAt,
            lastCreditUpdate: new Date()
        });

        return {
            resultImage,
            creditBalance: newCreditBalance,
            timerActive,
            nextCreditAt
        };
    } catch (error) {
        throw new Error(`Error removing background: ${error.message}`);
    }
};