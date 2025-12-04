package com.example.seabattle.ui;

import android.graphics.Color;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.Button;
import android.widget.GridLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.example.seabattle.R;
import com.example.seabattle.game.Board;
import com.example.seabattle.objects.Cell;
import com.example.seabattle.objects.Ship;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Random;
import java.util.Stack;

public class MainActivity extends AppCompatActivity {

    private Board enemyBoard;
    private Board playerBoard;
    private GridLayout enemyGrid;
    private GridLayout playerGrid;
    private Random random = new Random();
    private boolean isGameFinished = false;
    private boolean isComputerShooting = false;
    private boolean isHardMode = false;

    // vars for thinking computer
    private Stack<Point> targetStack = new Stack<>();
    private Point firstHitOfShip = null;
    private Point lastHitPoint = null;

    private final int COLOR_WATER = Color.parseColor("#E3F2FD");
    private final int COLOR_SHIP_PLAYER = Color.GRAY;
    private final int COLOR_SUNK = Color.DKGRAY;
    private final int COLOR_MISS = Color.parseColor("#CFD8DC");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        enemyGrid = findViewById(R.id.enemyGrid);
        playerGrid = findViewById(R.id.playerGrid);
        Button btnRestart = findViewById(R.id.btnRestart);
        btnRestart.setOnClickListener(v -> showStartDialog());

