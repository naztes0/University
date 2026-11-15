# Database quick guide

The project uses the MySQL database `display_class_schedule`.

## Start MySQL

```bash
brew services start mysql
```

## Create the database and tables

Run this command from the `lab1` directory:

```bash
mysql -u root < sql/schema.sql
```

The main database is intentionally created without records. New information can be added through the console application.

## Load optional sample data

```bash
mysql -u root < sql/seed.sql
```

This adds a small valid example containing classes, workstations, users, teachers, subjects, schedules, free-access periods, and usage records.

## Tables

- `display_classes` — computer classrooms.
- `workstations` — computers located in classrooms.
- `users` — students and teachers who use workstations.
- `teachers` — teachers who conduct scheduled classes.
- `subjects` — university subjects.
- `scheduled_classes` — planned lessons.
- `free_access` — periods available for independent work.
- `workstation_usage` — actual use of computers by users.

## Run the application

```bash
cabal run
```

## Run all tests

```bash
cabal test
```

The command runs:

- 14 unit tests for input validation;
- 8 integration tests for adding, reading, and updating every entity.

Integration tests use a temporary database named `display_class_schedule_test`. It is automatically removed after the tests and does not change the main database.

## Historical data backup

`sql/data_backup_before_cleanup.sql` contains the records that existed before the database was cleaned. It is kept only as a recovery archive and includes old records that were entered before validation was added. For normal demonstrations, use `sql/seed.sql` instead.
