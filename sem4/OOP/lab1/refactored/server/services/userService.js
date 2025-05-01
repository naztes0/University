import userModel from "../models/userModel.js";
import config from "../configs/appConfig.js";


// Global constants for managing user credits
export const { max: MAX_CREDITS, refreshTimer: CREDIT_REFRESH_TIME } = config.credits;

// Find a user 
export const findUserByClerkId = async (clerkId) => {
    return await userModel.findOne({ clerkId });
};

// Create a new user
export const createUser = async (userData) => {
    return await userModel.create(userData);
};

// Update user info
export const updateUser = async (clerkId, userData) => {
    return await userModel.findOneAndUpdate({ clerkId }, userData, { new: true });
};

// Delete user
export const deleteUser = async (clerkId) => {
    return await userModel.findOneAndDelete({ clerkId });
};

// Check and update user's credit balance
export const checkAndUpdateCredits = async (user) => {
    // If user has max credits, ensure the timer is off
    if (user.creditBalance >= MAX_CREDITS) {
        if (user.timerActive) {
            user.timerActive = false;
            user.nextCreditAt = null;
            await user.save();
        }

        return {
            credits: user.creditBalance,
            timerActive: false,
            nextCreditAt: null,
            timeRemaining: null
        };
    }

    // Check if it's time to add credits
    if (user.timerActive && user.nextCreditAt && new Date() >= new Date(user.nextCreditAt)) {
        // Calculate how many credits to add based on elapsed time
        const timePassedSinceLastUpdate = new Date() - new Date(user.lastCreditUpdate);
        const minutesPassedSinceLastUpdate = timePassedSinceLastUpdate / (1000 * 60);
        const creditsToAdd = Math.min(
            Math.floor(minutesPassedSinceLastUpdate / CREDIT_REFRESH_TIME),
            MAX_CREDITS - user.creditBalance
        );

        if (creditsToAdd > 0) {
            user.creditBalance += creditsToAdd;
            user.lastCreditUpdate = new Date();

            // If max credits reached, deactivate timer
            if (user.creditBalance >= MAX_CREDITS) {
                user.timerActive = false;
                user.nextCreditAt = null;
            } else {
                // Set time for next credit
                user.nextCreditAt = new Date(new Date().getTime() + CREDIT_REFRESH_TIME * 60 * 1000);
            }

            await user.save();
        }
    }

    // Calculate remaining time if timer is active
    let timeRemaining = null;
    if (user.timerActive && user.nextCreditAt) {
        timeRemaining = Math.max(0, new Date(user.nextCreditAt) - new Date());
    }

    return {
        credits: user.creditBalance,
        timerActive: user.timerActive,
        nextCreditAt: user.nextCreditAt,
        timeRemaining
    };
};

// Manage credit refill timer for a user
export const manageCreditTimer = async (user) => {
    // If user has max credits, ensure the timer is off
    if (user.creditBalance >= MAX_CREDITS) {
        if (user.timerActive) {
            user.timerActive = false;
            user.nextCreditAt = null;
            await user.save();
        }

        return {
            success: true,
            message: "Max credits reached, timer disabled",
            timerActive: false,
            creditBalance: user.creditBalance
        };
    }

    // Activate the timer if it's turned off and user has less than max credits
    if (user.creditBalance < MAX_CREDITS && !user.timerActive) {
        // Set time for next credit
        user.nextCreditAt = new Date(new Date().getTime() + CREDIT_REFRESH_TIME * 60 * 1000);
        user.timerActive = true;
        user.lastCreditUpdate = new Date();
        await user.save();

        return {
            success: true,
            message: "Timer activated",
            timerActive: true,
            nextCreditAt: user.nextCreditAt,
            creditBalance: user.creditBalance
        };
    }

    // Timer is already running correctly
    return {
        success: true,
        message: "Timer status checked",
        timerActive: user.timerActive,
        nextCreditAt: user.nextCreditAt,
        creditBalance: user.creditBalance
    };
};
