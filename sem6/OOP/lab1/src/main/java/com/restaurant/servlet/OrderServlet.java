package com.restaurant.servlet;

import java.io.IOException;
import java.util.List;
import java.util.Optional;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.restaurant.model.dto.CreateOrderRequest;
import com.restaurant.model.dto.OrderDTO;
import com.restaurant.model.entity.OrderStatus;
import com.restaurant.model.entity.User;
import com.restaurant.model.entity.UserRole;
import com.restaurant.service.OrderService;

import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

@WebServlet("/orders/*")
public class OrderServlet extends HttpServlet {

    private static final Logger logger = LogManager.getLogger(OrderServlet.class);

    private OrderService orderService;
    private ObjectMapper objectMapper;

    @Override
    public void init() {
        this.orderService = new OrderService();
        this.objectMapper = new ObjectMapper();
        this.objectMapper.registerModule(new JavaTimeModule());
    }

    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        String pathInfo = req.getPathInfo();
        logger.debug("GET /orders request, pathInfo: {}", pathInfo);

        User currentUser = (User) req.getAttribute("currentUser");
        if (currentUser == null) {
            sendError(resp, HttpServletResponse.SC_UNAUTHORIZED, "Unauthorized");
            return;
        }
        if (pathInfo == null || pathInfo.equals("/")) {
            List<OrderDTO> orders;

            if (currentUser.getRole() == UserRole.CLIENT) {
                orders = orderService.getByUserId(currentUser.getId());
            } else {
                String statusParam = req.getParameter("status");
                if (statusParam != null) {
                    OrderStatus status = OrderStatus.valueOf(statusParam.toUpperCase());
                    orders = orderService.getByStatus(status);
                } else {
                    orders = orderService.getAll();
                }
            }

            sendJson(resp, HttpServletResponse.SC_OK, orders);

        } else {
            Long id = parseId(pathInfo);
            if (id == null) {
                sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid ID");
                return;
            }

            Optional<OrderDTO> order = orderService.getById(id);
            if (order.isPresent()) {
                sendJson(resp, HttpServletResponse.SC_OK, order.get());
            } else {
                sendError(resp, HttpServletResponse.SC_NOT_FOUND, "Order not found");
            }
        }
    }

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws IOException {

        User currentUser = (User) req.getAttribute("currentUser");
        if (currentUser == null) {
            sendError(resp, HttpServletResponse.SC_UNAUTHORIZED, "Unauthorized");
            return;
        }
        if (currentUser.getRole() == UserRole.KITCHEN) {
            logger.warn("Forbidden POST /orders attempt by kitchen");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Access denied");
            return;
        }

        CreateOrderRequest request = objectMapper.readValue(
                req.getInputStream(), CreateOrderRequest.class);

        try {
            OrderDTO created = orderService.create(request, currentUser.getId());
            sendJson(resp, HttpServletResponse.SC_CREATED, created);
        } catch (RuntimeException e) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, e.getMessage());
        }
    }

    @Override
    protected void doPut(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        User currentUser = (User) req.getAttribute("currentUser");
        if (currentUser == null) {
            sendError(resp, HttpServletResponse.SC_UNAUTHORIZED, "Unauthorized");
            return;
        }
        if (currentUser.getRole() == UserRole.CLIENT) {
            logger.warn("Forbidden PUT /orders attempt by client");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Access denied");
            return;
        }

        Long id = parseId(req.getPathInfo());
        if (id == null) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid ID");
            return;
        }

        StatusUpdateRequest statusRequest = objectMapper.readValue(
                req.getInputStream(), StatusUpdateRequest.class);

        try {
            OrderStatus newStatus = OrderStatus.valueOf(
                    statusRequest.status().toUpperCase());
            Optional<OrderDTO> updated = orderService.updateStatus(id, newStatus);

            if (updated.isPresent()) {
                sendJson(resp, HttpServletResponse.SC_OK, updated.get());
            } else {
                sendError(resp, HttpServletResponse.SC_NOT_FOUND, "Order not found");
            }
        } catch (IllegalArgumentException e) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid status value");
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

    private Long parseId(String pathInfo) {
        try {
            return Long.parseLong(pathInfo.substring(1));
        } catch (Exception e) {
            return null;
        }
    }

    private record ErrorResponse(String error) {
    }

    private record StatusUpdateRequest(String status) {
    }
}