package com.lab1.util;

import com.lab1.controller.Compilation;
import com.lab1.model.*;
import org.junit.jupiter.api.*;
import java.io.File;
import java.io.IOException;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

@DisplayName("Compilation File Manager Tests")
class CompilationFileManagerTest {

    private static final String TEST_FOLDER = "test_compilations";
    private static final String TEST_FILE = TEST_FOLDER + File.separator + "test.txt";
    private Compilation compilation;

    @BeforeEach
    void setUp() {

        File folder = new File(TEST_FOLDER);
        if (!folder.exists()) {
            folder.mkdirs();
        }
        compilation = new Compilation("Test Compilation");
        compilation.addComposition(new Song("Test Song", "Test Artist", 200, "Rock", 2020));
        compilation.addComposition(new Instrumental("Test Inst", "Artist", 300, "Jazz", 2019, "Piano"));
        compilation.addComposition(new Remix("Test Remix", "Original", "Remixer", 250, "EDM", 2021));
    }

    @AfterEach
    void tearDown() {
        File file = new File(TEST_FILE);
        if (file.exists()) {
            file.delete();
        }

        File folder = new File(TEST_FOLDER);
        if (folder.exists()) {
            folder.delete();
        }
    }

    @Test
    @DisplayName("Should save compilation to file")
    void testSaveToFile() throws IOException {
        CompilationFileManager.saveToFile(compilation, TEST_FILE);

        File file = new File(TEST_FILE);
        assertTrue(file.exists());
        assertTrue(file.length() > 0);
    }

    @Test
    @DisplayName("Should load compilation from file")
    void testLoadFromFile() throws IOException {
        CompilationFileManager.saveToFile(compilation, TEST_FILE);

        Compilation loaded = CompilationFileManager.loadFromFile(TEST_FILE);

        assertNotNull(loaded);
        assertEquals("Test Compilation", loaded.getTitle());
        assertEquals(3, loaded.getPlaylist().size());
    }

    @Test
    @DisplayName("Should preserve all composition data after save and load")
    void testDataIntegrity() throws IOException {
        CompilationFileManager.saveToFile(compilation, TEST_FILE);
        Compilation loaded = CompilationFileManager.loadFromFile(TEST_FILE);

        List<MusicComposition> original = compilation.getPlaylist();
        List<MusicComposition> loadedList = loaded.getPlaylist();

        assertEquals(original.size(), loadedList.size());

        for (int i = 0; i < original.size(); i++) {
            MusicComposition orig = original.get(i);
            MusicComposition load = loadedList.get(i);

            assertEquals(orig.getTitle(), load.getTitle());
            assertEquals(orig.getArtist(), load.getArtist());
            assertEquals(orig.getDurationSec(), load.getDurationSec());
            assertEquals(orig.getGenre(), load.getGenre());
            assertEquals(orig.getYearReleased(), load.getYearReleased());
            assertEquals(orig.getType(), load.getType());
        }
    }

    @Test
    @DisplayName("Should handle Song type correctly")
    void testSongSerialization() throws IOException {
        Compilation comp = new Compilation("Songs Only");
        comp.addComposition(new Song("Song", "Band", 180, "Rock", 2020));

        CompilationFileManager.saveToFile(comp, TEST_FILE);
        Compilation loaded = CompilationFileManager.loadFromFile(TEST_FILE);

        assertEquals(1, loaded.getPlaylist().size());
        assertTrue(loaded.getPlaylist().get(0) instanceof Song);
    }

    @Test
    @DisplayName("Should handle Instrumental type correctly")
    void testInstrumentalSerialization() throws IOException {
        Compilation comp = new Compilation("Instrumentals");
        comp.addComposition(new Instrumental("Inst", "Artist", 300, "Jazz", 2019, "Guitar"));

        CompilationFileManager.saveToFile(comp, TEST_FILE);
        Compilation loaded = CompilationFileManager.loadFromFile(TEST_FILE);

        assertEquals(1, loaded.getPlaylist().size());
        assertTrue(loaded.getPlaylist().get(0) instanceof Instrumental);
        assertEquals("Guitar", ((Instrumental) loaded.getPlaylist().get(0)).getInstrument());
    }

    @Test
    @DisplayName("Should handle Remix type correctly")
    void testRemixSerialization() throws IOException {
        Compilation comp = new Compilation("Remixes");
        comp.addComposition(new Remix("Title", "Original", "Remixer", 250, "EDM", 2021));

        CompilationFileManager.saveToFile(comp, TEST_FILE);
        Compilation loaded = CompilationFileManager.loadFromFile(TEST_FILE);

        assertEquals(1, loaded.getPlaylist().size());
        assertTrue(loaded.getPlaylist().get(0) instanceof Remix);
        assertEquals("Remixer", ((Remix) loaded.getPlaylist().get(0)).getRemixer());
    }

    @Test
    @DisplayName("Should throw exception for invalid file format")
    void testInvalidFileFormat() throws IOException {
        File file = new File(TEST_FILE);
        file.getParentFile().mkdirs();
        java.nio.file.Files.writeString(file.toPath(), "Invalid content");

        assertThrows(IOException.class, () -> CompilationFileManager.loadFromFile(TEST_FILE));
    }

    @Test
    @DisplayName("Should handle empty compilation")
    void testEmptyCompilation() throws IOException {
        Compilation empty = new Compilation("Empty");

        CompilationFileManager.saveToFile(empty, TEST_FILE);
        Compilation loaded = CompilationFileManager.loadFromFile(TEST_FILE);

        assertEquals("Empty", loaded.getTitle());
        assertTrue(loaded.getPlaylist().isEmpty());
    }

    @Test
    @DisplayName("Should add .txt extension if missing")
    void testFileExtensionHandling() throws IOException {
        String fileWithoutExt = TEST_FOLDER + File.separator + "test";

        CompilationFileManager.saveToFile(compilation, fileWithoutExt);

        File file = new File(fileWithoutExt + ".txt");
        assertTrue(file.exists());
        file.delete();
    }

}