package com.restaurant.repository;

import com.restaurant.model.entity.User;
import com.restaurant.model.entity.UserRole;
import com.restaurant.util.DatabaseConnection;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

/**
 * Handles operations for users table
 */
public class UserRepository {

    private static final Logger logger = LogManager.getLogger(UserRepository.class);

    private final Connection connection;

    public UserRepository() {
        this.connection = DatabaseConnection.getInstance().getConnection();
    }

    /**
     * Finds a user by Auth0 ID.
     */
    public Optional<User> findByAuth0Id(String auth0Id) {
        String sql = "SELECT * FROM users WHERE auth0_id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, auth0Id);
            ResultSet rs = stmt.executeQuery();

            if (rs.next()) {
                return Optional.of(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch user by auth0Id: {}", auth0Id, e);
        }

        return Optional.empty();
    }

    /**
     * Finds a user by ID
     */
    public Optional<User> findById(Long id) {
        String sql = "SELECT * FROM users WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setLong(1, id);
            ResultSet rs = stmt.executeQuery();

            if (rs.next()) {
                return Optional.of(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch user by id: {}", id, e);
        }

        return Optional.empty();
    }

    /**
     * Returns all users
     */
    public List<User> findAll() {
        List<User> users = new ArrayList<>();
        String sql = "SELECT * FROM users";

        try (Statement stmt = connection.createStatement();
                ResultSet rs = stmt.executeQuery(sql)) {

            while (rs.next()) {
                users.add(mapRow(rs));
            }

        } catch (SQLException e) {
            logger.error("Failed to fetch all users", e);
        }

        return users;
    }

    /**
     * Saves a new user
     */
    public User save(User user) {
        String sql = "INSERT INTO users (auth0_id, name, email, role) " +
                "VALUES (?, ?, ?, ?::user_role) RETURNING id, created_at";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, user.getAuth0Id());
            stmt.setString(2, user.getName());
            stmt.setString(3, user.getEmail());
            stmt.setString(4, user.getRole().name());

            ResultSet rs = stmt.executeQuery();
            if (rs.next()) {
                user.setId(rs.getLong("id"));
                user.setCreatedAt(rs.getTimestamp("created_at").toLocalDateTime());
            }

        } catch (SQLException e) {
            logger.error("Failed to save user", e);
        }

        return user;
    }

    /**
     * Updates user role
     */
    public void updateRole(Long id, UserRole role) {
        String sql = "UPDATE users SET role = ?::user_role WHERE id = ?";

        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, role.name());
            stmt.setLong(2, id);
            stmt.executeUpdate();

        } catch (SQLException e) {
            logger.error("Failed to update role for user: {}", id, e);
        }
    }

    /**
     * Maps a single database row to a User object.
     */
    private User mapRow(ResultSet rs) throws SQLException {
        return User.builder()
                .id(rs.getLong("id"))
                .auth0Id(rs.getString("auth0_id"))
                .name(rs.getString("name"))
                .email(rs.getString("email"))
                .role(UserRole.valueOf(rs.getString("role")))
                .createdAt(rs.getTimestamp("created_at").toLocalDateTime())
                .build();
    }
}