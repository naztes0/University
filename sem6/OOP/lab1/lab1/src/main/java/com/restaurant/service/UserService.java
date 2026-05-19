package com.restaurant.service;

import com.restaurant.mapper.UserMapper;
import com.restaurant.model.dto.UserDTO;
import com.restaurant.model.entity.User;
import com.restaurant.model.entity.UserRole;
import com.restaurant.repository.UserRepository;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

public class UserService {

    private static final Logger logger = LogManager.getLogger(UserService.class);

    private final UserRepository userRepository;
    private final UserMapper userMapper;

    public UserService() {
        this.userRepository = new UserRepository();
        this.userMapper = UserMapper.INSTANCE;
    }

    /**
     * Finds or creates a user after Auth0 login
     */
    public User findOrCreate(String auth0Id, String name, String email) {
        Optional<User> existing = userRepository.findByAuth0Id(auth0Id);

        if (existing.isPresent()) {
            logger.info("Existing user logged in: {}", email);
            return existing.get();
        }

        // First time login — create new user with CLIENT role
        User newUser = User.builder()
                .auth0Id(auth0Id)
                .name(name)
                .email(email)
                .role(UserRole.CLIENT)
                .build();

        User saved = userRepository.save(newUser);
        logger.info("New user registered: {}", email);
        return saved;
    }

    /**
     * Returns all users (admin only)
     */
    public List<UserDTO> getAll() {
        return userRepository.findAll()
                .stream()
                .map(userMapper::toDTO)
                .collect(Collectors.toList());
    }

    /**
     * Returns a user by their Auth0 ID
     */
    public Optional<User> getByAuth0Id(String auth0Id) {
        return userRepository.findByAuth0Id(auth0Id);
    }

    public Optional<User> getById(Long id) {
        return userRepository.findById(id);
    }

    /**
     * Updates a user's role (admin only)
     */
    public void updateRole(Long id, String role) {
        UserRole userRole = UserRole.valueOf(role.toUpperCase());
        userRepository.updateRole(id, userRole);
        logger.info("User {} role updated to {}", id, userRole);
    }
}