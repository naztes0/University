package com.restaurant.repository;

import com.restaurant.model.entity.OrderItem;
import com.restaurant.util.DatabaseConnection;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

/**
 * Handles operations for order_items table
 */
public class OrderItemRepository {

    private static final Logger logger = LogManager.getLogger(OrderItemRepository.class);

    private final Connection connection;

    public OrderItemRepository() {
        this.connection = DatabaseConnection.getInstance().getConnection();
    }

    /**
     * Returns all items for a specific order
     */
    public List<OrderItem> findByOrderId(Long orderId) {
        List<OrderItem> items = new ArrayList<>();
        String sql = """
                SELECT oi.*, mi.name as menu_item_name
                FROM order_items oi
                JOIN menu_items mi ON oi.menu_item_id = mi.id
                WHERE oi.order_id = ?
                """;

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, orderId);
            ResultSet rs = stmt.executeQuery();

            while (rs.next()) {
                items.add(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch items for order: {}", orderId, e);
        }

        return items;
    }

    /**
     * Saves a single order item
     */
    public OrderItem save(OrderItem item) {
        String sql = "INSERT INTO order_items (order_id, menu_item_id, quantity, price_at_order) " +
                "VALUES (?, ?, ?, ?) RETURNING id";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, item.getOrderId());
            stmt.setLong(2, item.getMenuItemId());
            stmt.setInt(3, item.getQuantity());
            stmt.setBigDecimal(4, item.getPriceAtOrder());

            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                item.setId(rs.getLong("id"));
            }

        } catch (SQLException e) {
            logger.error("Failed to save order item", e);
        }

        return item;
    }

    /**
     * Saves multiple order items at once (used when creating an order)
     */
    public void saveAll(List<OrderItem> items) {
        String sql = "INSERT INTO order_items (order_id, menu_item_id, quantity, price_at_order) " +
                "VALUES (?, ?, ?, ?)";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            for (OrderItem item : items) {
                stmt.setLong(1, item.getOrderId());
                stmt.setLong(2, item.getMenuItemId());
                stmt.setInt(3, item.getQuantity());
                stmt.setBigDecimal(4, item.getPriceAtOrder());
                stmt.addBatch();
            }

            stmt.executeBatch();

        } catch (SQLException e) {
            logger.error("Failed to save order items batch", e);
        }
    }

    /**
     * Deletes all items for a specific order
     */
    public void deleteByOrderId(Long orderId) {
        String sql = "DELETE FROM order_items WHERE order_id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, orderId);
            stmt.executeUpdate();

        } catch (SQLException e) {
            logger.error("Failed to delete items for order: {}", orderId, e);
        }
    }

    /**
     * Maps a single database row to an OrderItem object
     */
    private OrderItem mapRow(ResultSet rs) throws SQLException {
        return OrderItem.builder()
                .id(rs.getLong("id"))
                .orderId(rs.getLong("order_id"))
                .menuItemId(rs.getLong("menu_item_id"))
                .quantity(rs.getInt("quantity"))
                .priceAtOrder(rs.getBigDecimal("price_at_order"))
                .menuItemName(rs.getString("menu_item_name"))
                .build();
    }
}