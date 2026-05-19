package com.restaurant.service;

import com.restaurant.mapper.MenuItemMapper;
import com.restaurant.model.dto.MenuItemDTO;
import com.restaurant.model.entity.MenuItem;
import com.restaurant.repository.MenuItemRepository;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

public class MenuItemService {

    private static final Logger logger = LogManager.getLogger(MenuItemService.class);

    private final MenuItemRepository menuItemRepository;
    private final MenuItemMapper menuItemMapper;

    public MenuItemService() {
        this.menuItemRepository = new MenuItemRepository();
        this.menuItemMapper = MenuItemMapper.INSTANCE;
    }

    /**
     * Returns all available menu items.
     */
    public List<MenuItemDTO> getAllAvailable() {
        return menuItemRepository.findAll()
                .stream()
                .filter(MenuItem::isAvailable) // only available items for clients
                .map(menuItemMapper::toDTO)
                .collect(Collectors.toList());
    }

    public List<MenuItemDTO> getAll() {
        return menuItemRepository.findAll()
                .stream()
                .map(menuItemMapper::toDTO)
                .collect(Collectors.toList());
    }

    public Optional<MenuItemDTO> getById(Long id) {
        return menuItemRepository.findById(id)
                .map(menuItemMapper::toDTO);
    }

    public MenuItemDTO create(MenuItemDTO dto) {
        MenuItem entity = menuItemMapper.toEntity(dto);
        entity.setAvailable(true);
        MenuItem saved = menuItemRepository.save(entity);
        logger.info("Menu item created: {}", saved.getId());
        return menuItemMapper.toDTO(saved);
    }

    public Optional<MenuItemDTO> update(Long id, MenuItemDTO dto) {
        return menuItemRepository.findById(id).map(existing -> {
            existing.setName(dto.getName());
            existing.setDescription(dto.getDescription());
            existing.setPrice(dto.getPrice());
            existing.setCategory(dto.getCategory());
            existing.setAvailable(dto.isAvailable());
            menuItemRepository.update(existing);
            logger.info("Menu item updated: {}", id);
            return menuItemMapper.toDTO(existing);
        });
    }

    public void delete(Long id) {
        menuItemRepository.delete(id);
        logger.info("Menu item deleted: {}", id);
    }
}