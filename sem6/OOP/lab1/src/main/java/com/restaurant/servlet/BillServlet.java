package com.restaurant.servlet;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule;
import com.restaurant.model.dto.BillDTO;
import com.restaurant.model.entity.User;
import com.restaurant.model.entity.UserRole;
import com.restaurant.service.BillService;
import com.restaurant.service.PaymentService;
import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.util.Optional;

/**
 * /api/bills endpoint
 */
@WebServlet("/bills/*")
public class BillServlet extends HttpServlet {

    private static final Logger logger = LogManager.getLogger(BillServlet.class);

    private BillService billService;
    private PaymentService paymentService;
    private ObjectMapper objectMapper;

    @Override
    public void init() {
        this.billService = new BillService();
        this.paymentService = new PaymentService();
        this.objectMapper = new ObjectMapper();
        this.objectMapper.registerModule(new JavaTimeModule());
    }

    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        logger.debug("GET /bills request");

        String orderIdParam = req.getParameter("orderId");
        if (orderIdParam == null) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "orderId parameter required");
            return;
        }

        try {
            Long orderId = Long.parseLong(orderIdParam);
            Optional<BillDTO> bill = billService.getByOrderId(orderId);

            if (bill.isPresent()) {
                sendJson(resp, HttpServletResponse.SC_OK, bill.get());
            } else {
                sendError(resp, HttpServletResponse.SC_NOT_FOUND, "Bill not found");
            }
        } catch (NumberFormatException e) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid orderId");
        }
    }

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        if (!hasRole(req, UserRole.ADMIN)) {
            logger.warn("Forbidden POST /bills attempt");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Admin access required");
            return;
        }

        String orderIdParam = req.getParameter("orderId");
        if (orderIdParam == null) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "orderId parameter required");
            return;
        }

        try {
            Long orderId = Long.parseLong(orderIdParam);
            BillDTO created = billService.create(orderId);
            sendJson(resp, HttpServletResponse.SC_CREATED, created);
        } catch (RuntimeException e) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, e.getMessage());
        }
    }

    @Override
    protected void doPut(HttpServletRequest req, HttpServletResponse resp) throws IOException {
        if (!hasRole(req, UserRole.CLIENT)) {
            logger.warn("Forbidden PUT /bills attempt");
            sendError(resp, HttpServletResponse.SC_FORBIDDEN, "Client access required");
            return;
        }

        String pathInfo = req.getPathInfo();
        if (pathInfo == null || !pathInfo.endsWith("/pay")) {
            sendError(resp, HttpServletResponse.SC_BAD_REQUEST, "Invalid endpoint");
            return;
        }

        try {
            Long billId = Long.parseLong(pathInfo.split("/")[1]);
            paymentService.pay(billId);
            sendJson(resp, HttpServletResponse.SC_OK, new MessageResponse("Payment successful"));
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

    private record MessageResponse(String message) {
    }
}