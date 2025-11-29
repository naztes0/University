package com.model;

import java.util.Objects;

/**
 * Visual characteristics of the knife
 */
public class Visual {
    private Blade blade;
    private Material material;
    private Handle handle;
    private boolean bloodGroove;

    public Visual() {
    }

    public Visual(Blade blade, Material material, Handle handle, boolean bloodGroove) {
        this.blade = blade;
        this.material = material;
        this.handle = handle;
        this.bloodGroove = bloodGroove;
    }

    public Blade getBlade() {
        return blade;
    }

    public void setBlade(Blade blade) {
        this.blade = blade;
    }

    public Material getMaterial() {
        return material;
    }

    public void setMaterial(Material material) {
        this.material = material;
    }

    public Handle getHandle() {
        return handle;
    }

    public void setHandle(Handle handle) {
        this.handle = handle;
    }

    public boolean isBloodGroove() {
        return bloodGroove;
    }

    public void setBloodGroove(boolean bloodGroove) {
        this.bloodGroove = bloodGroove;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        Visual visual = (Visual) o;
        return bloodGroove == visual.bloodGroove &&
                Objects.equals(blade, visual.blade) &&
                Objects.equals(material, visual.material) &&
                Objects.equals(handle, visual.handle);
    }

    @Override
    public String toString() {
        return "Visual{" +
                "blade=" + blade +
                ", material=" + material +
                ", handle=" + handle +
                ", bloodGroove=" + bloodGroove +
                '}';
    }
}