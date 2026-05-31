package com.restaurant.model.dto;

import java.time.LocalDateTime;
import java.util.List;

public record OrderDTO(
        Long id,
        Long userId,
        Integer tableNumber,
        String status,
        LocalDateTime createdAt,
        List<OrderItemDTO> items) {
}