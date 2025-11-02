const { pool } = require('./db');
const PropertyRepository = require('./repositories/propertyRepository');
const SaleAgreementRepository = require('./repositories/saleAgreement');

class UnitOfWork {
    constructor() {
        this.client = null;
    }

    async begin() {
        this.client = await pool.connect();
        await this.client.query('BEGIN');

        this.properties = new PropertyRepository(this.client);
        this.sales = new SaleAgreementRepository(this.client);
    }
    async commit() {
        if (this.client) {
            await this.client.query('COMMIT');
            this.client.release();
            this.client = null;
        }
    }

    async rollback() {
        if (this.client) {
            await this.client.query('ROLLBACK');
            this.client.release();
            this.client = null;
        }
    }

    getPropertyRepository() {
        return this.properties;
    }

    getSaleAgreementRepository() {
        return this.sales;
    }
}

module.exports = UnitOfWork;