package com.restaurant.repository;

import com.restaurant.model.entity.Order;
import com.restaurant.model.entity.OrderStatus;
import com.restaurant.model.entity.User;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface OrderRepository extends JpaRepository<Order, Long> {
    List<Order> findByUser(User user);

    List<Order> findByStatus(OrderStatus status);
}