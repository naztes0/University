import mongoose from "mongoose";


const userSchema = new mongoose.Schema({
    clerkId: { type: String, required: true, unique: true },
    email: { type: String, required: true, unique: true },
    photo: { type: String, required: true },
    firstName: { type: String },
    lastName: { type: String },
    creditBalance: { type: Number, default: 5 },
    nextCreditAt: { type: Date, default: null },
    timerActive: { type: Boolean, default: false },
    lastCreditUpdate: { type: Date, default: Date.now }
});

const userModel=mongoose.models.user||mongoose.model("user",userSchema);

export default userModel;