package com.model;

import java.util.Objects;

/**
 * Main model class representing a knife/cold weapon
 */
public class Knife {
    private String id;
    private KnifeType type;
    private HandyType handy;
    private String origin;
    private Visual visual;
    private Value value;

    public Knife() {
    }

    public Knife(String id, KnifeType type, HandyType handy, String origin, Visual visual, Value value) {
        this.id = id;
        this.type = type;
        this.handy = handy;
        this.origin = origin;
        this.visual = visual;
        this.value = value;
    }

    // Getters and setters
    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public KnifeType getType() {
        return type;
    }

    public void setType(KnifeType type) {
        this.type = type;
    }

    public HandyType getHandy() {
        return handy;
    }

    public void setHandy(HandyType handy) {
        this.handy = handy;
    }

    public String getOrigin() {
        return origin;
    }

    public void setOrigin(String origin) {
        this.origin = origin;
    }

    public Visual getVisual() {
        return visual;
    }

    public void setVisual(Visual visual) {
        this.visual = visual;
    }

    public Value getValue() {
        return value;
    }

    public void setValue(Value value) {
        this.value = value;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        Knife knife = (Knife) o;
        return Objects.equals(id, knife.id) &&
                type == knife.type &&
                handy == knife.handy &&
                Objects.equals(origin, knife.origin) &&
                Objects.equals(visual, knife.visual) &&
                Objects.equals(value, knife.value);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, type, handy, origin, visual, value);
    }

    @Override
    public String toString() {
        return "Knife{" +
                "id='" + id + '\'' +
                ", type=" + type +
                ", handy=" + handy +
                ", origin='" + origin + '\'' +
                ", visual=" + visual +
                ", value=" + value +
                '}';
    }
}