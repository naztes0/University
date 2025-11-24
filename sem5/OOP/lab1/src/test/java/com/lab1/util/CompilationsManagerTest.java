package com.lab1.util;

import com.lab1.controller.Compilation;
import org.junit.jupiter.api.*;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.io.File;
import java.io.IOException;
import java.util.List;
import java.util.Scanner;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@ExtendWith(MockitoExtension.class)
@DisplayName("Compilations Manager Tests")
class CompilationsManagerTest {

    @Mock
    private Scanner mockScanner;

    private CompilationsManager manager;
    private static final String TEST_FOLDER = AppConstants.COMPILATION_FOLDER;

    @BeforeEach
    void setUp() {
        manager = new CompilationsManager(mockScanner);

        // Створюємо тестову папку
        File folder = new File(TEST_FOLDER);
        if (!folder.exists()) {
            folder.mkdirs();
        }
    }

    @AfterEach
    void tearDown() {
        // Очищаємо тестові файли
        File folder = new File(TEST_FOLDER);
        if (folder.exists()) {
            File[] files = folder.listFiles();
            if (files != null) {
                for (File file : files) {
                    if (file.getName().startsWith("Test")) {
                        file.delete();
                    }
                }
            }
        }
    }

    @Test
    @DisplayName("Should create new compilation")
    void testCreateCompilation() {
        when(mockScanner.nextLine()).thenReturn("Test Compilation");

        Compilation result = manager.createCompilation();

        assertNotNull(result);
        assertEquals("Test Compilation", result.getTitle());
        verify(mockScanner, times(1)).nextLine();
    }

    @Test
    @DisplayName("Should get compilations list")
    void testGetCompilationsList() throws IOException {
        // Створюємо тестові файли
        Compilation comp1 = new Compilation("Test1");
        Compilation comp2 = new Compilation("Test2");
        CompilationFileManager.saveToFile(comp1, TEST_FOLDER + File.separator + "Test1.txt");
        CompilationFileManager.saveToFile(comp2, TEST_FOLDER + File.separator + "Test2.txt");

        List<String> list = manager.getCompilationsList();

        assertNotNull(list);
        assertTrue(list.size() >= 2);
        assertTrue(list.stream().anyMatch(name -> name.contains("Test1")));
        assertTrue(list.stream().anyMatch(name -> name.contains("Test2")));
    }

    @Test
    @DisplayName("Should return null when folder is empty")
    void testGetCompilationsListEmpty() {
        // Очищаємо папку
        File folder = new File(TEST_FOLDER);
        if (folder.exists()) {
            File[] files = folder.listFiles();
            if (files != null) {
                for (File file : files) {
                    file.delete();
                }
            }
        }

        List<String> list = manager.getCompilationsList();

        assertNull(list);
    }

    @Test
    @DisplayName("Should save compilation with correct filename")
    void testSaveCompilation() {
        Compilation comp = new Compilation("TestSave");
        comp.addComposition(new com.lab1.model.Song("Song", "Artist", 100, "Rock", 2020));

        manager.saveCompilation(comp);

        File file = new File(TEST_FOLDER + File.separator + "TestSave.txt");
        assertTrue(file.exists());

        // Cleanup
        file.delete();
    }

    @Test
    @DisplayName("Should not save null compilation")
    void testSaveNullCompilation() {
        assertDoesNotThrow(() -> manager.saveCompilation(null));
    }

    @Test
    @DisplayName("Should select existing compilation")
    void testSelectExistingCompilation() throws IOException {
        // Створюємо тестовий файл
        Compilation testComp = new Compilation("TestSelect");
        testComp.addComposition(new com.lab1.model.Song("Song", "Artist", 100, "Rock", 2020));
        CompilationFileManager.saveToFile(testComp, TEST_FOLDER + File.separator + "TestSelect.txt");

        // Мокуємо введення: користувач вибирає перший файл у списку
        when(mockScanner.hasNextInt()).thenReturn(true);
        when(mockScanner.nextInt()).thenReturn(1);
        when(mockScanner.nextLine()).thenReturn("");

        Compilation result = manager.selectExistingCompilation();

        assertNotNull(result);
    }

    @Test
    @DisplayName("Should return null when selecting invalid compilation number")
    void testSelectInvalidCompilationNumber() throws IOException {
        // Створюємо тестовий файл
        Compilation testComp = new Compilation("Test");
        CompilationFileManager.saveToFile(testComp, TEST_FOLDER + File.separator + "Test.txt");

        when(mockScanner.hasNextInt()).thenReturn(true);
        when(mockScanner.nextInt()).thenReturn(999); // Неіснуючий номер
        when(mockScanner.nextLine()).thenReturn("");

        Compilation result = manager.selectExistingCompilation();

        assertNull(result);
    }

    @Test
    @DisplayName("Should delete compilation when user confirms")
    void testDeleteCompilationConfirmed() throws IOException {
        // Створюємо тестовий файл
        Compilation testComp = new Compilation("TestDelete");
        String filename = "TestDelete.txt";
        CompilationFileManager.saveToFile(testComp, TEST_FOLDER + File.separator + filename);

        File file = new File(TEST_FOLDER + File.separator + filename);
        assertTrue(file.exists());

        // Мокуємо введення: вибір файлу та підтвердження
        when(mockScanner.hasNextInt()).thenReturn(true);
        when(mockScanner.nextInt()).thenReturn(1);
        when(mockScanner.nextLine()).thenReturn("").thenReturn("y");

        manager.deleteCompilation();

        // Файл повинен бути видалений
        assertFalse(file.exists());
    }

    @Test
    @DisplayName("Should not delete compilation when user cancels")
    void testDeleteCompilationCancelled() throws IOException {
        // Створюємо тестовий файл
        Compilation testComp = new Compilation("TestNoDelete");
        String filename = "TestNoDelete.txt";
        CompilationFileManager.saveToFile(testComp, TEST_FOLDER + File.separator + filename);

        File file = new File(TEST_FOLDER + File.separator + filename);
        assertTrue(file.exists());

        // Мокуємо введення: вибір файлу та відміна
        when(mockScanner.hasNextInt()).thenReturn(true);
        when(mockScanner.nextInt()).thenReturn(1);
        when(mockScanner.nextLine()).thenReturn("").thenReturn("n");

        manager.deleteCompilation();

        // Файл повинен залишитися
        assertTrue(file.exists());

        // Cleanup
        file.delete();
    }

    @Test
    @DisplayName("Should handle empty folder when deleting")
    void testDeleteFromEmptyFolder() {
        // Очищаємо папку
        File folder = new File(TEST_FOLDER);
        if (folder.exists()) {
            File[] files = folder.listFiles();
            if (files != null) {
                for (File file : files) {
                    file.delete();
                }
            }
        }

        assertDoesNotThrow(() -> manager.deleteCompilation());
    }
}