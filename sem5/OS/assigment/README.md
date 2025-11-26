# Розподілена система обчислень 



## Варіант завдання
**Комунікація:** через локальний сокет

**Мова:** Java  

**Підхід до обробки:** Мультиплексування

<img width="487" height="339" alt="image" src="https://github.com/user-attachments/assets/15285ce0-84ea-4124-aed0-f9e0034e2815" />


## Математична постановка

Користувач вводить ціле число x. Система паралельно обчислює:
- **Worker F:** f(x) = x^2
- **Worker G:** g(x) = 15/x
- **Manager:** Сумує результат Sum = f(x) + g(x)

---

## Архітектура системи

### Компоненти

**Manager (Manager.java)**
- Центральне управління розподілених обчислень
- Запускає Workers як окремі процеси через ProcessBuilder
- Управляє життєвим циклом дочірніх процесів
- Забезпечує інтерактивну взаємодію з користувачем

**Worker F (WorkerF.java)**
- Обчислює f(x) = x^2
- Піднімає ServerSocket на порту 5001
- Симулює зависання при x = 5 (затримка 20 секунд)
- Завжди повертає результат без помилок

**Worker G (WorkerG.java)**
- Обчислює g(x) = 15/x
- Піднімає ServerSocket на порту 5002
- Обробляє виняток ArithmeticException при x = 0
- Відправляє повідомлення "ERROR: Division by zero" у разі помилки
- Підтримує симуляцію зависання (закоментовано для x = 7)

### Стек

**Міжпроцесна комунікація:**
- TCP Sockets на localhost
- Неблокуючі канали (SocketChannel)
- Асинхронна передача даних

**Конкурентність:**
- Java NIO Selector для мультиплексування в/в операцій
- Один потік Manager обслуговує обидва Workers одночасно

---


### Алгоритм виконання

**1. Ініціалізація**
```
Manager → ProcessBuilder → Worker F (порт 5001)
Manager → ProcessBuilder → Worker G (порт 5002)
```
- Запуск дочірніх процесів з параметрами: порт, значення x
- Затримка 500ms для ініціалізації Workers
- Workers піднімають ServerSocket та очікують підключення

**2. Встановлення з'єднання**
```
Manager → SocketChannel.open() → Worker F
Manager → SocketChannel.open() → Worker G
```
- Метод connectToWorker() з 5 спробами та затримкою 200ms
- Реєстрація каналів у Selector з OP_READ та attachment ("F"/"G")

**3. Розподіл завдань**
- Аргумент x передається як параметр командного рядка при запуску
- Workers отримують значення через args[1]

**4. Event Loop (Очікування відповідей)**
```java
while (результати не отримано) {
    selector.select(100); 
    обробляємо ready channels
    перевірити timeout (5 секунд)
}
```

**5. Обробка відповідей**
- Workers відправляють результат як текстовий рядок
- Формат: "число" або "ERROR: повідомлення"
- Manager парсить відповідь та зберігає в resultF/resultG

**6. Агрегація**
```
Sum = f(x) + g(x)
```
Вивід результату або звіту про помилку

---

## Види ERROR/Fail

### Hard Fail

**Випадок:** x = 0

**Послідовність подій:**
1. Manager запускає Worker G з x = 0
2. Worker G виявляє ділення на нуль у блоці обчислень
3. Worker G відправляє: `"ERROR: Division by zero"`
4. Manager отримує повідомлення через Selector
5. processWorkerResponse() встановлює errorG = true
6. Worker G коректно закриває з'єднання
7. Manager виводить звіт про помилку та припиняє обчислення

**Вивід:**
```
=== ERROR REPORT ===
Worker G error: Division by zero
Computation impossible due to worker error.
```

### Soft Fail (Зависання)

**Сценарій:** Worker не відповідає протягом 5 секунд

**Механізм виявлення:**
```java
long elapsed = System.currentTimeMillis() - startTime;
if (elapsed > TIMEOUT_MS && !waitingForInput) {
    handleTimeout();
}
```

**Інтерактивне меню:**
```
=== TIMEOUT DETECTED ===

--- Status ---
Worker F: Done (25)
Worker G: Running...

Options:
1. Continue for 10 more seconds
2. Continue indefinitely
3. Show status
4. Cancel
Choose (1-4):
```

**Опції користувача:**

**1. Continue for 10 more seconds**
- Скидаємо startTime = System.currentTimeMillis()
- Додаємо 10 секунд до таймауту
- Повертаємось до Event Loop

