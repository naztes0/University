package com.restaurant.servlet;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.restaurant.model.entity.User;
import com.restaurant.model.entity.UserRole;
import com.restaurant.repository.UserRepository;
import com.restaurant.util.JwtUtil;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.mindrot.jbcrypt.BCrypt;

import java.io.IOException;
import java.util.Optional;

/**
 * Handles authentication endpoints
 */
@WebServlet("/auth/*")
public class AuthServlet extends HttpServlet {

    private static final Logger logger = LogManager.getLogger(AuthServlet.class);

    private UserRepository userRepository;
    private ObjectMapper objectMapper;

    @Override
    public void init() {
        this.userRepository = new UserRepository();
        this.objectMapper = new ObjectMapper();
    }

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        String pathInfo = req.getPathInfo();

        if ("/register".equals(pathInfo)) {
            handleRegister(req, resp);
        } else if ("/login".equals(pathInfo)) {
            handleLogin(req, resp);
        } else {
            sendError(resp, HttpServletResponse.SC_NOT_FOUND, "Endpoint not found");
        }
    }

    private void handleRegister(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        AuthRequest authRequest = objectMapper.readValue(req.getInputStream(), AuthRequest.class);

        // Check if user already exists
        Optional<User> existing = userRepository.findByEmail(authRequest.email());
        if (existing.isPresent()) {
            sendError(resp, HttpServletResponse.SC_CONFLICT, "Email already registered");
            return;
        }

        // Hash password with BCrypt
        String passwordHash = BCrypt.hashpw(authRequest.password(), BCrypt.gensalt());

        User newUser = User.builder()
                .name(authRequest.name())
                .email(authRequest.email())
                .passwordHash(passwordHash)
                .role(UserRole.CLIENT)
                .build();

        User saved = userRepository.saveWithPassword(newUser);

        String token = JwtUtil.generateToken(
                saved.getId(),
                saved.getEmail(),
                saved.getRole().name());

        logger.info("New user registered: {}", saved.getEmail());
        sendJson(resp, HttpServletResponse.SC_CREATED, new TokenResponse(token, saved.getRole().name()));
    }

    /**
     * Logs in an existing user
     */
    private void handleLogin(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        AuthRequest authRequest = objectMapper.readValue(req.getInputStream(), AuthRequest.class);

        Optional<User> userOptional = userRepository.findByEmail(authRequest.email());

        if (userOptional.isEmpty()) {
            sendError(resp, HttpServletResponse.SC_UNAUTHORIZED, "Invalid email or password");
            return;
        }

        User user = userOptional.get();

        // Check password
        if (!BCrypt.checkpw(authRequest.password(), user.getPasswordHash())) {
            sendError(resp, HttpServletResponse.SC_UNAUTHORIZED, "Invalid email or password");
            return;
        }

        String token = JwtUtil.generateToken(
                user.getId(),
                user.getEmail(),
                user.getRole().name());

        logger.info("User logged in: {}", user.getEmail());
        sendJson(resp, HttpServletResponse.SC_OK, new TokenResponse(token, user.getRole().name()));
    }

    private void sendJson(HttpServletResponse resp, int status, Object data) throws IOException {
        resp.setStatus(status);
        resp.setContentType("application/json");
        resp.setCharacterEncoding("UTF-8");
        resp.getWriter().write(objectMapper.writeValueAsString(data));
    }

    private void sendError(HttpServletResponse resp, int status, String message) throws IOException {
        sendJson(resp, status, new ErrorResponse(message));
    }

    private record AuthRequest(String name, String email, String password) {
    }

    private record TokenResponse(String token, String role) {
    }

    private record ErrorResponse(String error) {
    }
}