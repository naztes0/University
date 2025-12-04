package com.example.seabattle; // Зверни увагу на назву пакету

import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.GridLayout;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;

import com.example.seabattle.R;
import com.example.seabattle.game.Board;
import com.example.seabattle.objects.Cell;

public class MainActivity extends AppCompatActivity {

    private Board enemyBoard;
    private GridLayout enemyGrid;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Знаходимо сітку ворога на екрані
        enemyGrid = findViewById(R.id.enemyGrid);

        // Ініціалізуємо логіку гри
        startNewGame();
    }

    private void startNewGame() {
        // Створюємо нове логічне поле
        enemyBoard = new Board();
        enemyBoard.placeTestShip(); // Ставимо тестовий корабель

        // Малюємо кнопки на екрані
        drawBoard();
    }

    private void drawBoard() {
        enemyGrid.removeAllViews(); // Очистити старі кнопки

        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                // Створюємо кнопку для кожної клітинки
                Button btn = new Button(this);

                // Налаштування розміру кнопки (щоб влізло 10 штук)
                // 90 пікселів ширина (приблизно)
                GridLayout.LayoutParams params = new GridLayout.LayoutParams();
                params.width = 80;
                params.height = 80;
                params.rowSpec = GridLayout.spec(y);
                params.columnSpec = GridLayout.spec(x);
                btn.setLayoutParams(params);

                // Запам'ятовуємо координати кнопки
                final int finalX = x;
                final int finalY = y;

                // Натискання на кнопку
                btn.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        handleShot(finalX, finalY, btn);
                    }
                });

                enemyGrid.addView(btn);
            }
        }
    }

    private void handleShot(int x, int y, Button btn) {
        int result = enemyBoard.shoot(x, y);

        if (result == -1) return; // Вже стріляли

        if (result == 0) {
            btn.setBackgroundColor(Color.GRAY); // Промах
            Toast.makeText(this, "Промах!", Toast.LENGTH_SHORT).show();
        } else if (result == 1) {
            btn.setBackgroundColor(Color.RED); // Влучив
            btn.setText("X");
        } else if (result == 2) {
            btn.setBackgroundColor(Color.BLACK); // Потопив
            btn.setText("☠");
            Toast.makeText(this, "Корабель потоплено!", Toast.LENGTH_SHORT).show();
        }

        btn.setEnabled(false); // Більше сюди тиснути не можна
    }
}