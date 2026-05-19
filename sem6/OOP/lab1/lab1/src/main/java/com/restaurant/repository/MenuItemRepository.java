package com.restaurant.repository;

import com.restaurant.model.entity.MenuItem;
import com.restaurant.util.DatabaseConnection;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

/**
 * Handles all database operations for menu_items table.
 */
public class MenuItemRepository {

    private static final Logger logger = LogManager.getLogger(MenuItemRepository.class);

    // Gets the connection from our Singleton
    private final Connection connection;

    public MenuItemRepository() {
        this.connection = DatabaseConnection.getInstance().getConnection();
    }

    /**
     * Fetches all menu items from the database.
     */
    public List<MenuItem> findAll() {
        List<MenuItem> items = new ArrayList<>();
        String sql = "SELECT * FROM menu_items";

        try (Statement stmt = connection.createStatement();
                ResultSet rs = stmt.executeQuery(sql)) {

            while (rs.next()) {
                items.add(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch menu items", e);
        }

        return items;
    }

    /**
     * Finds a single menu item by its ID.
     */
    public Optional<MenuItem> findById(Long id) {
        String sql = "SELECT * FROM menu_items WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, id);
            ResultSet rs = stmt.executeQuery();

            if (rs.next()) {
                return Optional.of(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch menu item by id: {}", id, e);
        }

        return Optional.empty();
    }

    /**
     * Inserts a new menu item into the database.
     */
    public MenuItem save(MenuItem item) {
        String sql = "INSERT INTO menu_items (name, description, price, category, available) " +
                "VALUES (?, ?, ?, ?, ?) RETURNING id";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, item.getName());
            stmt.setString(2, item.getDescription());
            stmt.setBigDecimal(3, item.getPrice());
            stmt.setString(4, item.getCategory());
            stmt.setBoolean(5, item.isAvailable());

            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                item.setId(rs.getLong("id"));
            }

        } catch (SQLException e) {
            logger.error("Failed to save menu item", e);
        }

        return item;
    }

    /**
     * Updates an existing menu item.
     */
    public void update(MenuItem item) {
        String sql = "UPDATE menu_items SET name = ?, description = ?, price = ?, " +
                "category = ?, available = ? WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, item.getName());
            stmt.setString(2, item.getDescription());
            stmt.setBigDecimal(3, item.getPrice());
            stmt.setString(4, item.getCategory());
            stmt.setBoolean(5, item.isAvailable());
            stmt.setLong(6, item.getId());

            stmt.executeUpdate();

        } catch (SQLException e) {
            logger.error("Failed to update menu item: {}", item.getId(), e);
        }
    }

    /**
     * Deletes a menu item by ID.
     */
    public void delete(Long id) {
        String sql = "DELETE FROM menu_items WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, id);
            stmt.executeUpdate();

        } catch (SQLException e) {
            logger.error("Failed to delete menu item: {}", id, e);
        }
    }

    /**
     * Maps a single database row to a MenuItem object.
     */
    private MenuItem mapRow(ResultSet rs) throws SQLException {
        return MenuItem.builder()
                .id(rs.getLong("id"))
                .name(rs.getString("name"))
                .description(rs.getString("description"))
                .price(rs.getBigDecimal("price"))
                .category(rs.getString("category"))
                .available(rs.getBoolean("available"))
                .build();
    }
}