package com.restaurant.service;

import com.restaurant.mapper.BillMapper;
import com.restaurant.model.dto.BillDTO;
import com.restaurant.model.entity.Bill;
import com.restaurant.model.entity.BillStatus;
import com.restaurant.model.entity.OrderItem;
import com.restaurant.model.entity.OrderStatus;
import com.restaurant.repository.BillRepository;
import com.restaurant.repository.OrderItemRepository;
import com.restaurant.repository.OrderRepository;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.math.BigDecimal;
import java.util.List;
import java.util.Optional;

/**
 * Business logic for bills and payments.
 */
public class BillService {

    private static final Logger logger = LogManager.getLogger(BillService.class);

    private final BillRepository billRepository;
    private final OrderRepository orderRepository;
    private final OrderItemRepository orderItemRepository;
    private final BillMapper billMapper;

    public BillService() {
        this.billRepository = new BillRepository();
        this.orderRepository = new OrderRepository();
        this.orderItemRepository = new OrderItemRepository();
        this.billMapper = BillMapper.INSTANCE;
    }

    /**
     * Returns bill for a specific order.
     */
    public Optional<BillDTO> getByOrderId(Long orderId) {
        return billRepository.findByOrderId(orderId)
                .map(billMapper::toDTO);
    }

    /**
     * Creates a bill for a ready order — admin only.
     * Automatically calculates total from order items.
     */
    public BillDTO create(Long orderId) {
        // Check order exists and is in READY status
        orderRepository.findById(orderId).orElseThrow(() -> new RuntimeException("Order not found: " + orderId));

        // Check bill doesn't already exist for this order
        if (billRepository.findByOrderId(orderId).isPresent()) {
            throw new RuntimeException("Bill already exists for order: " + orderId);
        }

        // Calculate total from order items
        List<OrderItem> items = orderItemRepository.findByOrderId(orderId);
        BigDecimal total = items.stream()
                .map(item -> item.getPriceAtOrder()
                        .multiply(BigDecimal.valueOf(item.getQuantity())))
                .reduce(BigDecimal.ZERO, BigDecimal::add);

        // Save bill
        Bill bill = Bill.builder()
                .orderId(orderId)
                .totalAmount(total)
                .status(BillStatus.UNPAID)
                .build();

        Bill saved = billRepository.save(bill);

        // Update order status to BILLED
        orderRepository.updateStatus(orderId, OrderStatus.BILLED);

        logger.info("Bill created for order: {}, total: {}", orderId, total);
        return billMapper.toDTO(saved);
    }

    /**
     * Marks bill as paid — called after successful payment.
     */
    public Optional<BillDTO> markAsPaid(Long billId) {
        return billRepository.findById(billId).map(bill -> {
            if (bill.getStatus() == BillStatus.PAID) {
                throw new RuntimeException("Bill already paid: " + billId);
            }

            billRepository.updateStatus(billId, BillStatus.PAID);

            // Update order status to PAID
            orderRepository.updateStatus(bill.getOrderId(), OrderStatus.PAID);

            bill.setStatus(BillStatus.PAID);
            logger.info("Bill paid: {}", billId);
            return billMapper.toDTO(bill);
        });
    }
}