const { pool } = require('../config/dbSQL.js');
const connectMongo = require('../config/mongoDb.js');
const PropertyNoSQL = require('../model/PropertyNoSQL.js');

const runBenchmark = async () => {
    if (require('mongoose').connection.readyState === 0) {
        await connectMongo();
    }

    console.log('\n--- BENCHMARK ---');
    console.log('Task: find all property objects with pool, and show their owners\n');

    const iterations = 100;

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

    const startSql = performance.now();

    for (let i = 0; i < iterations; i++) {
        await pool.query(sqlQuery);
    }

    const endSql = performance.now();
    const avgSql = (endSql - startSql) / iterations;
    console.log(`SQL Avarage time: ${avgSql.toFixed(4)} ms`);

    const mongoQuery = {
        price: { $lt: 200000 },
        "attributes.Garage": /^Yes/,
        "attributes.Internet": "Starlink",
        "attributes.Heating": "Gas Individual"
    };

    const startMongo = performance.now();

    for (let i = 0; i < iterations; i++) {
        await PropertyNoSQL.find(mongoQuery).lean();
    }

    const endMongo = performance.now();
    const avgMongo = (endMongo - startMongo) / iterations;
    console.log(`NoSQL Avarage time: ${avgMongo.toFixed(4)} ms`);

    console.log('\n--- RESULTS ---');
    if (avgMongo < avgSql) {
        const factor = (avgSql / avgMongo).toFixed(1);
        console.log(`MongoDB ${factor} times faster!`);
    } else {
        console.log('SQL faster');
    }

};

module.exports = runBenchmark;