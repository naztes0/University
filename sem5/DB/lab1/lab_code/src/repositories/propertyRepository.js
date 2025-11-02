const BaseRepository = require('./baseRepository');

class PropertyRepository extends BaseRepository {
    // Отримання всіх активних об'єктів (Read)
    async getAllActive() {
        // Використовуємо VIEW для отримання даних
        const sql = 'SELECT * FROM active_properties_view ORDER BY property_id';
        const result = await this.query(sql);
        return result.rows;
    }

    // М'яке видалення об'єкта (Write)
    async softDelete(propertyId, updatedByUserId) {
        // Використовуємо ЗБЕРЕЖЕНУ ПРОЦЕДУРУ для модифікації
        const sql = 'CALL soft_delete_property($1, $2)';
        await this.query(sql, [propertyId, updatedByUserId]);
        return { property_id: propertyId, is_deleted: true };
    }
}

module.exports = PropertyRepository;