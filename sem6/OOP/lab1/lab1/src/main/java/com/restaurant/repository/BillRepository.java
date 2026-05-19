package com.restaurant.repository;

import com.restaurant.model.entity.Bill;
import com.restaurant.model.entity.BillStatus;
import com.restaurant.util.DatabaseConnection;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.Optional;

/**
 * Handles all operations for bills table
 */
public class BillRepository {

    private static final Logger logger = LogManager.getLogger(BillRepository.class);

    private final Connection connection;

    public BillRepository() {
        this.connection = DatabaseConnection.getInstance().getConnection();
    }

    /**
     * Finds a bill by order ID
     */
    public Optional<Bill> findByOrderId(Long orderId) {
        String sql = "SELECT * FROM bills WHERE order_id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, orderId);
            ResultSet rs = stmt.executeQuery();

            if (rs.next()) {
                return Optional.of(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch bill for order: {}", orderId, e);
        }

        return Optional.empty();
    }

    /**
     * Finds a bill by its ID
     */
    public Optional<Bill> findById(Long id) {
        String sql = "SELECT * FROM bills WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, id);
            ResultSet rs = stmt.executeQuery();

            if (rs.next()) {
                return Optional.of(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch bill by id: {}", id, e);
        }

        return Optional.empty();
    }

    /**
     * Saves a new bill
     */
    public Bill save(Bill bill) {
        String sql = "INSERT INTO bills (order_id, total_amount, status) " +
                "VALUES (?, ?, ?::bill_status) RETURNING id, created_at";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, bill.getOrderId());
            stmt.setBigDecimal(2, bill.getTotalAmount());
            stmt.setString(3, BillStatus.UNPAID.name());

            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                bill.setId(rs.getLong("id"));
                bill.setCreatedAt(rs.getTimestamp("created_at").toLocalDateTime());
                bill.setStatus(BillStatus.UNPAID);
            }

        } catch (SQLException e) {
            logger.error("Failed to save bill", e);
        }

        return bill;
    }

    /**
     * Updates bill status to PAID
     */
    public void updateStatus(Long id, BillStatus status) {
        String sql = "UPDATE bills SET status = ?::bill_status WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, status.name());
            stmt.setLong(2, id);
            stmt.executeUpdate();

            logger.info("Bill {} status updated to {}", id, status);

        } catch (SQLException e) {
            logger.error("Failed to update status for bill: {}", id, e);
        }
    }

    /**
     * Maps a single database row to a Bill object
     */
    private Bill mapRow(ResultSet rs) throws SQLException {
        return Bill.builder()
                .id(rs.getLong("id"))
                .orderId(rs.getLong("order_id"))
                .totalAmount(rs.getBigDecimal("total_amount"))
                .status(BillStatus.valueOf(rs.getString("status")))
                .createdAt(rs.getTimestamp("created_at").toLocalDateTime())
                .build();
    }
}