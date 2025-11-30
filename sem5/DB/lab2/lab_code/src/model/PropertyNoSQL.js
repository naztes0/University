const mongoose = require('mongoose');

const PropertySchema = new mongoose.Schema({
    sql_id: { type: Number, required: true, unique: true },
    address: { type: String, required: true },
    type: { type: String },
    area: { type: Number },
    price: { type: Number },


    owner: {
        first_name: String,
        last_name: String,
        email: String,
        phone: String
    },

    attributes: {
        type: Map,
        of: String
    },

    migrated_at: { type: Date, default: Date.now }
});

PropertySchema.index({ "attributes.Pool": 1 });
PropertySchema.index({ price: 1 });

module.exports = mongoose.model('Property', PropertySchema);