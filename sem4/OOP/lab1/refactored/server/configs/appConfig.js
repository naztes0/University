
//Application configuration file

const config = {
    //credit system config
    credits: {
        max: 5,
        refreshTimer: 2,
    },
    //server configs
    server: {
        port: process.env.PORT || 4000,
    },
    // MongoDB connection
    db: {
        uri: process.env.MONGODB_URI || 'mongodb://localhost:27017',
        name: 'bg-removal',
        options: {
            serverSelectionTimeoutMS: 5000,
            connectTimeoutMS: 5000,
        },
    },
    //API keys
    apiKeys: {
        clipdrop: process.env.CLIPDROP_API,
        clerk: {
            webhookSecret: process.env.CLERK_WEBHOOK_SECRET,
        },
    }

}

export default config;
