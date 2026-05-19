package com.restaurant.model.entity;

/**
 * Represents the lifecycle of an order
 */
public enum OrderStatus {
    NEW, // customer created the order
    CONFIRMED, // admin confirmed and sent to kitchen
    IN_KITCHEN, // kitchen took it
    READY, // kitchen marked as done
    BILLED, // admin issued the bill
    PAID // customer paid
}