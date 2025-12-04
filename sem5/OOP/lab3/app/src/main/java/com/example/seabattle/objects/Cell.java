package com.example.seabattle.objects;

public class Cell {
    private final int x;
    private final int y;
    private boolean hasShip;
    private boolean isHit;

    public Cell(int x, int y) {
        this.x = x;
        this.y = y;
        this.hasShip = false;
        this.isHit = false;
    }

    public void shoot() {
        this.isHit = true;
    }

    public boolean hasShip() {
        return hasShip;
    }

    public void setShip(boolean hasShip) {
        this.hasShip = hasShip;
    }

    public boolean isHit() {
        return isHit;
    }

    public int getX(){
        return x;
    }
    public int getY(){
        return y;
    }
}