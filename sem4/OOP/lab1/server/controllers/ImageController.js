import axios from "axios";
import fs from "fs";
import FormData from "form-data";
import userModel from "../models/userModel.js";
import { buffer } from "stream/consumers";


//Controller func to remove bg from image 
const removeBgImage=async(req,res)=>{
    try {
        
        const {clerkId}=req.body
        const user = await userModel.findOne({clerkId})

        if(!user){
            return res.json({success:false, message:"User not found"})
        }
        if(user.creditBalance===0){
            return res.json({success:false, message:"No credit balance", creditBalance:user.creditBalance})
        }
        const imagePath= req.file.path;
        //Reading the image file 
        const imageFile= fs.createReadStream(imagePath)
        const formdata = new FormData()
        formdata.append('image_file',imageFile)

        const {data}=await axios.post('https://clipdrop-api.co/remove-background/v1',formdata, {
            headers:{
                'x-api-key':process.env.CLIPDROP_API,
            },
            responseType:"arraybuffer"
        })


        const base64Image=Buffer.from(data,"binary").toString("base64")

        const resultImage = `data: ${req.file.mimetype};base64,${base64Image}`

        // //To minus one credit for removing img bg

        // await userModel.findByIdAndUpdate(user._id,{creditBalance:user.creditBalance-1})

     
        // res.json({success:true, resultImage, creditBalance:user.creditBalance-1, message:"Background Removed"})

        // To minus one credit for removing img bg
        const newCreditBalance = user.creditBalance - 1;
        
        // Check if we need to activate the timer
        let timerActive = user.timerActive;
        let nextCreditAt = user.nextCreditAt;
        
        // If credits are below max and timer isn't active, activate it
        if (newCreditBalance < 5 && !timerActive) {
            timerActive = true;
            nextCreditAt = new Date(new Date().getTime() +  3*60 * 1000); //3 * 60 *
        }
        
        // Update user with new credit balance and timer status
        await userModel.findByIdAndUpdate(user._id, {
            creditBalance: newCreditBalance,
            timerActive,
            nextCreditAt,
            lastCreditUpdate: new Date()
        });

        res.json({
            success: true,
            resultImage,
            creditBalance: newCreditBalance,
            timerActive,
            nextCreditAt,
            message: "Background Removed"
        })

    } catch (error) {
        console.log(error.message)
        res.json({success:false,message:error.message})
    }
}

export{removeBgImage}