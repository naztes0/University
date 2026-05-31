package com.restaurant.repository;

import com.restaurant.model.entity.Bill;
import com.restaurant.model.entity.Order;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface BillRepository extends JpaRepository<Bill, Long> {
    Optional<Bill> findByOrder(Order order);
}