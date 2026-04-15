package com.restaurant.model.dto;

import lombok.NoArgsConstructor;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;

import java.time.LocalDateTime;
import java.util.List;

/**
 * DTO for an order
 */
@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor

public class OrderDTO {
    private Long id;
    private Long userId;
    private Integer tableNumber;
    private String status;
    private LocalDateTime createdAt;

    private List<OrderItemDTO> items;
}
