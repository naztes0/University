package com.restaurant.model.dto;

import java.math.BigDecimal;
import java.time.LocalDateTime;

public record BillDTO(
        Long id,
        Long orderId,
        BigDecimal totalAmount,
        String status,
        LocalDateTime createdAt) {
}
