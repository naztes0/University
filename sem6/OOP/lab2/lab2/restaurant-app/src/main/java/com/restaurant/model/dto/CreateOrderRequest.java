package com.restaurant.model.dto;

import java.util.List;

public record CreateOrderRequest(
        Integer tableNumber,
        List<OrderItemRequest> items) {
    public record OrderItemRequest(
            Long menuItemId,
            Integer quantity) {
    }
}