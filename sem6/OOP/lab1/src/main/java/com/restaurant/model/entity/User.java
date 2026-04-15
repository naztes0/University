package com.restaurant.model.entity;

import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.time.LocalDateTime;

/**
 * Represents a user (client, admin or kitchen staff)
 */
@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
public class User {

    private Long id;
    private String auth0Id;
    private String name;
    private String email;
    private UserRole role;
    private LocalDateTime createdAt;
}