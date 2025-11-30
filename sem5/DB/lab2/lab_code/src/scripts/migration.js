require('dotenv').config();
const { pool } = require('../config/dbSQL.js');
const connectMongo = require('../config/mongoDb.js');
const PropertyNoSQL = require('../model/PropertyNoSQL.js');

const migrate = async () => {
    await connectMongo();

    console.log('🚀 Починаємо міграцію даних в хмару...');

    try {
        //Getting data from PostgreSql
        const sqlQuery = `
            SELECT 
                p.property_id, p.address, p.property_area, p.sale_price,
                pt.property_name as type_name,
                o.first_name as owner_name, o.last_name as owner_surname, o.email, o.phone,
                pa.attr_name, pa.attr_value
            FROM property p
            JOIN property_type pt ON p.property_type = pt.property_type_id
            JOIN owner o ON p.owner_id = o.owner_id
            LEFT JOIN property_attributes pa ON p.property_id = pa.property_id
            WHERE p.is_deleted = FALSE
            ORDER BY p.property_id;
        `;

        const { rows } = await pool.query(sqlQuery);
        console.log(`Downloaded ${rows.length} notes from PostgreSQL.`);

        const propertiesMap = new Map();

        rows.forEach(row => {
            if (!propertiesMap.has(row.property_id)) {
                propertiesMap.set(row.property_id, {
                    sql_id: row.property_id,
                    address: row.address,
                    type: row.type_name,
                    area: parseFloat(row.property_area),
                    price: parseFloat(row.sale_price),
                    owner: {
                        first_name: row.owner_name,
                        last_name: row.owner_surname,
                        email: row.email,
                        phone: row.phone
                    },
                    attributes: {}
                });
            }

            if (row.attr_name) {
                propertiesMap.get(row.property_id).attributes[row.attr_name] = row.attr_value;
            }
        });

        //Write data to mongo
        const documents = Array.from(propertiesMap.values());

        //clean it before writing down to avoid duplicates 
        await PropertyNoSQL.deleteMany({});

        await PropertyNoSQL.insertMany(documents);

        console.log(`Migrated successfully`);

    } catch (error) {
        console.error('Migration error:', error);
    }
};

module.exports = migrate;