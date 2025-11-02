const BaseRepository = require('./baseRepository');

class SaleAgreementRepository extends BaseRepository {
    async getAllDetails() {
        //  Using VIEW to get data
        const sql = 'SELECT * FROM sale_agreement_details_view ORDER BY sale_id';
        const result = await this.query(sql);
        return result.rows;
    }

    // Change to 'Closed' -> trigger_soft_delete_property
    async updateStatus(saleId, newStatusId, updatedByUserId) {
        const sql = `
            UPDATE public.sale_agreement
            SET status_id = $1, updated_by = $2
            WHERE sale_id = $3
            RETURNING *;
        `;
        const result = await this.query(sql, [newStatusId, updatedByUserId, saleId]);
        return result.rows[0];
    }
}

module.exports = SaleAgreementRepository;