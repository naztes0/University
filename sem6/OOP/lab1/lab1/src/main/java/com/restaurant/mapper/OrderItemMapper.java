package com.restaurant.mapper;

import com.restaurant.model.dto.OrderItemDTO;
import com.restaurant.model.entity.OrderItem;
import org.mapstruct.Mapper;
import org.mapstruct.Mapping;
import org.mapstruct.factory.Mappers;

@Mapper
public interface OrderItemMapper {

    OrderItemMapper INSTANCE = Mappers.getMapper(OrderItemMapper.class);

    OrderItemDTO toDTO(OrderItem orderItem);

    @Mapping(target = "orderId", ignore = true)
    OrderItem toEntity(OrderItemDTO orderItemDTO);
}