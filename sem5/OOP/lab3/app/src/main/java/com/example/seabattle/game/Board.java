package com.example.seabattle.game;

import com.example.seabattle.objects.Cell;
import com.example.seabattle.objects.Ship;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class Board {
    private Cell[][] cells = new Cell[10][10];
    private List<Ship> ships = new ArrayList<>();
    private Random random = new Random();

    public Board() {
        for (int x = 0; x < 10; x++) {
            for (int y = 0; y < 10; y++) {
                cells[x][y] = new Cell(x, y);
            }
        }
    }

    public void placeShipsRandomly() {

        // Ships
        placeShipOfSize(4);
        placeShipOfSize(3);
        placeShipOfSize(3);
        placeShipOfSize(2);
        placeShipOfSize(2);
        placeShipOfSize(2);
        placeShipOfSize(1);
        placeShipOfSize(1);
        placeShipOfSize(1);
        placeShipOfSize(1);
    }

    // Set ship with given size
    private void placeShipOfSize(int size) {
        while (true) {
            int x = random.nextInt(10);
            int y = random.nextInt(10);
            boolean horizontal = random.nextBoolean();

            if (canPlaceShip(x, y, size, horizontal)) {
                createAndAddShip(x, y, size, horizontal);
                break;
            }
        }
    }

    // Check if ship fits and dont touch other ships
    private boolean canPlaceShip(int x, int y, int size, boolean horizontal) {
        for (int i = 0; i < size; i++) {
            int checkX = horizontal ? x + i : x;
            int checkY = horizontal ? y : y + i;
            if (checkX >= 10 || checkY >= 10)
                return false;

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int neighborX = checkX + dx;
                    int neighborY = checkY + dy;

                    if (neighborX >= 0 && neighborX < 10 && neighborY >= 0 && neighborY < 10) {
                        if (cells[neighborX][neighborY].hasShip()) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    private void createAndAddShip(int x, int y, int size, boolean horizontal) {
        List<Cell> shipCells = new ArrayList<>();
        for (int i = 0; i < size; i++) {
            int cellX = horizontal ? x + i : x;
            int cellY = horizontal ? y : y + i;
            shipCells.add(cells[cellX][cellY]);
        }
        Ship newShip = new Ship(shipCells);
        ships.add(newShip);
    }

    public Cell getCell(int x, int y) {
        if (x >= 0 && x < 10 && y >= 0 && y < 10) {
            return cells[x][y];
        }
        return null;
    }

    public int shoot(int x, int y) {
        Cell cell = getCell(x, y);
        if (cell == null || cell.isHit())
            return -1;

        cell.shoot();

        if (cell.hasShip()) {
            for (Ship ship : ships) {
                if (ship.belongsToShip(cell)) {
                    if (ship.isSunk()) {
                        markNeighbors(ship);
                        return 2;
                    } else {
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    private void markNeighbors(Ship ship) {
        for (Cell shipCell : ship.getCells()) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = shipCell.getX() + dx;
                    int ny = shipCell.getY() + dy;

                    Cell neighbor = getCell(nx, ny);
                    if (neighbor != null && !ship.belongsToShip(neighbor)) {
                        neighbor.shoot();
                    }
                }
            }
        }
    }

    public boolean isGameOver() {
        for (Ship ship : ships) {
            if (!ship.isSunk()) {
                return false;
            }
        }
        return true;
    }

    public List<Ship> getShips() {
        return ships;
    }
}