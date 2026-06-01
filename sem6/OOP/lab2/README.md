# Restaurant Management System — Lab 2

**Умова:** система ресторану. Клієнт створює замовлення з меню. Адміністратор підтверджує замовлення, передає його на кухню та виставляє рахунок. Кухня змінює статус приготування. Клієнт оплачує рахунок.

---

## Tech Stack

| Layer | Technology |
|-------|------------|
| Backend | Java 17, Spring Boot, Spring Web |
| Database | PostgreSQL |
| ORM | Hibernate, Spring Data JPA |
| Migrations | Liquibase |
| Security | Spring Security, JWT |
| Frontend | React, TypeScript, Vite, Axios |
| Build | Maven |
| Web Server | Embedded Apache Tomcat |
| Code generation | Lombok, MapStruct dependency |

---

## Database Schema

```text
users
├── id BIGSERIAL PK
├── name VARCHAR
├── email VARCHAR UNIQUE
├── password_hash VARCHAR
├── role user_role (CLIENT | ADMIN | KITCHEN)
└── created_at TIMESTAMP

menu_items
├── id BIGSERIAL PK
├── name VARCHAR
├── description TEXT
├── price DECIMAL
├── category VARCHAR
└── available BOOLEAN

orders
├── id BIGSERIAL PK
├── user_id -> users.id
├── table_number INTEGER
├── status order_status
└── created_at TIMESTAMP

order_items
├── id BIGSERIAL PK
├── order_id -> orders.id
├── menu_item_id -> menu_items.id
├── quantity INTEGER
└── price_at_order DECIMAL

bills
├── id BIGSERIAL PK
├── order_id -> orders.id UNIQUE
├── total_amount DECIMAL
├── status bill_status (UNPAID | PAID)
└── created_at TIMESTAMP

payments
├── id BIGSERIAL PK
├── bill_id -> bills.id
├── amount DECIMAL
└── paid_at TIMESTAMP
```

---

## Order Lifecycle

```text
NEW -> CONFIRMED -> IN_KITCHEN -> READY -> BILLED -> PAID
 |        |            |           |        |        |
Client   Admin        Kitchen     Kitchen  Admin    Client
creates  confirms     starts      marks    creates  pays
order    order        cooking     ready    bill     bill
```

---

## API Base URL

Backend uses context path `/api`.

```text
http://localhost:8079/api
```

For frontend from Lab 1:

```env
VITE_API_URL=http://localhost:8079/api
```

---

## API Endpoints

### Auth

| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| POST | `/auth/register` | Public | Register new user |
| POST | `/auth/login` | Public | Login and receive JWT token |

### Menu

| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/menu` | Authenticated | Get menu items |
| GET | `/menu/{id}` | Authenticated | Get single menu item |
| POST | `/menu` | ADMIN | Create menu item |
| PUT | `/menu/{id}` | ADMIN | Update menu item |
| DELETE | `/menu/{id}` | ADMIN | Delete menu item |

### Orders

| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/orders` | Authenticated | Client gets own orders, admin/kitchen get all |
| GET | `/orders?status=READY` | ADMIN, KITCHEN | Filter orders by status |
| GET | `/orders/{id}` | Authenticated | Get single order |
| POST | `/orders` | CLIENT, ADMIN | Create new order |
| PUT | `/orders/{id}` | ADMIN, KITCHEN | Update order status |

### Bills

| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/bills?orderId={id}` | Authenticated | Get bill for order |
| POST | `/bills?orderId={id}` | ADMIN | Create bill |
| PUT | `/bills/{id}/pay` | CLIENT | Pay bill |

### Users

| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/users` | ADMIN | Get all users |
| PUT | `/users/{id}/role` | ADMIN | Update user role |

---

## Project Structure

```text
restaurant-app/
├── src/main/java/com/restaurant/
│   ├── config/
│   │   ├── DotenvConfig.java
│   │   └── SecurityConfig.java
│   ├── controller/
│   │   ├── AuthController.java
│   │   ├── BillController.java
│   │   ├── MenuItemController.java
│   │   ├── OrderController.java
│   │   └── UserController.java
│   ├── exception/
│   │   └── GlobalExceptionHandler.java
│   ├── model/
│   │   ├── dto/
│   │   └── entity/
│   ├── repository/
│   │   ├── BillRepository.java
│   │   ├── MenuItemRepository.java
│   │   ├── OrderRepository.java
│   │   ├── PaymentRepository.java
│   │   └── UserRepository.java
│   ├── security/
│   │   ├── JwtAuthFilter.java
│   │   ├── JwtUtil.java
│   │   ├── UserDetailsImpl.java
│   │   └── UserDetailsServiceImpl.java
│   ├── service/
│   │   ├── AuthService.java
│   │   ├── BillService.java
│   │   ├── MenuItemService.java
│   │   ├── OrderService.java
│   │   └── UserService.java
│   └── RestaurantAppApplication.java
├── src/main/resources/
│   ├── application.properties
│   └── db/changelog/
│       ├── db.changelog-master.xml
│       └── changes/V1__create_tables.xml
├── src/test/
│   ├── java/com/restaurant/RestaurantAppApplicationTests.java
│   └── resources/application-test.properties
├── mvnw
├── mvnw.cmd
└── pom.xml
```

---

## Configuration

Default application configuration:

```properties
server.port=8079
server.servlet.context-path=/api

spring.datasource.url=${DB_URL:jdbc:postgresql://localhost:5432/restaurant_db}
spring.datasource.username=${DB_USER:postgres}
spring.datasource.password=${DB_PASSWORD:postgres}
```

The app can run without a `.env` file if PostgreSQL uses these default values. For another database, set environment variables before running.

PowerShell example:

```powershell
$env:DB_URL="jdbc:postgresql://localhost:5432/restaurant_lab2"
$env:DB_USER="postgres"
$env:DB_PASSWORD="your_password"
$env:JWT_SECRET="restaurant-lab2-secret"
```
---

## Run


Set DB connection if needed:

```powershell
$env:DB_URL="jdbc:postgresql://localhost:5432/restaurant_lab2"
$env:DB_USER="postgres"
$env:DB_PASSWORD="your_password"
```

Run backend:

```powershell
.\mvnw.cmd spring-boot:run
```

---
