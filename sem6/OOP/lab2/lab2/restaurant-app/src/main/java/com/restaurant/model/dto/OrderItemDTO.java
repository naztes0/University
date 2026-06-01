package com.restaurant.model.dto;

import java.math.BigDecimal;

public record OrderItemDTO(
        Long id,
        Long menuItemId,
        String menuItemName,
        Integer quantity,
        BigDecimal priceAtOrder) {
}