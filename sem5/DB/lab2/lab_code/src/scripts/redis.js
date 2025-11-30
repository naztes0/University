const { pool } = require('../config/dbSQL');
const connectRedis = require('../config/redisDb');

const runRedisDemo = async () => {
    const redisClient = await connectRedis();

    console.log('\n--- REDIS ---');

    //key by which data is saved
    const cacheKey = 'complex_query_result';

    const getData = async () => {
        const start = performance.now();

        // Getting from cahce
        const cachedData = await redisClient.get(cacheKey);

        if (cachedData) {
            const end = performance.now();
            console.log(`Data get from Redis`);
            console.log(`Execution time: ${(end - start).toFixed(4)} ms`);
            return JSON.parse(cachedData);
        }

        // if not - getting from sql
        console.log(`Getting data from SQL`);

        const sqlQuery = `
            SELECT p.property_id, p.address, p.sale_price
            FROM property p
            JOIN property_attributes pa1 ON p.property_id = pa1.property_id 
            JOIN property_attributes pa2 ON p.property_id = pa2.property_id
            JOIN property_attributes pa3 ON p.property_id = pa3.property_id
            WHERE 
                p.sale_price < 200000
                AND (pa1.attr_name = 'Garage' AND pa1.attr_value LIKE 'Yes%')
                AND (pa2.attr_name = 'Internet' AND pa2.attr_value = 'Starlink')
                AND (pa3.attr_name = 'Heating' AND pa3.attr_value = 'Gas Individual');
        `;

        const dbRes = await pool.query(sqlQuery);
        const data = dbRes.rows;

        //Save result to redis
        await redisClient.setEx(cacheKey, 60, JSON.stringify(data));

        const end = performance.now();
        console.log(`Got data from SQL and written to Redis`);
        console.log(`Execution time: ${(end - start).toFixed(4)} ms`);

        return data;
    };


    // first time getting from sql
    console.log('\n--- First request ---');
    await getData();

    // getting from redis
    console.log('\n--- Second request ---');
    await getData();

    await redisClient.del(cacheKey);

    await redisClient.disconnect();
};

module.exports = runRedisDemo;