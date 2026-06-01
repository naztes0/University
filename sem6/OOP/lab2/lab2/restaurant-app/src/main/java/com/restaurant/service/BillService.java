package com.restaurant.service;

import com.restaurant.model.dto.BillDTO;
import com.restaurant.model.entity.*;
import com.restaurant.repository.BillRepository;
import com.restaurant.repository.OrderRepository;
import com.restaurant.repository.PaymentRepository;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.math.BigDecimal;
import java.util.Optional;

@Service
@RequiredArgsConstructor
public class BillService {

    private final BillRepository billRepository;
    private final OrderRepository orderRepository;
    private final PaymentRepository paymentRepository;

    public Optional<BillDTO> getByOrderId(Long orderId) {
        return orderRepository.findById(orderId)
                .flatMap(billRepository::findByOrder)
                .map(this::mapToDTO);
    }

    @Transactional
    public BillDTO create(Long orderId) {
        Order order = orderRepository.findById(orderId)
                .orElseThrow(() -> new RuntimeException("Order not found: " + orderId));

        if (billRepository.findByOrder(order).isPresent()) {
            throw new RuntimeException("Bill already exists for order: " + orderId);
        }

        BigDecimal total = order.getItems().stream()
                .map(item -> item.getPriceAtOrder()
                        .multiply(BigDecimal.valueOf(item.getQuantity())))
                .reduce(BigDecimal.ZERO, BigDecimal::add);

        Bill bill = Bill.builder()
                .order(order)
                .totalAmount(total)
                .status(BillStatus.UNPAID)
                .build();

        order.setStatus(OrderStatus.BILLED);
        orderRepository.save(order);

        return mapToDTO(billRepository.save(bill));
    }

    @Transactional
    public Optional<BillDTO> markAsPaid(Long billId) {
        return billRepository.findById(billId).map(bill -> {
            if (bill.getStatus() == BillStatus.PAID) {
                throw new RuntimeException("Bill already paid");
            }

            Payment payment = Payment.builder()
                    .bill(bill)
                    .amount(bill.getTotalAmount())
                    .build();
            paymentRepository.save(payment);

            bill.setStatus(BillStatus.PAID);
            bill.getOrder().setStatus(OrderStatus.PAID);
            orderRepository.save(bill.getOrder());

            return mapToDTO(billRepository.save(bill));
        });
    }

    private BillDTO mapToDTO(Bill bill) {
        return new BillDTO(
                bill.getId(),
                bill.getOrder().getId(),
                bill.getTotalAmount(),
                bill.getStatus().name(),
                bill.getCreatedAt());
    }
}