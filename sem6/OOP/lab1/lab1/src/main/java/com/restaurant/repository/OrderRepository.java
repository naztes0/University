package com.restaurant.repository;

import com.restaurant.model.entity.Order;
import com.restaurant.model.entity.OrderStatus;
import com.restaurant.util.DatabaseConnection;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

/**
 * Handles operations for orders table
 */
public class OrderRepository {

    private static final Logger logger = LogManager.getLogger(OrderRepository.class);

    private final Connection connection;

    public OrderRepository() {
        this.connection = DatabaseConnection.getInstance().getConnection();
    }

    /**
     * Returns all orders (used by admin and kitchen)
     */
    public List<Order> findAll() {
        List<Order> orders = new ArrayList<>();
        String sql = "SELECT * FROM orders ORDER BY created_at DESC";

        try (Statement stmt = connection.createStatement();
                ResultSet rs = stmt.executeQuery(sql)) {

            while (rs.next()) {
                orders.add(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch all orders", e);
        }

        return orders;
    }

    /**
     * Returns all orders for a specific user (used by client)
     */
    public List<Order> findByUserId(Long userId) {
        List<Order> orders = new ArrayList<>();
        String sql = "SELECT * FROM orders WHERE user_id = ? ORDER BY created_at DESC";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, userId);
            ResultSet rs = stmt.executeQuery();

            while (rs.next()) {
                orders.add(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch orders for user: {}", userId, e);
        }

        return orders;
    }

    /**
     * Returns all orders with a specific status (used by kitchen)
     */
    public List<Order> findByStatus(OrderStatus status) {
        List<Order> orders = new ArrayList<>();
        String sql = "SELECT * FROM orders WHERE status = ?::order_status ORDER BY created_at DESC";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, status.name());
            ResultSet rs = stmt.executeQuery();

            while (rs.next()) {
                orders.add(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch orders by status: {}", status, e);
        }

        return orders;
    }

    /**
     * Finds a single order by ID
     */
    public Optional<Order> findById(Long id) {
        String sql = "SELECT * FROM orders WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, id);
            ResultSet rs = stmt.executeQuery();

            if (rs.next()) {
                return Optional.of(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch order by id: {}", id, e);
        }

        return Optional.empty();
    }

    /**
     * Saves a new order
     */
    public Order save(Order order) {
        String sql = "INSERT INTO orders (user_id, table_number, status) " +
                "VALUES (?, ?, ?::order_status) RETURNING id, created_at";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, order.getUserId());
            stmt.setInt(2, order.getTableNumber());
            stmt.setString(3, OrderStatus.NEW.name());

            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                order.setId(rs.getLong("id"));
                order.setCreatedAt(rs.getTimestamp("created_at").toLocalDateTime());
                order.setStatus(OrderStatus.NEW);
            }

        } catch (SQLException e) {
            logger.error("Failed to save order", e);
        }

        return order;
    }

    /**
     * Updates the status of an order
     */
    public void updateStatus(Long id, OrderStatus status) {
        String sql = "UPDATE orders SET status = ?::order_status WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, status.name());
            stmt.setLong(2, id);
            stmt.executeUpdate();

            logger.info("Order {} status updated to {}", id, status);

        } catch (SQLException e) {
            logger.error("Failed to update status for order: {}", id, e);
        }
    }

    /**
     * Maps a single database row to an Order object
     */
    private Order mapRow(ResultSet rs) throws SQLException {
        return Order.builder()
                .id(rs.getLong("id"))
                .userId(rs.getLong("user_id"))
                .tableNumber(rs.getInt("table_number"))
                .status(OrderStatus.valueOf(rs.getString("status")))
                .createdAt(rs.getTimestamp("created_at").toLocalDateTime())
                .build();
    }
}