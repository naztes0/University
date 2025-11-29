package com.model;

import java.util.Objects;

/**
 * Blade characteristics (length and width)
 */
public class Blade {
    private int length; // in mm
    private int width; // in mm

    public Blade() {
    }

    public Blade(int length, int width) {
        this.length = length;
        this.width = width;
    }

    public int getLength() {
        return length;
    }

    public void setLength(int length) {
        this.length = length;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        Blade blade = (Blade) o;
        return length == blade.length && width == blade.width;
    }

    @Override
    public int hashCode() {
        return Objects.hash(length, width);
    }

    @Override
    public String toString() {
        return "Blade{" +
                "length=" + length + "cm" +
                ", width=" + width + "mm" +
                '}';
    }
}