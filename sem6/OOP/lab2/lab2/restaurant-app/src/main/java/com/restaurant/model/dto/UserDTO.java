package com.restaurant.model.dto;

public record UserDTO(
        Long id,
        String name,
        String email,
        String role) {
}