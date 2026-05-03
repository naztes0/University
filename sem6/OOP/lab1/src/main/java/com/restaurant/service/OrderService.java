package com.restaurant.service;

import com.restaurant.mapper.OrderMapper;
import com.restaurant.model.dto.CreateOrderRequest;
import com.restaurant.model.dto.OrderDTO;
import com.restaurant.model.entity.Order;
import com.restaurant.model.entity.OrderItem;
import com.restaurant.model.entity.OrderStatus;
import com.restaurant.model.entity.MenuItem;
import com.restaurant.repository.OrderRepository;
import com.restaurant.repository.OrderItemRepository;
import com.restaurant.repository.MenuItemRepository;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

public class OrderService {

    private static final Logger logger = LogManager.getLogger(OrderService.class);

    private final OrderRepository orderRepository;
    private final OrderItemRepository orderItemRepository;
    private final MenuItemRepository menuItemRepository;
    private final OrderMapper orderMapper;

    public OrderService() {
        this.orderRepository = new OrderRepository();
        this.orderItemRepository = new OrderItemRepository();
        this.menuItemRepository = new MenuItemRepository();
        this.orderMapper = OrderMapper.INSTANCE;
    }

    /**
     * Returns all orders with their items (admin/kitchen)
     */
    public List<OrderDTO> getAll() {
        return orderRepository.findAll()
                .stream()
                .map(this::enrichWithItems) // add info about order items
                .map(orderMapper::toDTO)
                .collect(Collectors.toList());
    }

    /**
     * Returns all orders for a specific client
     */
    public List<OrderDTO> getByUserId(Long userId) {
        return orderRepository.findByUserId(userId)
                .stream()
                .map(this::enrichWithItems) // add info about order items
                .map(orderMapper::toDTO)
                .collect(Collectors.toList());
    }

    /**
     * Returns all orders with a specific status (kitchen)
     */
    public List<OrderDTO> getByStatus(OrderStatus status) {
        return orderRepository.findByStatus(status)
                .stream()
                .map(this::enrichWithItems)
                .map(orderMapper::toDTO)
                .collect(Collectors.toList());
    }

    /**
     * Returns a single order by ID.
     */
    public Optional<OrderDTO> getById(Long id) {
        return orderRepository.findById(id)
                .map(this::enrichWithItems)
                .map(orderMapper::toDTO);
    }

    /**
     * Creates a new order from client request.
     * 1. Validates all menu items exist
     * 2. Creates order record
     * 3. Creates order items with prices at time of order
     */
    public OrderDTO create(CreateOrderRequest request, Long userId) {
        // validate and build order items with current prices
        List<OrderItem> items = request.getItems().stream()
                .map(itemRequest -> {
                    MenuItem menuItem = menuItemRepository.findById(itemRequest.getMenuItemId())
                            .orElseThrow(() -> new RuntimeException(
                                    "Menu item not found: " + itemRequest.getMenuItemId()));

                    if (!menuItem.isAvailable()) {
                        throw new RuntimeException(
                                "Menu item not available: " + menuItem.getName());
                    }

                    return OrderItem.builder()
                            .menuItemId(menuItem.getId())
                            .menuItemName(menuItem.getName())
                            .quantity(itemRequest.getQuantity())
                            .priceAtOrder(menuItem.getPrice())
                            .build();
                })
                .collect(Collectors.toList());

        // save order
        Order order = Order.builder()
                .userId(userId)
                .tableNumber(request.getTableNumber())
                .status(OrderStatus.NEW)
                .build();

        Order saved = orderRepository.save(order);

        // attach order ID to each item and save all
        items.forEach(item -> item.setOrderId(saved.getId()));
        orderItemRepository.saveAll(items);

        saved.setItems(items);
        logger.info("Order created: {} for user: {}", saved.getId(), userId);
        return orderMapper.toDTO(saved);
    }

    /**
     * Updates order status (with status validation)
     */
    public Optional<OrderDTO> updateStatus(Long id, OrderStatus newStatus) {
        return orderRepository.findById(id).map(order -> {
            validateStatusTransition(order.getStatus(), newStatus);
            orderRepository.updateStatus(id, newStatus);
            order.setStatus(newStatus);
            logger.info("Order {} status changed: {} -> {}", id, order.getStatus(), newStatus);
            return orderMapper.toDTO(enrichWithItems(order));
        });
    }

    /**
     * Validates that status transition follows the correct flow
     */
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
            throw new RuntimeException(
                    "Invalid status transition: " + current + " -> " + next);
        }
    }

    /**
     * Loads order items and attaches them to the order.
     */
    private Order enrichWithItems(Order order) {
        List<OrderItem> items = orderItemRepository.findByOrderId(order.getId());
        order.setItems(items);
        return order;
    }
}