package com.restaurant.repository;

import com.restaurant.model.entity.Bill;
import com.restaurant.model.entity.Payment;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface PaymentRepository extends JpaRepository<Payment, Long> {
    Optional<Payment> findByBill(Bill bill);
}