// index.js (ОНОВЛЕНИЙ З АНГЛІЙСЬКИМИ ЛОГАМИ)
const UnitOfWork = require('./src/unitOfWork');
const { pool } = require('./src/db');

async function runTest() {
    const uow = new UnitOfWork();

    // --- Test Configuration (Adjust IDs based on your test data) ---
    const TEST_SALE_ID = 1;
    const TEST_PROPERTY_ID = 1;
    const TEST_USER_ID = 3;
    let closedStatusId;

    try {
        // 1. Find the required 'Closed' status ID
        const statusResult = await pool.query("SELECT status_id FROM contract_status WHERE status_name = 'Closed' LIMIT 1");
        if (statusResult.rows.length === 0) {
            throw new Error("Missing 'Closed' status in contract_status table.");
        }
        closedStatusId = statusResult.rows[0].status_id;

        console.log(`--- UoW Transaction Start ---`);
        await uow.begin();

        // 2. Update the Sale Agreement status
        console.log(`[1/2] Updating Sale ID: ${TEST_SALE_ID} to 'Closed' (Status ID: ${closedStatusId}).`);
        const updatedSale = await uow.getSaleAgreementRepository().updateStatus(
            TEST_SALE_ID,
            closedStatusId,
            TEST_USER_ID
        );

        const checkSql = 'SELECT is_deleted, updated_by FROM property WHERE property_id = $1';
        const checkResult = await uow.client.query(checkSql, [TEST_PROPERTY_ID]);

        if (checkResult.rows.length === 0) {
            throw new Error(`Property ID ${TEST_PROPERTY_ID} not found.`);
        }

        const propertyData = checkResult.rows[0];

        console.log(`[2/2] Verifying Property ID: ${TEST_PROPERTY_ID} status.`);
        console.log(`    -> is_deleted: ${propertyData.is_deleted}`);
        console.log(`    -> updated_by: ${propertyData.updated_by}`);


        if (propertyData.is_deleted === true) {
            console.log("    -> Success: Property soft delete verified via Trigger.");
        } else {
            throw new Error("Verification failed: Property.is_deleted is still FALSE.");
        }

        // 4. Commit transaction
        await uow.commit();
        console.log(`\n--- UoW Transaction committed successfully ---`);

    } catch (error) {
        console.error(`\n--- UoW Transaction Rollback ---`);
        console.error(`Error: ${error.message}`);
        await uow.rollback();
    } finally {
        await pool.end();
        console.log("Database connection pool ended.");
    }
}

runTest();