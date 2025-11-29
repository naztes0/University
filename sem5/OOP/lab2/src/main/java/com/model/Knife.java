package model;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * Main model class representing a knife/cold weapon
 */
public class Knife {
    private String id;
    private KnifeType type;
    private HandyType handy;
    private String origin;
    private List<Visual> visuals;
    private Value value;

    public Knife() {
        this.visuals = new ArrayList<>();
    }

    public Knife(String id, KnifeType type, HandyType handy, String origin, List<Visual> visuals, Value value) {
        this.id = id;
        this.type = type;
        this.handy = handy;
        this.origin = origin;
        this.visuals = visuals != null ? new ArrayList<>(visuals) : new ArrayList<>();
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

    public List<Visual> getVisuals() {
        return visuals;
    }

    public void setVisuals(List<Visual> visuals) {
        this.visuals = visuals;
    }

    public void addVisual(Visual visual) {
        this.visuals.add(visual);
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
                Objects.equals(visuals, knife.visuals) &&
                Objects.equals(value, knife.value);
    }

    @Override
    public String toString() {
        return "Knife{" +
                "id='" + id + '\'' +
                ", type=" + type +
                ", handy=" + handy +
                ", origin='" + origin + '\'' +
                ", visuals=" + visuals +
                ", value=" + value +
                '}';
    }
}