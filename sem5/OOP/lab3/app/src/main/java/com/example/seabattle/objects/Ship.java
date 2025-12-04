package com.example.seabattle.objects;

import java.util.List;

public class Ship {
    private List<Cell> cells;

    public Ship(List<Cell> cells) {
        this.cells = cells;
        for (Cell cell : cells) {
            cell.setShip(true);
        }
    }
    public List<Cell> getCells() {
        return cells;
    }

    public boolean belongsToShip(Cell cell) {
        return cells.contains(cell);
    }


    public boolean isSunk() {
        for (Cell cell : cells) {
            if (!cell.isHit()) {
                return false;
            }
        }
        return true;
    }
}