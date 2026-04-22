package com.restaurant.mapper;

import com.restaurant.model.dto.BillDTO;
import com.restaurant.model.entity.Bill;
import org.mapstruct.Mapper;
import org.mapstruct.Mapping;
import org.mapstruct.factory.Mappers;

@Mapper
public interface BillMapper {

    BillMapper INSTANCE = Mappers.getMapper(BillMapper.class);

    @Mapping(target = "status", expression = "java(bill.getStatus().name())")
    BillDTO toDTO(Bill bill);
}