package com.model;

import java.util.Objects;

/**
 * Value information - whether knife is collectible
 */
public class Value {
    private boolean collectible;

    public Value() {
    }

    public Value(boolean collectible) {
        this.collectible = collectible;
    }

    public boolean isCollectible() {
        return collectible;
    }

    public void setCollectible(boolean collectible) {
        this.collectible = collectible;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        Value value = (Value) o;
        return collectible == value.collectible;
    }

    @Override
    public String toString() {
        return "Value{" +
                "collectible=" + collectible +
                '}';
    }
}