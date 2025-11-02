const { pool } = require('../db');

class BaseRepository {
    constructor(clientOrPool) {
        this.db = clientOrPool || pool;
    }

    async query(sql, params = []) {
        return this.db.query(sql, params);
    }
}

module.exports = BaseRepository;