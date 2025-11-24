# Lab1 - Music Compilation Manager

## 📋 Requirements

Створити консольну програму, що задовольняє наступним вимогам:

1. Використовувати можливості ОOП: класи, наслідування, поліморфізм, інкапсуляція.
2. Кожен клас повинен мати вичерпну сенс назву та інформативний склад.
3. Наслідування слід застосовувати лише тоді, коли це має сенс.
4. Під час кодування мають бути використані угоди про оформлення коду java code convention.
5. Класи мають бути розкладені за пакетами.
6. Консольне меню має бути мінімальним.
7. Для збереження параметрів ініціалізації можна використовувати файли.
8. Весь код повинен бути покритий юніт тестамі Використовувати Junit 4 або 5, Mockito.


**Task:**  Звукозапис. Визначити ієрархію музичних композицій. Записати на диск сбірку пісень . Порахувати тривалість. Провести перестановку композицій диска з урахуванням приналежності до стилю. Знайти композицію, яка відповідає заданому діапазону довжини треків. 

---


### Package Structure

```
src/main/java/com/lab1/
├── Main.java
├── controller/
│   └── Compilation.java
├── model/
│   ├── MusicComposition.java (abstract)
│   ├── Song.java
│   ├── Instrumental.java
│   └── Remix.java
└── util/
    ├── CompilationFileManager.java
    ├── CompilationsManager.java
    └── AppConstants.java

src/test/java/com/lab1/
├── controller/
│   └── CompilationTest.java
├── model/
│   ├── MusicCompositionTest.java
│   ├── SongTest.java
│   ├── InstrumentalTest.java
│   └── RemixTest.java
└── util/
    ├── CompilationFileManagerTest.java
    └── CompilationsManagerTest.java
```

### Class Hierarchy

```
MusicComposition (abstract)
├── Song
├── Instrumental
└── Remix
```

---

### Model Classes

#### `MusicComposition` (abstract)
Base abstract class for all music compositions.

**Fields:**
- `String title`
- `String artist`
- `int durationSec`
- `String genre`
- `int yearReleased`

**Methods:**
- `String getTitle()`
- `String getArtist()`
- `int getDurationSec()`
- `String getGenre()`
- `int getYearReleased()`
- `abstract String getType()`
- `String toString()`

---

#### `Song`
Represents a song with a band.

**Additional Fields:**
- `String band`

**Methods:**
- `String getBand()`
- `String getType()` 
- `String toString()`

---

#### `Instrumental`
Represents an instrumental composition.

**Additional Fields:**
- `String instrument`

**Methods:**
- `String getInstrument()`
- `String getType()`
- `String toString()`

---

#### `Remix`
Represents a remix with the original artist and remixer.

**Additional Fields:**
- `String originalArtist`
- `String remixer`

**Methods:**
- `String getOriginalArtist()`
- `String getRemixer()`
- `String getType()` 
- `String toString()` 

---

### Controller Classes

#### `Compilation`
Manages a collection of music compositions.

**Fields:**
- `String title`
- `List<MusicComposition> playlist`

**Methods:**
- `String getTitle()`
- `List<MusicComposition> getPlaylist()` → returns defensive copy
- `void addComposition(MusicComposition composition)`
- `void removeComposition(MusicComposition composition)`
- `int calculateTotalDuration()` → calculates total duration in seconds
- `void sortByGenre()` → sorts by genre, then by title
- `List<MusicComposition> findByDuration(int minDur, int maxDur)` → finds compositions in range
- `void printPlaylist()` → prints formatted playlist to console

---

### Utility Classes

#### `CompilationFileManager`
Handles saving and loading compilations from files.

**Methods:**
- `static void saveToFile(Compilation compilation, String filename)` → saves to .txt file
- `static Compilation loadFromFile(String filename)` → loads from .txt file
- `private static String serializeComposition(MusicComposition comp)` → converts to string format
- `private static MusicComposition deserializeComposition(String line)` → parses from string

---

#### `CompilationsManager`
Manages multiple compilations (create, select, delete, save).

**Methods:**
- `Compilation createCompilation()` → creates new compilation
- `Compilation selectExistingCompilation()` → loads existing compilation
- `void deleteCompilation()` → deletes compilation file
- `List<String> getCompilationsList()` → returns list of available compilations
- `void saveCompilation(Compilation compilation)` → saves compilation to file

---

#### `AppConstants`
Contains application constants.

**Constants:**
- `String COMPILATION_FOLDER = "compilations_folder"`
- `String FILE_EXTENSION = ".txt"`

---

## 🧪 Testing

### Test Architecture

```
src/test/java/com/lab1/
├── model/
│   ├── MusicCompositionTest.java        (16 tests)
│   ├── SongTest.java                    (8 tests)
│   ├── InstrumentalTest.java            (5 tests)
│   └── RemixTest.java                   (6 tests)
├── controller/
│   └── CompilationTest.java             (12 tests)
└── util/
    ├── CompilationFileManagerTest.java  (9 tests)
    └── CompilationsManagerTest.java     (10 tests)
```


### Build and Run
```bash
mvn exec:java
```

### Run Tests
```bash
mvn test
```

### Generate Coverage Report (JaCoCo)

target/site/jacoco/index.html

```bash
mvn test
```

---

## 📊 Summary

- **7 main classes** implemented with full OOP principles
- **66 unit tests** covering 67% of codebase
- **3 types** of music compositions (Song, Instrumental, Remix)
- **File-based** persistence with custom format
- **Console interface** for user interaction
- **Mockito** used for testing user input scenarios
