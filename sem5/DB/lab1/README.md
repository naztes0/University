# Report
## Предметна область: Система управління нерухомістю (16)

1) Розробити схему бази даних з вказанням зв’язків між об’єктами, полями, ключами й тд.
(https://app.diagrams.net/ https://dbdiagram.io/home , тощо)
> Було використано dbdiagram.io

2) Реалізовані сутності:
```
buyer
contract_status
document_type
documents
maintenance_status
maintenance_worker
owner
payment
payment_method
property
property_type
rental_agreement
role
sale_agreement
tenant
users
```
3) Сутності повинні спиратися на сформовані вимоги. Хоча б деякі сутності мають
підтримувати “Soft delete”, деякі сутності мають зберігати
дату останньої зміни даних та користувача що їх змінив.
> Soft Delete: Сутності property та documents ```is_deleted bool```
> 
> Зберігання інформації змін: Сутності users, owner, property, sale_agreement ``` updated_at, updated_by```

4) СУБД
> PostgreSQL

5) Створення й використання збережених процедур, користувацьких функцій, тригерів, розрізів даних (View)
* **Функції та Процедури:** soft_delete_property (для логічного видалення),restore_property, set_timestamp (база для системи аудиту)...
* **Тригери (11):** Реалізовано 7 аудит-тригерів (для оновлення updated_at) та 4 тригери для забезпечення workflow: trigger_soft_delete_property м'яко видаляє об'єкт property після закриття угоди sale_agreement.
* **Розрізи Даних (3 Views):** Створені для роботи в патерні Repository, зокрема active_properties_view (для вибірки лише не видаленої нерухомості).

6) Створення індексів
>B-tree індекси: для прискорення стандартного пошуку за ідентифікаторами та ключовими текстовими полями (idx_owner_last_name, таблиця owner)
>
>GIN індекс (Generalized Inverted Index): Використовується для забезпечення ефективного повнотекстового пошуку (idx_property_address_gin, таблиця property)

7) Реалізувати комбінацію патернів Repository + Unit of Work для декількох логічно-зв'язаних сутностей, де усі запити будуть йти через використання збережених процедур та розрізів даних(для отримання даних).

Реалізовано комбінацію патернів Repository та Unit of Work (UoW) на базі Node.js:
> Unit of Work (unitOfWork.js): Керує єдиною транзакцією для операції закриття угоди продажу.
> 
> Repository: Забезпечує доступ до сутностей (PropertyRepository, SaleAgreementRepository).

Запити на читання (SELECT) йдуть через розрізи даних (active_properties_view, sale_agreement_details_view).

Запити INSERT/UPDATE ініціюються кодом, але все виконується на стороні БД через тригери (оновлення статусу sale_agreement -> автоматичне м'яке видалення property через trigger_soft_delete_property)
