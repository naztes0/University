package com.restaurant.servlet;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.restaurant.model.dto.UserDTO;
import com.restaurant.model.entity.User;
import com.restaurant.model.entity.UserRole;
import com.restaurant.service.UserService;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.List;

@WebServlet("/users/*")
public class UserServlet extends HttpServlet {

    private static final Logger logger = LogManager.getLogger(UserServlet.class);

    private UserService userService;
    private ObjectMapper objectMapper;

    @Override
    public void init() {
        this.userService = new UserService();
        this.objectMapper = new ObjectMapper();
        this.objectMapper.registerModule(new JavaTimeModule());
    }

    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        String pathInfo = req.getPathInfo();
        logger.debug("GET /users request, pathInfo: {}", pathInfo);

        User currentUser = (User) req.getAttribute("currentUser");

        if (pathInfo != null && pathInfo.equals("/me")) {
            // GET /api/users/me — return current user profile
            UserDTO dto = userService.getByAuth0Id(currentUser.getAuth0Id())
                    .map(user -> new UserDTO(
                            user.getId(),
                            user.getName(),
                            user.getEmail(),
                            user.getRole().name()))
                    .orElse(null);

            sendJson(resp, HttpServletResponse.SC_OK, dto);
            return;
        }

        // GET /api/users — admin only
        if (!hasRole(req, UserRole.ADMIN)) {
            logger.warn("Forbidden GET /users attempt");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Admin access required");
            return;
        }

        List<UserDTO> users = userService.getAll();
        sendJson(resp, HttpServletResponse.SC_OK, users);
    }

    @Override
    protected void doPut(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        if (!hasRole(req, UserRole.ADMIN)) {
            logger.warn("Forbidden PUT /users attempt");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Admin access required");
            return;
        }

        String pathInfo = req.getPathInfo();
        if (pathInfo == null || !pathInfo.endsWith("/role")) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid endpoint");
            return;
        }

        try {
            Long userId = Long.parseLong(pathInfo.split("/")[1]);
            RoleUpdateRequest roleRequest = objectMapper.readValue(
                    req.getInputStream(), RoleUpdateRequest.class);

            userService.updateRole(userId, roleRequest.role());
            sendJson(resp, HttpServletResponse.SC_OK, new MessageResponse("Role updated"));
        } catch (NumberFormatException e) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid user ID");
        } catch (RuntimeException e) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, e.getMessage());
        }
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

    private boolean hasRole(HttpServletRequest req, UserRole role) {
        User currentUser = (User) req.getAttribute("currentUser");
        return currentUser != null && currentUser.getRole() == role;
    }

    private record ErrorResponse(String error) {
    }

    private record RoleUpdateRequest(String role) {
    }

    private record MessageResponse(String message) {
    }
}