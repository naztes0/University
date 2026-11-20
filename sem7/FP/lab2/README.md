# Lab 2 — Parallel Shortest Path

Консольна програма на Haskell для пошуку найкоротшого шляху у зваженому графі паралельним алгоритмом Беллмана–Форда.

## Технології

- Haskell, GHC із ключем `-threaded`, Cabal
- `forkIO` і `MVar` для потоків
- `containers`, `deepseq`, `time`
- HUnit для тестів

## Запуск

```bash
cd lab2
cabal run
```

У меню можна запустити приклад, завантажити власний граф або порівняти час для різної кількості потоків.

Тести:

```bash
cabal test --test-show-details=direct
```

## Формат графа

```text
кількість_вершин кількість_ребер
звідки куди вага
```

Приклад знаходиться у `data/sample-graph.txt`.

## Структура

- `app/Main.hs` — запуск
- `src/Graph.hs` — граф, файл і генератор
- `src/ShortestPath.hs` — послідовний та паралельний алгоритми
- `src/Benchmark.hs` — час, прискорення й ефективність
- `src/Menu.hs` — консольне меню
- `test/TestMain.hs` — тести
