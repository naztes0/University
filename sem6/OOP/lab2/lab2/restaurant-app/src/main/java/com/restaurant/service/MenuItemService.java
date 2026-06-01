package com.restaurant.service;

import com.restaurant.model.dto.MenuItemDTO;
import com.restaurant.model.entity.MenuItem;
import com.restaurant.repository.MenuItemRepository;
import lombok.RequiredArgsConstructor;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

/**
 * Business logic for menu items.
 * mapToDTO converts Entity to DTO inline — no MapStruct needed for simple
 * cases.
 */
@Service
@RequiredArgsConstructor
public class MenuItemService {

    private final MenuItemRepository menuItemRepository;

    public List<MenuItemDTO> getAllAvailable() {
        return menuItemRepository.findByAvailableTrue()
                .stream()
                .map(this::mapToDTO)
                .toList();
    }

    public List<MenuItemDTO> getAll() {
        return menuItemRepository.findAll()
                .stream()
                .map(this::mapToDTO)
                .toList();
    }

    public Optional<MenuItemDTO> getById(Long id) {
        return menuItemRepository.findById(id).map(this::mapToDTO);
    }

    public MenuItemDTO create(MenuItemDTO dto) {
        MenuItem entity = MenuItem.builder()
                .name(dto.name())
                .description(dto.description())
                .price(dto.price())
                .category(dto.category())
                .available(true)
                .build();
        return mapToDTO(menuItemRepository.save(entity));
    }

    public Optional<MenuItemDTO> update(Long id, MenuItemDTO dto) {
        return menuItemRepository.findById(id).map(existing -> {
            existing.setName(dto.name());
            existing.setDescription(dto.description());
            existing.setPrice(dto.price());
            existing.setCategory(dto.category());
            existing.setAvailable(dto.available());
            return mapToDTO(menuItemRepository.save(existing));
        });
    }

    public void delete(Long id) {
        menuItemRepository.deleteById(id);
    }

    private MenuItemDTO mapToDTO(MenuItem item) {
        return new MenuItemDTO(
                item.getId(),
                item.getName(),
                item.getDescription(),
                item.getPrice(),
                item.getCategory(),
                item.isAvailable());
    }
}