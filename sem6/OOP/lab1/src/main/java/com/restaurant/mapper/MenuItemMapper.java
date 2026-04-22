package com.restaurant.mapper;

import com.restaurant.model.dto.MenuItemDTO;
import com.restaurant.model.entity.MenuItem;
import org.mapstruct.Mapper;
import org.mapstruct.factory.Mappers;

@Mapper
public interface MenuItemMapper {

    MenuItemMapper INSTANCE = Mappers.getMapper(MenuItemMapper.class);

    MenuItemDTO toDTO(MenuItem menuItem);

    MenuItem toEntity(MenuItemDTO menuItemDTO);
}