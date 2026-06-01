package com.restaurant.controller;

import com.restaurant.model.dto.CreateOrderRequest;
import com.restaurant.model.dto.OrderDTO;
import com.restaurant.model.entity.OrderStatus;
import com.restaurant.model.entity.UserRole;
import com.restaurant.security.UserDetailsImpl;
import com.restaurant.service.OrderService;
import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpStatus;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.security.core.annotation.AuthenticationPrincipal;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseStatus;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.server.ResponseStatusException;

import java.util.List;

@RestController
@RequestMapping("/orders")
@RequiredArgsConstructor
public class OrderController {

    private final OrderService orderService;

    @GetMapping
    public List<OrderDTO> getAll(
            @RequestParam(required = false) String status,
            @AuthenticationPrincipal UserDetailsImpl currentUser) {

        if (currentUser.getUser().getRole() == UserRole.CLIENT) {
            return orderService.getByUser(currentUser.getUser());
        }

        if (status != null && !status.isBlank()) {
            return orderService.getByStatus(OrderStatus.valueOf(status.toUpperCase()));
        }

        return orderService.getAll();
    }

    @GetMapping("/{id}")
    public OrderDTO getById(@PathVariable Long id) {
        return orderService.getById(id)
                .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "Order not found"));
    }

    @PostMapping
    @PreAuthorize("!hasRole('KITCHEN')")
    @ResponseStatus(HttpStatus.CREATED)
    public OrderDTO create(
            @RequestBody CreateOrderRequest request,
            @AuthenticationPrincipal UserDetailsImpl currentUser) {
        return orderService.create(request, currentUser.getUser());
    }

    @PutMapping("/{id}")
    @PreAuthorize("!hasRole('CLIENT')")
    public OrderDTO updateStatus(@PathVariable Long id, @RequestBody StatusUpdateRequest request) {
        OrderStatus newStatus = OrderStatus.valueOf(request.status().toUpperCase());
        return orderService.updateStatus(id, newStatus)
                .orElseThrow(() -> new ResponseStatusException(HttpStatus.NOT_FOUND, "Order not found"));
    }

    private record StatusUpdateRequest(String status) {
    }
}
