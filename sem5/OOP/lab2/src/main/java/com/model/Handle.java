package com.model;

import java.util.Objects;

/**
 * Handle information
 */
public class Handle {
    private HandleMaterial material;
    private WoodType woodType; // optional, only when material is WOOD

    public Handle() {
    }

    public Handle(HandleMaterial material, WoodType woodType) {
        this.material = material;
        this.woodType = woodType;
    }

    public HandleMaterial getMaterial() {
        return material;
    }

    public void setMaterial(HandleMaterial material) {
        this.material = material;
    }

    public WoodType getWoodType() {
        return woodType;
    }

    public void setWoodType(WoodType woodType) {
        this.woodType = woodType;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        Handle handle = (Handle) o;
        return material == handle.material && woodType == handle.woodType;
    }

    @Override
    public int hashCode() {
        return Objects.hash(material, woodType);
    }

    @Override
    public String toString() {
        return "Handle{" +
                "material=" + material +
                ", woodType=" + woodType +
                '}';
    }
}