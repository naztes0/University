package com.restaurant.model.dto;

public record AuthRequest(
                String name,
                String email,
                String password) {
}