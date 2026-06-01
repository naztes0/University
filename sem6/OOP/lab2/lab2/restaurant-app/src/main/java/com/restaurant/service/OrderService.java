package com.restaurant.service;

import com.restaurant.model.dto.CreateOrderRequest;
import com.restaurant.model.dto.OrderDTO;
import com.restaurant.model.dto.OrderItemDTO;
import com.restaurant.model.entity.*;
import com.restaurant.repository.MenuItemRepository;
import com.restaurant.repository.OrderRepository;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.Optional;

@Service
@RequiredArgsConstructor
public class OrderService {

    private final OrderRepository orderRepository;
    private final MenuItemRepository menuItemRepository;

    public List<OrderDTO> getAll() {
        return orderRepository.findAll().stream().map(this::mapToDTO).toList();
    }

    public List<OrderDTO> getByUser(User user) {
        return orderRepository.findByUser(user).stream().map(this::mapToDTO).toList();
    }

    public List<OrderDTO> getByStatus(OrderStatus status) {
        return orderRepository.findByStatus(status).stream().map(this::mapToDTO).toList();
    }

    public Optional<OrderDTO> getById(Long id) {
        return orderRepository.findById(id).map(this::mapToDTO);
    }

    @Transactional
    public OrderDTO create(CreateOrderRequest request, User user) {
        Order order = Order.builder()
                .user(user)
                .tableNumber(request.tableNumber())
                .status(OrderStatus.NEW)
                .build();

        List<OrderItem> items = request.items().stream().map(itemRequest -> {
            MenuItem menuItem = menuItemRepository.findById(itemRequest.menuItemId())
                    .orElseThrow(() -> new RuntimeException("Menu item not found: " + itemRequest.menuItemId()));

            if (!menuItem.isAvailable()) {
                throw new RuntimeException("Menu item not available: " + menuItem.getName());
            }

            return OrderItem.builder()
                    .order(order)
                    .menuItem(menuItem)
                    .quantity(itemRequest.quantity())
                    .priceAtOrder(menuItem.getPrice())
                    .build();
        }).toList();

        order.setItems(items);
        return mapToDTO(orderRepository.save(order));
    }

    @Transactional
    public Optional<OrderDTO> updateStatus(Long id, OrderStatus newStatus) {
        return orderRepository.findById(id).map(order -> {
            validateStatusTransition(order.getStatus(), newStatus);
            order.setStatus(newStatus);
            return mapToDTO(orderRepository.save(order));
        });
    }

    private void validateStatusTransition(OrderStatus current, OrderStatus next) {
        boolean valid = switch (current) {
            case NEW -> next == OrderStatus.CONFIRMED;
            case CONFIRMED -> next == OrderStatus.IN_KITCHEN;
            case IN_KITCHEN -> next == OrderStatus.READY;
            case READY -> next == OrderStatus.BILLED;
            case BILLED -> next == OrderStatus.PAID;
            case PAID -> false;
        };
        if (!valid) {
            throw new RuntimeException("Invalid status transition: " + current + " -> " + next);
        }
    }

    private OrderDTO mapToDTO(Order order) {
        List<OrderItemDTO> items = order.getItems() == null ? List.of()
                : order.getItems().stream().map(item -> new OrderItemDTO(
                        item.getId(),
                        item.getMenuItem().getId(),
                        item.getMenuItem().getName(),
                        item.getQuantity(),
                        item.getPriceAtOrder())).toList();

        return new OrderDTO(
                order.getId(),
                order.getUser().getId(),
                order.getTableNumber(),
                order.getStatus().name(),
                order.getCreatedAt(),
                items);
    }
}