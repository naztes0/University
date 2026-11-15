# Lab 1 — Display Class Schedule

Консольна система для обліку комп'ютерних класів, робочих місць, користувачів, викладачів, занять, вільного доступу та завантаженості.

## Технології

- Haskell, GHC, Cabal
- MySQL, `mysql-simple`
- HUnit для тестів

## Запуск

```bash
brew services start mysql
mysql -u root < sql/schema.sql
cabal run
```

Тестові дані за потреби: `mysql -u root < sql/seed.sql`.

Тести: `cabal test`.

## Структура

- `app/Main.hs` — запуск програми
- `src/` — моделі, БД, меню та валідація
- `sql/` — схема і тестові дані
- `test/` — юніт-тести
- `test-integration/` — інтеграційні тести MySQL
