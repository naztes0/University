package com.restaurant.model.entity;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.math.BigDecimal;

/**
 * Represents a menu item in the restaurant
 */

@Data // getters, setters, toString, equals, hashCode
@Builder
@NoArgsConstructor // constructor with no arguments (for DB reading)
@AllArgsConstructor // constructor with all arguments (for builder)
public class MenuItem {

    private Long id;
    private String name;
    private String description;
    private BigDecimal price;
    private String category;
    private boolean available;
}