import express from "express"
import { clerkWebhooks, userCredits, checkCreditTimer } from "../controllers/UserController.js"
import authUser from "../middlewares/auth.js"

const userRouter=express.Router()

userRouter.post('/webhooks',clerkWebhooks)
userRouter.get("/credits", authUser, userCredits)
userRouter.post("/check-timer", authUser, checkCreditTimer)

export default userRouter

