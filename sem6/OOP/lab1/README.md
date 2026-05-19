# Restaurant Management System — Lab 1

> **Умова:** Система Ресторан. Клієнт здійснює Замовлення із меню. Адміністратор підтверджує Замовлення та відправляє його на кухню для виконання. Адміністратор виставляє Рахунок. Клієнт здійснює його оплату.

---

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Backend | Java 17, Servlets, Filters, JDBC |
| Database | PostgreSQL, Flyway migrations |
| Frontend | React, TypeScript, Vite, Axios |
| Auth | auth0-jwt |
| Web Server | Apache Tomcat 10 (via Cargo Maven plugin) |
| Build | Maven |
| Logging | Log4j2 |
| Code generation | Lombok, MapStruct |



---

## Database Schema

```
users
├── id SERIAL PK
├── auth0_id VARCHAR (nullable)
├── name VARCHAR
├── email VARCHAR UNIQUE
├── password_hash VARCHAR
├── role user_role (CLIENT | ADMIN | KITCHEN)
└── created_at TIMESTAMP

menu_items
├── id SERIAL PK
├── name VARCHAR
├── description TEXT
├── price DECIMAL
├── category VARCHAR
└── available BOOLEAN

orders
├── id SERIAL PK
├── user_id → users.id
├── table_number INTEGER
├── status order_status
└── created_at TIMESTAMP

order_items
├── id SERIAL PK
├── order_id → orders.id
├── menu_item_id → menu_items.id
├── quantity INTEGER
└── price_at_order DECIMAL

bills
├── id SERIAL PK
├── order_id → orders.id (UNIQUE)
├── total_amount DECIMAL
├── status bill_status (UNPAID | PAID)
└── created_at TIMESTAMP

payments
├── id SERIAL PK
├── bill_id → bills.id
├── amount DECIMAL
└── paid_at TIMESTAMP
```

---

## Order Lifecycle

```
NEW → CONFIRMED → IN_KITCHEN → READY → BILLED → PAID
 ↑         ↑           ↑          ↑        ↑       ↑
Client   Admin      Kitchen    Kitchen   Admin  Client
creates confirms   takes it   marks   issues  pays
order   & sends              ready    bill
```

---

## API Endpoints

### Auth (public)
| Method | Endpoint | Description |
|--------|----------|-------------|
| POST | `/auth/register` | Register new user |
| POST | `/auth/login` | Login and receive JWT token |

### Menu
| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/menu` | All | Get available menu items |
| GET | `/menu/{id}` | All | Get single menu item |
| POST | `/menu` | ADMIN | Create menu item |
| PUT | `/menu/{id}` | ADMIN | Update menu item |
| DELETE | `/menu/{id}` | ADMIN | Delete menu item |

### Orders
| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/orders` | All | Get orders (client: own, admin/kitchen: all) |
| GET | `/orders?status=` | ADMIN, KITCHEN | Filter by status |
| GET | `/orders/{id}` | All | Get single order |
| POST | `/orders` | CLIENT, ADMIN | Create new order |
| PUT | `/orders/{id}` | ADMIN, KITCHEN | Update order status |

### Bills
| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/bills?orderId=` | All | Get bill for order |
| POST | `/bills?orderId=` | ADMIN | Create bill for ready order |
| PUT | `/bills/{id}/pay` | CLIENT | Pay a bill |

### Users
| Method | Endpoint | Access | Description |
|--------|----------|--------|-------------|
| GET | `/users` | ADMIN | Get all users |
| GET | `/users/me` | All | Get current user profile |
| PUT | `/users/{id}/role` | ADMIN | Update user role |

---

## Project Structure

```
lab1/                                  ← Backend (Maven)
├── src/main/java/com/restaurant/
│   ├── filter/
│   │   ├── AuthFilter.java            ← JWT validation on every request
│   │   └── CorsFilter.java            ← CORS headers for frontend
│   ├── mapper/
│   │   ├── BillMapper.java
│   │   ├── MenuItemMapper.java
│   │   ├── OrderItemMapper.java
│   │   ├── OrderMapper.java
│   │   └── UserMapper.java
│   ├── model/
│   │   ├── dto/                       ← Data Transfer Objects
│   │   └── entity/                    ← Database entities
│   ├── repository/                    ← JDBC database operations
│   │   ├── BillRepository.java
│   │   ├── MenuItemRepository.java
│   │   ├── OrderItemRepository.java
│   │   ├── OrderRepository.java
│   │   ├── PaymentRepository.java
│   │   └── UserRepository.java
│   ├── service/                       ← Business logic
│   │   ├── BillService.java
│   │   ├── MenuItemService.java
│   │   ├── OrderService.java
│   │   ├── PaymentService.java
│   │   └── UserService.java
│   ├── servlet/                       ← HTTP request handlers
│   │   ├── AuthServlet.java
│   │   ├── BillServlet.java
│   │   ├── MenuItemServlet.java
│   │   ├── OrderServlet.java
│   │   └── UserServlet.java
│   └── util/
│       ├── AppContextListener.java    ← Tomcat startup listener
│       ├── DatabaseConnection.java    
│       ├── FlywayMigrationRunner.java 
│       └── JwtUtil.java              
├── src/main/resources/
│   ├── db/migration/                  
│   └── log4j2.xml                     
└── pom.xml

front_lab1/                            ← Frontend (React + TypeScript)
├── src/
│   ├── api/
│   │   ├── bills.ts
│   │   ├── client.ts                 
│   │   ├── menu.ts
│   │   └── orders.ts
│   ├── components/
│   │   └── Navbar.tsx
│   ├── pages/
│   │   ├── AdminPage.tsx
│   │   ├── KitchenPage.tsx
│   │   ├── LoginPage.tsx
│   │   ├── MenuPage.tsx
│   │   └── OrdersPage.tsx
│   ├── App.tsx
│   └── main.tsx
└── package.json
```

---
