package com.restaurant.model.dto;

public record AuthResponse(
        String token,
        String role) {
}