package com.restaurant.servlet;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.restaurant.model.dto.MenuItemDTO;
import com.restaurant.model.entity.User;
import com.restaurant.model.entity.UserRole;
import com.restaurant.service.MenuItemService;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.List;
import java.util.Optional;

/**
 * requests /api/menu endpoint
 */
@WebServlet("/menu/*")
public class MenuItemServlet extends HttpServlet {

    private static final Logger logger = LogManager.getLogger(MenuItemServlet.class);

    private MenuItemService menuItemService;
    private ObjectMapper objectMapper;

    @Override
    public void init() {
        this.menuItemService = new MenuItemService();
        this.objectMapper = new ObjectMapper();
        this.objectMapper.registerModule(new JavaTimeModule());
    }

    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        String pathInfo = req.getPathInfo();
        logger.debug("GET /menu request, pathInfo: {}", pathInfo);

        if (pathInfo == null || pathInfo.equals("/")) {
            User currentUser = (User) req.getAttribute("currentUser");
            List<MenuItemDTO> items;

            if (currentUser != null && currentUser.getRole() == UserRole.ADMIN) {
                items = menuItemService.getAll();
            } else {
                items = menuItemService.getAllAvailable();
            }

            sendJson(resp, HttpServletResponse.SC_OK, items);

        } else {
            Long id = parseId(pathInfo);
            if (id == null) {
                sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid ID");
                return;
            }

            Optional<MenuItemDTO> item = menuItemService.getById(id);
            if (item.isPresent()) {
                sendJson(resp, HttpServletResponse.SC_OK, item.get());
            } else {
                sendError(resp, HttpServletResponse.SC_NOT_FOUND, "Menu item not found");
            }
        }
    }

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        if (!hasRole(req, UserRole.ADMIN)) {
            logger.warn("Forbidden POST /menu attempt");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Admin access required");
            return;
        }

        MenuItemDTO dto = objectMapper.readValue(req.getInputStream(), MenuItemDTO.class);
        MenuItemDTO created = menuItemService.create(dto);
        sendJson(resp, HttpServletResponse.SC_CREATED, created);
    }

    @Override
    protected void doPut(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        if (!hasRole(req, UserRole.ADMIN)) {
            logger.warn("Forbidden PUT /menu attempt");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Admin access required");
            return;
        }

        Long id = parseId(req.getPathInfo());
        if (id == null) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid ID");
            return;
        }

        MenuItemDTO dto = objectMapper.readValue(req.getInputStream(), MenuItemDTO.class);
        Optional<MenuItemDTO> updated = menuItemService.update(id, dto);

        if (updated.isPresent()) {
            sendJson(resp, HttpServletResponse.SC_OK, updated.get());
        } else {
            sendError(resp, HttpServletResponse.SC_NOT_FOUND, "Menu item not found");
        }
    }

    @Override
    protected void doDelete(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        if (!hasRole(req, UserRole.ADMIN)) {
            logger.warn("Forbidden DELETE /menu attempt");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Admin access required");
            return;
        }

        Long id = parseId(req.getPathInfo());
        if (id == null) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid ID");
            return;
        }

        menuItemService.delete(id);
        resp.setStatus(HttpServletResponse.SC_NO_CONTENT);
    }

    /*
     * Helpers
     */

    private void sendJson(HttpServletResponse resp, int status, Object data) throws IOException {
        resp.setStatus(status);
        resp.setContentType("application/json");
        resp.setCharacterEncoding("UTF-8");
        resp.getWriter().write(objectMapper.writeValueAsString(data));
    }

    private void sendError(HttpServletResponse resp, int status, String message) throws IOException {
        sendJson(resp, status, new ErrorResponse(message));
    }

    private Long parseId(String pathInfo) {
        try {
            return Long.parseLong(pathInfo.substring(1));
        } catch (Exception e) {
            return null;
        }
    }

    private boolean hasRole(HttpServletRequest req, UserRole role) {
        User currentUser = (User) req.getAttribute("currentUser");
        return currentUser != null && currentUser.getRole() == role;
    }

    private record ErrorResponse(String error) {
    }
}