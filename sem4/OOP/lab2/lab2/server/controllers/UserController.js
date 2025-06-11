import { Webhook } from "svix"
// import of service functions
import {
    createUser,
    updateUser,
    deleteUser,
    findUserByClerkId,
    checkAndUpdateCredits,
    manageCreditTimer,
    MAX_CREDITS,
    CREDIT_REFRESH_TIME,
} from "../services/userService.js";
import config from "../configs/appConfig.js";


// API conroller-function for Clerk webhooks
// http://localhost4000/api/user/webhooks
const clerkWebhooks = async (req, res) => {
    try {
        console.log("Webhook received:", req.body.type);
        console.log("Webhook data:", JSON.stringify(req.body.data));
        // Verify the webhook signature
        const whook = new Webhook(config.apiKeys.clerk.webhookSecret)

        await whook.verify(JSON.stringify(req.body), {
            "svix-id": req.headers["svix-id"],
            "svix-timestamp": req.headers["svix-timestamp"],
            "svix-signature": req.headers["svix-signature"]
        })
        const { data, type } = req.body

        switch (type) {
            case "user.created": {
                const userData = {
                    clerkId: data.id,
                    email: data.email_addresses[0].email_address,
                    firstName: data.first_name,
                    lastName: data.last_name,
                    photo: data.profile_image_url || data.image_url

                }
                console.log("Creating user with data:", userData);
                await createUser(userData)

                break;
            }
            case "user.updated": {
                const userData = {
                    email: data.email_addresses[0].email_address,
                    firstName: data.first_name,
                    lastName: data.last_name,
                    photo: data.profile_image_url || data.image_url
                }
                await updateUser(data.id, userData);
                res.json({})
                break;
            }
            case "user.deleted": {
                await deleteUser(data.id);
                res.json({})
                break;
            }

        }
        res.json({});

    } catch (error) {
        console.log(error.message)
        res.json({ success: false, message: error.message })
    }
}

//Api conrtroller-function for getting user credits
const userCredits = async (req, res) => {
    try {
        const { clerkId } = req.body
        const user = await findUserByClerkId(clerkId)

        if (!user) {
            return res.json({ success: false, message: "User not found" });
        }
        const creditInfo = await checkAndUpdateCredits(user)

        res.json({
            success: true,
            ...creditInfo
        });

    } catch (error) {
        console.log(error.message)
        res.json({ success: false, message: error.message })
    }
}
//Function to manage the credit timer
const checkCreditTimer = async (req, res) => {
    try {
        const { clerkId } = req.body;
        const user = await findUserByClerkId(clerkId);

        if (!user) {
            return res.json({ success: false, message: "User not found" });
        }

        const result = await manageCreditTimer(user);
        res.json(result);

    } catch (error) {
        console.log(error.message);
        res.json({ success: false, message: error.message });
    }
}


export { clerkWebhooks, userCredits, checkCreditTimer }