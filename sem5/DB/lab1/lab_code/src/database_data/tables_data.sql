
-- clear whole db
TRUNCATE TABLE
    public.users, public.role, public.owner, public.tenant, public.buyer,
    public.property_type, public.property, public.contract_status,
    public.maintenance_status, public.payment_method, public.maintenance_worker,
    public.rental_agreement, public.sale_agreement, public.payment, public.work_assignment,
    public.document_type, public.documents
RESTART IDENTITY CASCADE;


-- role
INSERT INTO public.role (role_name)
VALUES
    ('Agent'),
    ('Manager'),
    ('Maintenance'),
    ('Admin');

-- contract_status 
INSERT INTO public.contract_status (status_name)
VALUES
    ('Draft'),
    ('Pending'),   -- ID ~ 2 (Status before testing)
    ('Signed'),
    ('Closed');    -- ID ~ 4 (Final status, after testing)

-- maintenance_status 
INSERT INTO public.maintenance_status (status_name)
VALUES
    ('New'),
    ('Assigned'),
    ('In Progress'),
    ('Completed'); 

--property_type 
INSERT INTO public.property_type (property_name)
VALUES
    ('Apartment'),
    ('House'),
    ('Commercial');

--  payment_method 
INSERT INTO public.payment_method (method_name)
VALUES
    ('Card'),
    ('Bank Transfer'),
    ('Cash');

-- document_type
INSERT INTO public.document_type (type_name)
VALUES
    ('Sale Contract'),
    ('Rental Contract'),
    ('Passport Copy');



-- users 
-- user_id = 1
INSERT INTO public.users (first_name, last_name, email, phone, role_id)
VALUES
    ('Ivan', 'Kovalenko', 'i.kovalenko@agency.com', '0951112233', (SELECT role_id FROM public.role WHERE role_name = 'Agent')),
    ('Olena', 'Melnyk', 'o.melnyk@agency.com', '0954445566', (SELECT role_id FROM public.role WHERE role_name = 'Manager')),
    ('Mykola', 'Serviceman', 'm.service@agency.com', '0677778899', (SELECT role_id FROM public.role WHERE role_name = 'Maintenance'));

-- owner 
INSERT INTO public.owner (first_name, last_name, email, phone, updated_by)
VALUES
    ('Taras', 'Shevchuk', 'taras.s@gmail.com', '0671230001', 1), -- owner_id 1
    ('Lesya', 'Ukrainka', 'lesya.u@gmail.com', '0931230002', 2);  -- owner_id 2

-- tenant 
INSERT INTO public.tenant (first_name, last_name, email, phone, updated_by)
VALUES
    ('Stepan', 'Bandera', 'stepan.b@ukr.net', '0501230003', 1); -- tenant_id 1

-- buyer
INSERT INTO public.buyer (first_name, last_name, email, phone, updated_by)
VALUES
    ('Bohdan', 'Khmelnitsky', 'bohdan.kh@ukr.net', '0671230004', 2); -- buyer_id 1


-- property
-- property_id = 1 (object for test selling)
INSERT INTO public.property (property_id, address, property_type, property_area, owner_id, sale_price, is_deleted, updated_by)
VALUES
    (1, 'Kyiv, Lypky St, 15, Apt 5', (SELECT property_type_id FROM public.property_type WHERE property_name = 'Apartment'), 65, 1, 150000.00, FALSE, 1),
    (2, 'Lviv, Svobody Ave, 3, House', (SELECT property_type_id FROM public.property_type WHERE property_name = 'House'), 120, 2, 350000.00, FALSE, 2);

-- maintenance_worker
INSERT INTO public.maintenance_worker (user_id, specialization)
VALUES
    ((SELECT user_id FROM public.users WHERE first_name = 'Mykola'), 'Plumbing, Electrician');



--  sale_agreement (sale agreement -> TEST_SALE_ID = 1)
-- creation status'Pending' (ID ~ 2) 
INSERT INTO public.sale_agreement (property_id, buyer_id, sale_price, sale_date, status_id, created_by)
VALUES (
    1, -- TEST_PROPERTY_ID 1
    1, -- buyer_id 1
    150000.00,
    CURRENT_DATE,
    (SELECT status_id FROM public.contract_status WHERE status_name = 'Pending'),
    1  -- created_by Ivan Kovalenko
);

-- rental_agreement
-- rental_id = 1
INSERT INTO public.rental_agreement (property_id, tenant_id, start_date, end_date, rent_price, status_id, created_at, created_by)
VALUES (
    2, -- property_id 2
    1, -- tenant_id 1
    CURRENT_DATE,
    CURRENT_DATE + INTERVAL '1 year',
    800.00,
    (SELECT status_id FROM public.contract_status WHERE status_name = 'Signed'),
    NOW(),
    1  -- created_by Ivan Kovalenko
);

-- payment 
INSERT INTO public.payment (rental_id, sale_id, price, payment_method_id, created_by)
VALUES (
    1, -- rental_id 1
    NULL,
    800.00,
    (SELECT payment_method_id FROM public.payment_method WHERE method_name = 'Bank Transfer'),
    1
);


-- work_assignment ID ~ 2 
INSERT INTO public.work_assignment (user_id, property_id, description, worker_id, status_id, assigned_at)
VALUES (
    2, 
    2,
    'Plumbing leak in kitchen',
    (SELECT worker_id FROM public.maintenance_worker WHERE user_id = 3), -- worker_id
    (SELECT status_id FROM public.maintenance_status WHERE status_name = 'Assigned'),
    NOW()
);