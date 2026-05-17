package com.restaurant.service;

import java.util.Optional;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.restaurant.model.entity.Bill;
import com.restaurant.model.entity.BillStatus;
import com.restaurant.model.entity.Payment;
import com.restaurant.repository.BillRepository;
import com.restaurant.repository.PaymentRepository;

public class PaymentService {

    private static final Logger logger = LogManager.getLogger(PaymentService.class);

    private final PaymentRepository paymentRepository;
    private final BillRepository billRepository;
    private final BillService billService;

    public PaymentService() {
        this.paymentRepository = new PaymentRepository();
        this.billRepository = new BillRepository();
        this.billService = new BillService();
    }

    public Payment pay(Long billId) {
        Optional<Bill> billOptional = billRepository.findById(billId);
        if (billOptional.isEmpty()) {
            throw new RuntimeException("Bill not found: " + billId);
        }

        Bill bill = billOptional.get();
        if (bill.getStatus() == BillStatus.PAID) {
            throw new RuntimeException("Bill already paid: " + billId);
        }

        Payment payment = Payment.builder()
                .billId(billId)
                .amount(bill.getTotalAmount())
                .build();

        Payment saved = paymentRepository.save(payment);
        billService.markAsPaid(billId);

        logger.info("Payment processed for bill: {}, amount: {}", billId, bill.getTotalAmount());
        return saved;
    }
}