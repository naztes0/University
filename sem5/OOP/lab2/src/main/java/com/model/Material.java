package com.model;

import java.util.Objects;

/**
 * Material information for blade
 */
public class Material {
    private BladeMaterial blade;

    public Material() {
    }

    public Material(BladeMaterial blade) {
        this.blade = blade;
    }

    public BladeMaterial getBlade() {
        return blade;
    }

    public void setBlade(BladeMaterial blade) {
        this.blade = blade;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        Material material = (Material) o;
        return blade == material.blade;
    }

    @Override
    public String toString() {
        return "Material{" +
                "blade=" + blade +
                '}';
    }
}