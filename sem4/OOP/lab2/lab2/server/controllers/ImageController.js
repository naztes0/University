import { findUserByClerkId } from "../services/userService.js"
import { removeImageBackground } from "../services/imageService.js"


// Controller func to remove bg 
const removeBgImage = async (req, res) => {
    try {
        const { clerkId } = req.body
        const user = await findUserByClerkId(clerkId);

        if (!user) {
            return res.json({ success: false, message: "User not found" })
        }
        if (user.creditBalance === 0) {
            return res.json({
                success: false,
                message: "No credit balance",
                creditBalance: user.creditBalance
            })
        }
        user.mime = req.file.mimetype;
        const result = await removeImageBackground(user, req.file.path);
        res.json({
            success: true,
            resultImage: result.resultImage,
            creditBalance: result.creditBalance,
            timerActive: result.timerActive,
            nextCreditAt: result.nextCreditAt,
            message: "Background Removed"
        });

    } catch (error) {
        console.log(error.message)
        res.json({ success: false, message: error.message })
    }
}

export { removeBgImage }