        enemyGrid.post(this::showStartDialog);
    }

    private void showStartDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        View view = getLayoutInflater().inflate(R.layout.dialog_start_game, null);
        builder.setView(view);
        builder.setCancelable(false);

        AlertDialog dialog = builder.create();
        if (dialog.getWindow() != null) {
            dialog.getWindow().setBackgroundDrawable(
                    new android.graphics.drawable.ColorDrawable(android.graphics.Color.TRANSPARENT));
        }

        view.findViewById(R.id.btnEasy).setOnClickListener(v -> {
            isHardMode = false;
            dialog.dismiss();
            startNewGame();
        });

        view.findViewById(R.id.btnHard).setOnClickListener(v -> {
            isHardMode = true;
            dialog.dismiss();
            startNewGame();
        });

        dialog.show();
    }

    private void startNewGame() {
        isGameFinished = false;
        isComputerShooting = false;
        targetStack.clear();
        firstHitOfShip = null;
        lastHitPoint = null;

        enemyBoard = new Board();
        enemyBoard.placeShipsRandomly();
        drawEnemyBoard();

        playerBoard = new Board();
        playerBoard.placeShipsRandomly();
        drawPlayerBoard();

        setEnemyGridEnabled(true);
    }

    private void computerTurn() {
        if (isGameFinished)
            return;
        isComputerShooting = true;
        setEnemyGridEnabled(false);
        makeComputerMoveWithDelay();
    }

    private void makeComputerMoveWithDelay() {
        if (isGameFinished)
            return;

        new Handler(Looper.getMainLooper()).postDelayed(() -> {

            if (playerBoard.isGameOver()) {
                checkGameEnd();
                return;
            }

            int x, y;
            if (isHardMode && !targetStack.isEmpty()) {
                Point target = targetStack.pop();
                x = target.x;
                y = target.y;
            } else {
                x = random.nextInt(10);
                y = random.nextInt(10);
            }

            int result = playerBoard.shoot(x, y);
            if (result == -1) {
                makeComputerMoveWithDelay();
                return;
            }

            int index = y * 10 + x;
            ImageButton btn = (ImageButton) playerGrid.getChildAt(index);

            if (result == 0) {
                btn.setBackgroundColor(COLOR_MISS);
                if (isHardMode && firstHitOfShip != null && lastHitPoint != null) {
                }
                isComputerShooting = false;
                setEnemyGridEnabled(true);

            } else {
                btn.setImageResource(R.drawable.ic_cross);
                btn.setBackgroundColor(COLOR_SHIP_PLAYER);

                if (result == 2) {
                    refreshPlayerGrid();
                    if (isHardMode) {
                        targetStack.clear();
                        firstHitOfShip = null;
                        lastHitPoint = null;
                    }
                } else if (result == 1 && isHardMode) {
                    if (firstHitOfShip == null) {
                        firstHitOfShip = new Point(x, y);
                        lastHitPoint = new Point(x, y);
                        // adding 4 neighbors
                        addNeighborsToStack(x, y);
                    } else {

                        // Need to define the axis
                        boolean isVertical = (x == firstHitOfShip.x);
                        boolean isHorizontal = (y == firstHitOfShip.y);

                        filterStackByAxis(isVertical, isHorizontal);
                        int dx = x - lastHitPoint.x;
                        int dy = y - lastHitPoint.y;
                        addSpecificTarget(x + dx, y + dy);

                        if (targetStack.size() < 2) {
                            int reverseDx = (x - firstHitOfShip.x) > 0 ? -1 : (x - firstHitOfShip.x) < 0 ? 1 : 0;
                            int reverseDy = (y - firstHitOfShip.y) > 0 ? -1 : (y - firstHitOfShip.y) < 0 ? 1 : 0;

                            if (reverseDx == 0 && dx != 0)
                                reverseDx = -dx;
                            if (reverseDy == 0 && dy != 0)
                                reverseDy = -dy;

                            addSpecificTarget(firstHitOfShip.x + reverseDx, firstHitOfShip.y + reverseDy);
                        }

                        lastHitPoint = new Point(x, y);
                    }
                }

                if (playerBoard.isGameOver()) {
                    checkGameEnd();
                    return;
                }
                makeComputerMoveWithDelay();
            }
        }, 850);
    }

    // adding 4 neighbours after successfull shot
    private void addNeighborsToStack(int x, int y) {
        List<Point> neighbors = new ArrayList<>();
        neighbors.add(new Point(x + 1, y));
        neighbors.add(new Point(x - 1, y));
        neighbors.add(new Point(x, y + 1));
        neighbors.add(new Point(x, y - 1));
        Collections.shuffle(neighbors);

        for (Point p : neighbors) {
            if (isValidTarget(p.x, p.y)) {
                targetStack.push(p);
            }
        }
    }

    private void addSpecificTarget(int x, int y) {
        if (isValidTarget(x, y)) {
            targetStack.push(new Point(x, y));
        }
    }

    private boolean isValidTarget(int x, int y) {
        if (x >= 0 && x < 10 && y >= 0 && y < 10) {
            Cell cell = playerBoard.getCell(x, y);
            return !cell.isHit();
        }
        return false;
    }

    private void filterStackByAxis(boolean vertical, boolean horizontal) {
        if (!vertical && !horizontal)
            return;

        Stack<Point> newStack = new Stack<>();

        for (Point p : targetStack) {
            if (vertical && p.x == firstHitOfShip.x) {
                newStack.push(p);
            } else if (horizontal && p.y == firstHitOfShip.y) {
                newStack.push(p);
            }
        }
        targetStack = newStack;
    }

    private void setEnemyGridEnabled(boolean enabled) {
        for (int i = 0; i < enemyGrid.getChildCount(); i++) {
            int x = i % 10;
            int y = i / 10;
            if (!enemyBoard.getCell(x, y).isHit()) {
                enemyGrid.getChildAt(i).setEnabled(enabled);
            }
        }
    }

    private void drawEnemyBoard() {
        enemyGrid.removeAllViews();
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                ImageButton btn = new ImageButton(this);
                GridLayout.LayoutParams params = new GridLayout.LayoutParams();
                params.width = 80;
                params.height = 80;
                params.rowSpec = GridLayout.spec(y);
                params.columnSpec = GridLayout.spec(x);
                params.setMargins(1, 1, 1, 1);
                btn.setLayoutParams(params);
                btn.setBackgroundColor(COLOR_WATER);
                btn.setScaleType(ImageView.ScaleType.FIT_CENTER);
                btn.setPadding(15, 15, 15, 15);

                final int fx = x;
                final int fy = y;
                btn.setOnClickListener(v -> {
                    if (!isGameFinished && !isComputerShooting) {
                        handlePlayerShot(fx, fy, btn);
                    }
                });
                enemyGrid.addView(btn);
            }
        }
    }

    private void drawPlayerBoard() {
        playerGrid.removeAllViews();
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                ImageButton btn = new ImageButton(this);
                GridLayout.LayoutParams params = new GridLayout.LayoutParams();
                params.width = 80;
                params.height = 80;
                params.rowSpec = GridLayout.spec(y);
                params.columnSpec = GridLayout.spec(x);
                params.setMargins(1, 1, 1, 1);
                btn.setLayoutParams(params);
                btn.setScaleType(ImageView.ScaleType.FIT_CENTER);
                btn.setPadding(15, 15, 15, 15);

                Cell cell = playerBoard.getCell(x, y);
                if (cell.hasShip()) {
                    btn.setBackgroundColor(COLOR_SHIP_PLAYER);
                } else {
                    btn.setBackgroundColor(COLOR_WATER);
                }
                btn.setEnabled(false);
                btn.setTag(x + "," + y);
                playerGrid.addView(btn);
            }
        }
    }

    private void refreshEnemyGrid() {
        for (int i = 0; i < enemyGrid.getChildCount(); i++) {
            ImageButton btn = (ImageButton) enemyGrid.getChildAt(i);
            int x = i % 10;
            int y = i / 10;
            Cell cell = enemyBoard.getCell(x, y);

            btn.setImageResource(0);
            btn.setBackgroundColor(COLOR_WATER);

            if (cell.isHit() && !cell.hasShip()) {
                btn.setBackgroundColor(COLOR_MISS);
                btn.setEnabled(false);
            } else {
                btn.setEnabled(!cell.isHit());
            }
        }

        for (Ship ship : enemyBoard.getShips()) {
            boolean isSunk = ship.isSunk();
            for (Cell cell : ship.getCells()) {
                if (cell.isHit()) {
                    int index = cell.getY() * 10 + cell.getX();
                    ImageButton btn = (ImageButton) enemyGrid.getChildAt(index);
                    btn.setImageResource(R.drawable.ic_cross);
                    if (isSunk)
                        btn.setBackgroundColor(COLOR_SUNK);
                    else
                        btn.setBackgroundColor(COLOR_WATER);
                    btn.setEnabled(false);
                }
            }
        }
    }

    private void refreshPlayerGrid() {
        for (int i = 0; i < playerGrid.getChildCount(); i++) {
            ImageButton btn = (ImageButton) playerGrid.getChildAt(i);
            int x = i % 10;
            int y = i / 10;
            Cell cell = playerBoard.getCell(x, y);

            if (cell.isHit()) {
                if (cell.hasShip()) {
                    btn.setImageResource(R.drawable.ic_cross);
                } else {
                    btn.setBackgroundColor(COLOR_MISS);
                }
            }
        }
    }

    private void handlePlayerShot(int x, int y, ImageButton btn) {
        int result = enemyBoard.shoot(x, y);
        if (result == -1)
            return;

        refreshEnemyGrid();

        if (result == 0) {
            computerTurn();
        } else if (result == 1 || result == 2) {
            checkGameEnd();
        }
    }

    private void checkGameEnd() {

        if (isGameFinished)
            return;

        if (enemyBoard.isGameOver()) {
            isGameFinished = true;
            showCustomDialog("ПЕРЕМОГА!", "Ви знищили весь ворожий флот!", true);
        } else if (playerBoard.isGameOver()) {
            isGameFinished = true;
            showCustomDialog("ПОРАЗКА", "Ваш флот знищено.", false);
        }

    }

    private void showCustomDialog(String title, String message, boolean isWin) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        View view = getLayoutInflater().inflate(R.layout.dialog_game_over, null);
        builder.setView(view);
        builder.setCancelable(false);

        AlertDialog dialog = builder.create();
        if (dialog.getWindow() != null) {
            dialog.getWindow().setBackgroundDrawable(
                    new android.graphics.drawable.ColorDrawable(android.graphics.Color.TRANSPARENT));
        }

        android.widget.TextView tvTitle = view.findViewById(R.id.dialogTitle);
        android.widget.TextView tvMessage = view.findViewById(R.id.dialogMessage);
        Button btnReset = view.findViewById(R.id.dialogButton);

        tvTitle.setText(title);
        tvMessage.setText(message);

        if (isWin) {
            tvTitle.setTextColor(Color.parseColor("#4CAF50"));
        } else {
            tvTitle.setTextColor(Color.parseColor("#F44336"));
        }

        btnReset.setOnClickListener(v -> {
            dialog.dismiss();
            showStartDialog();
        });

        dialog.show();
    }
}