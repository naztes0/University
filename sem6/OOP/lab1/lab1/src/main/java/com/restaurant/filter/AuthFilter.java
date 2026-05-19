package com.restaurant.filter;

import com.auth0.jwt.exceptions.JWTVerificationException;
import com.auth0.jwt.interfaces.DecodedJWT;
import com.restaurant.model.entity.User;
import com.restaurant.service.UserService;
import com.restaurant.util.JwtUtil;
import jakarta.servlet.Filter;
import jakarta.servlet.FilterChain;
import jakarta.servlet.FilterConfig;
import jakarta.servlet.ServletException;
import jakarta.servlet.ServletRequest;
import jakarta.servlet.ServletResponse;
import jakarta.servlet.annotation.WebFilter;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.Optional;

/**
 * Auth Filter — validates JWT token on every request.
 * Skips validation for /auth/* endpoints (login/register).
 */
@WebFilter("/*")
public class AuthFilter implements Filter {

    private static final Logger logger = LogManager.getLogger(AuthFilter.class);

    private UserService userService;

    @Override
    public void init(FilterConfig filterConfig) {
        this.userService = new UserService();
        logger.info("AuthFilter initialized");
    }

    @Override
    public void doFilter(ServletRequest request, ServletResponse response,
            FilterChain chain) throws IOException, ServletException {

        HttpServletRequest httpRequest = (HttpServletRequest) request;
        HttpServletResponse httpResponse = (HttpServletResponse) response;

        String path = httpRequest.getServletPath();

        // Skip auth check for login and register endpoints
        if (path.startsWith("/auth")) {
            chain.doFilter(request, response);
            return;
        }

        // Extract token from Authorization header
        String authHeader = httpRequest.getHeader("Authorization");

        if (authHeader == null || !authHeader.startsWith("Bearer ")) {
            sendUnauthorized(httpResponse, "Missing or invalid Authorization header");
            return;
        }

        String token = authHeader.substring(7);

        try {
            // Validate token and extract claims
            DecodedJWT decodedJWT = JwtUtil.validateToken(token);

            Long userId = Long.parseLong(decodedJWT.getSubject());
            String role = decodedJWT.getClaim("role").asString();
            String email = decodedJWT.getClaim("email").asString();

            // Find user in DB
            Optional<User> userOptional = userService.getById(userId);
            if (userOptional.isEmpty()) {
                sendUnauthorized(httpResponse, "User not found");
                return;
            }

            // Attach user to request for Servlets to use
            httpRequest.setAttribute("currentUser", userOptional.get());
            logger.debug("Authenticated user: {}", email);

            chain.doFilter(request, response);

        } catch (JWTVerificationException e) {
            logger.warn("JWT verification failed: {}", e.getMessage());
            sendUnauthorized(httpResponse, "Invalid or expired token");
        }
    }

    private void sendUnauthorized(HttpServletResponse response, String message) throws IOException {
        response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
        response.setContentType("application/json");
        response.setCharacterEncoding("UTF-8");
        response.getWriter().write("{\"error\": \"" + message + "\"}");
    }

    @Override
    public void destroy() {
        logger.info("AuthFilter destroyed");
    }
}