
CREATE TYPE user_role AS ENUM ('CLIENT', 'ADMIN', 'KITCHEN');
CREATE TYPE order_status AS ENUM ('NEW', 'CONFIRMED', 'IN_KITCHEN', 'READY', 'BILLED', 'PAID');
CREATE TYPE bill_status AS ENUM ('UNPAID', 'PAID');

CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    auth0_id VARCHAR(100) UNIQUE NOT NULL,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    role user_role NOT NULL DEFAULT 'CLIENT',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE menu_items (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    price DECIMAL(10,2) NOT NULL,
    category VARCHAR(50),
    available BOOLEAN DEFAULT TRUE
);

CREATE TABLE orders (
    id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(id),
    table_number INTEGER NOT NULL,
    status order_status NOT NULL DEFAULT 'NEW',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE order_items (
    id SERIAL PRIMARY KEY,
    order_id INTEGER REFERENCES orders(id),
    menu_item_id INTEGER REFERENCES menu_items(id),
    quantity INTEGER NOT NULL,
    price_at_order DECIMAL(10,2) NOT NULL
);

CREATE TABLE bills (
    id SERIAL PRIMARY KEY,
    order_id INTEGER REFERENCES orders(id) UNIQUE,
    total_amount DECIMAL(10,2) NOT NULL,
    status bill_status NOT NULL DEFAULT 'UNPAID',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE payments (
    id SERIAL PRIMARY KEY,
    bill_id INTEGER REFERENCES bills(id),
    amount DECIMAL(10,2) NOT NULL,
    paid_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);