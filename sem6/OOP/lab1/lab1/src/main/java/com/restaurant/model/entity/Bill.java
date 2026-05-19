package com.restaurant.model.entity;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.math.BigDecimal;
import java.time.LocalDateTime;

/**
 * Represents a bill issued for an order
 */
@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class Bill {

    private Long id;
    private Long orderId;
    private BigDecimal totalAmount;
    private BillStatus status;
    private LocalDateTime createdAt;
}