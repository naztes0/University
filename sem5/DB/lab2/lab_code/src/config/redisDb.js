const { createClient } = require('redis');
require('dotenv').config();

const connectRedis = async () => {
    const client = createClient({
        url: process.env.REDIS_URL
    });

    client.on('error', (err) => console.log('Redis Client Error', err));

    await client.connect();
    console.log('Redis Connected');

    return client;
};

module.exports = connectRedis;