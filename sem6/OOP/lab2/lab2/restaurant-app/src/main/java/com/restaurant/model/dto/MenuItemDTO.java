package com.restaurant.model.dto;

import java.math.BigDecimal;

public record MenuItemDTO(
        Long id,
        String name,
        String description,
        BigDecimal price,
        String category,
        boolean available) {
}