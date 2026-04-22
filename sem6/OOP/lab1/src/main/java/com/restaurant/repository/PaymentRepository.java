package com.restaurant.repository;

import com.restaurant.model.entity.Payment;
import com.restaurant.util.DatabaseConnection;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.Optional;

/**
 * Handles all operations for payments table
 */
public class PaymentRepository {

    private static final Logger logger = LogManager.getLogger(PaymentRepository.class);

    private final Connection connection;

    public PaymentRepository() {
        this.connection = DatabaseConnection.getInstance().getConnection();
    }

    /**
     * Finds a payment by bill ID
     */
    public Optional<Payment> findByBillId(Long billId) {
        String sql = "SELECT * FROM payments WHERE bill_id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, billId);
            ResultSet rs = stmt.executeQuery();

            if (rs.next()) {
                return Optional.of(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch payment for bill: {}", billId, e);
        }

        return Optional.empty();
    }

    /**
     * Saves a new payment
     */
    public Payment save(Payment payment) {
        String sql = "INSERT INTO payments (bill_id, amount) " +
                "VALUES (?, ?) RETURNING id, paid_at";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, payment.getBillId());
            stmt.setBigDecimal(2, payment.getAmount());

            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                payment.setId(rs.getLong("id"));
                payment.setPaidAt(rs.getTimestamp("paid_at").toLocalDateTime());
            }

        } catch (SQLException e) {
            logger.error("Failed to save payment", e);
        }

        return payment;
    }

    /**
     * Maps a single database row to a Payment object
     */
    private Payment mapRow(ResultSet rs) throws SQLException {
        return Payment.builder()
                .id(rs.getLong("id"))
                .billId(rs.getLong("bill_id"))
                .amount(rs.getBigDecimal("amount"))
                .paidAt(rs.getTimestamp("paid_at").toLocalDateTime())
                .build();
    }
}