**2. Continue indefinitely**
- Фактично вимикає таймаут
- Очікуємо до отримання відповіді або Cancel

**3. Show status**
- Виводить поточний стан Workers

**4. Cancel**
- Встановлює errorF/errorG = true з повідомленням "Cancelled by user"
- Виходить з Event Loop
- Викликає cleanup() для знищення процесів

**Захист:**
- Прапорець waitingForInput блокує повторне відкриття меню
- Відповіді від Workers обробляються, але не псують консоль
- Після виходу з меню продовжується нормальна обробка

---

## Технічні деталі реалізації

### Мультиплексування в/в

**Java NIO Selector:**
```java
Selector selector = Selector.open();
channelF.register(selector, SelectionKey.OP_READ, "F");
channelG.register(selector, SelectionKey.OP_READ, "G");

while (true) {
    int ready = selector.select(100);  // Timeout 100ms
    Iterator<SelectionKey> keys = selector.selectedKeys().iterator();
    // обробляємо ready channels
}
```

### Неблокуючі з'єднання

**Підключення з retry:**
```java
private SocketChannel connectToWorker(int port) {
    for (int i = 0; i < 5; i++) {
        SocketChannel channel = SocketChannel.open();
        channel.configureBlocking(false);
        channel.connect(new InetSocketAddress("localhost", port));
        while (!channel.finishConnect()) {
            Thread.sleep(10);
        }
        return channel;
    }
    return null;
}
```

- 5 спроб підключення
- Затримка 200ms між спробами
- Якщо всі спроби невдалі, тоді errorF/errorG = true

### Управління процесами

**Створення Workers:**
```java
ProcessBuilder pbF = new ProcessBuilder(
    "java", "WorkerF", String.valueOf(PORT_F), String.valueOf(x)
);
pbF.redirectErrorStream(true);
workerF = pbF.start();
```

**Коректне завершення:**
```java
private void cleanup() {
    channelF.close();
    channelG.close();
    selector.close();
    workerF.destroyForcibly();  // Гарантоване знищення
    workerG.destroyForcibly();
}
```

**Формати відповідей:**
- Успіх: `"25.0"` 
- Помилка: `"ERROR: Division by zero"`

---


### Вивід у консолі

**Нормальне обчислення:**
```
Enter integer x: 3
Starting computation for x = 3

[Worker F completed: 9.0]
[Worker G completed: 5.0]

=== RESULT ===
f(x) = x^2 = 9.0
g(x) = 15/x = 5.0
Sum = 14.0
```

**Критична помилка:**
```
Enter integer x: 0
Starting computation for x = 0

[Manager received error from G: ERROR: Division by zero]

=== ERROR REPORT ===
Worker G error: Division by zero
Computation impossible due to worker error.
```

**Таймаут:**
```
Enter integer x: 5
Starting computation for x = 5

=== TIMEOUT DETECTED ===

--- Status ---
Worker F: Running...
Worker G: Done (3.0)

Options:
1. Continue for 10 more seconds
2. Continue indefinitely
3. Show status
4. Cancel
Choose (1-4): 1

[Worker F completed: 25.0]

=== RESULT ===
f(x) = x^2 = 25.0
g(x) = 15/x = 3.0
Sum = 28.0
```

---

## Тестування

### Тестові сценарії

* х=5 - WorkerF робить таймаут(спеціально для тестування додаткового меню)

**Worker F (x = 5):**

Активоване в коді:
```java
if (x == 5) {
    System.out.println("Worker F simulating hang...");
    Thread.sleep(20000);
}
```

* х=7 - WorkerG робить таймаут(спеціально для тестування додаткового меню)

**Worker G (x = 7):**

Закомічено:
```java
if (x == 7) {
    System.out.println("Worker G simulating hang...");
    Thread.sleep(20000);
}
```

## Відповідність технічному завданню

### Реалізовані вимоги

- ProcessBuilder для створення Workers
- **TCP Socket комунікація:** localhost, порти 5001/5002
- **Мультиплексування:** Java NIO Selector, один потік
- **Паралельне обчислення:** f(x) та g(x) незалежно
- **Hard Fail обробка:** Ділення на нуль, повідомлення про помилку
- **Soft Fail обробка:** Таймаут 5 секунд, інтерактивне меню
- **Меню з 4 опціями:** Continue (2 варіанти), Status, Cancel
- **Захист консолі:** waitingForInput flag
- **Симуляція зависання:** Workers підтримують тестові затримки

---

