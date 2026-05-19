package com.restaurant.mapper;

import com.restaurant.model.dto.OrderDTO;
import com.restaurant.model.entity.Order;
import org.mapstruct.Mapper;
import org.mapstruct.Mapping;
import org.mapstruct.factory.Mappers;

@Mapper(uses = OrderItemMapper.class)
public interface OrderMapper {

    OrderMapper INSTANCE = Mappers.getMapper(OrderMapper.class);

    @Mapping(target = "status", expression = "java(order.getStatus().name())")
    OrderDTO toDTO(Order order);
}