
import { Webhook } from "svix"
import userModel from "../models/userModel.js"
//API controller Function to manage clerk user with database

//http://localhost4000/api/user/webhooks

const clerkWebhooks=async(req,res)=>{
    try {
        console.log("Webhook received:", req.body.type);
        console.log("Webhook data:", JSON.stringify(req.body.data));
        //Create a Svix instance with clerk webhook secret
        const whook=new Webhook(process.env.CLERK_WEBHOOK_SECRET)

        await whook.verify(JSON.stringify(req.body),{
            "svix-id":req.headers["svix-id"],
            "svix-timestamp":req.headers["svix-timestamp"],
            "svix-signature":req.headers["svix-signature"]
        })
        const {data,type}=req.body

        switch (type) {
            case "user.created":{
                const userData={
                    clerkId:data.id,
                    email: data.email_addresses[0].email_address,
                    firstName:data.first_name,
                    lastName:data.last_name,
                    photo:data.profile_image_url || data.image_url
                 
                }
                console.log("Creating user with data:", userData);
                await userModel.create(userData)
                res.json({})
                
                break;
            }
            case "user.updated":{
                const userData={
                    email: data.email_addresses[0].email_address,
                    firstName:data.first_name,
                    lastName:data.last_name,
                    photo:data.profile_image_url || data.image_url
                }
                await userModel.findOneAndUpdate({clerkId:data.id},userData)
                res.json({})
                break;
            }
            case "user.deleted":{
                await userModel.findOneAndDelete({clerkId:data.id})
                res.json({})
                break;
            }
            default:
                break;
        }


    } catch (error) {
       console.log(error.message)
       res.json({success:false, message:error.message})
    }
}


//Api controller function to get user available credits data 
const userCredits = async (req, res) => {
    try {
        const { clerkId } = req.body
        const userData = await userModel.findOne({ clerkId })
        
        // Check if credits should be updated
        if (userData.timerActive && userData.nextCreditAt && new Date() >= new Date(userData.nextCreditAt)) {
            // Calculate how many credits to add based on time passed
            const timePassedSinceLastUpdate = new Date() - new Date(userData.lastCreditUpdate);
            const hoursPassedSinceLastUpdate = timePassedSinceLastUpdate / (1000  *  60 ); //* 60
            const creditsToAdd = Math.min(
                Math.floor(hoursPassedSinceLastUpdate/3  ),// 
                5 - userData.creditBalance
            );
            
            if (creditsToAdd > 0) {
                userData.creditBalance += creditsToAdd;
                userData.lastCreditUpdate = new Date();
                
                // If reached max credits, deactivate timer
                if (userData.creditBalance >= 5) {
                    userData.timerActive = false;
                    userData.nextCreditAt = null;
                } else {
                    // Set next credit time to 3 hours from now
                    userData.nextCreditAt = new Date(new Date().getTime() + 3 * 60 * 1000); // 60 * 
                }
                
                await userData.save();
            }
        }
        
        // Calculate time remaining if timer active
        let timeRemaining = null;
        if (userData.timerActive && userData.nextCreditAt) {
            timeRemaining = Math.max(0, new Date(userData.nextCreditAt) - new Date());
        }

        res.json({
            success: true, 
            credits: userData.creditBalance,
            timerActive: userData.timerActive,
            nextCreditAt: userData.nextCreditAt,
            timeRemaining
        });

    } catch (error) {
        console.log(error.message)
        res.json({ success: false, message: error.message })
    }
}

// Function to check and manage credit timer
const checkCreditTimer = async (req, res) => {
    try {
        const { clerkId } = req.body;
        const user = await userModel.findOne({ clerkId });
        
        if (!user) {
            return res.json({ success: false, message: "User not found" });
        }
        
        // If user already has max credits, ensure timer is off
        if (user.creditBalance >= 5) {
            if (user.timerActive) {
                user.timerActive = false;
                user.nextCreditAt = null;
                await user.save();
            }
            
            return res.json({ 
                success: true, 
                message: "Max credits reached, timer disabled",
                timerActive: false,
                creditBalance: user.creditBalance
            });
        }
        
        // If timer should be active but isn't
        if (user.creditBalance < 5 && !user.timerActive) {
            // Set next credit time to 3 hours from now
            user.nextCreditAt = new Date(new Date().getTime() +  3 * 60 * 1000); // 60 *
            user.timerActive = true;
            user.lastCreditUpdate = new Date();
            await user.save();
            
            return res.json({ 
                success: true, 
                message: "Timer activated", 
                timerActive: true,
                nextCreditAt: user.nextCreditAt,
                creditBalance: user.creditBalance
            });
        }
        
        // Timer is already running correctly
        return res.json({ 
            success: true, 
            message: "Timer status checked", 
            timerActive: user.timerActive,
            nextCreditAt: user.nextCreditAt,
            creditBalance: user.creditBalance
        });
        
    } catch (error) {
        console.log(error.message);
        res.json({ success: false, message: error.message });
    }
}


export { clerkWebhooks, userCredits, checkCreditTimer }