package com.lab1.model;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.NullAndEmptySource;
import org.junit.jupiter.params.provider.ValueSource;

import static org.junit.jupiter.api.Assertions.*;

@DisplayName("MusicComposition Abstract Class Tests")
class MusicCompositionTest {

    // Concrete implementation for testing abstract class
    private static class TestComposition extends MusicComposition {
        public TestComposition(String title, String artist, int durationSec, String genre, int yearReleased) {
            super(title, artist, durationSec, genre, yearReleased);
        }

        @Override
        public String getType() {
            return "Test";
        }
    }

    @Test
    @DisplayName("Should create valid music composition")
    void testValidComposition() {
        MusicComposition comp = new TestComposition("Test Title", "Test Artist", 200, "Test Genre", 2020);

        assertEquals("Test Title", comp.getTitle());
        assertEquals("Test Artist", comp.getArtist());
        assertEquals(200, comp.getDurationSec());
        assertEquals("Test Genre", comp.getGenre());
        assertEquals(2020, comp.getYearReleased());
        assertEquals("Test", comp.getType());
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t", "\n", "   \t\n   " })
    @DisplayName("Should throw exception for invalid title")
    void testInvalidTitle(String title) {
        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class,
                () -> new TestComposition(title, "Artist", 200, "Genre", 2020));
        assertEquals("Title cannot be empty", exception.getMessage());
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t", "\n" })
    @DisplayName("Should throw exception for invalid artist")
    void testInvalidArtist(String artist) {
        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class,
                () -> new TestComposition("Title", artist, 200, "Genre", 2020));
        assertEquals("Artist cannot be empty", exception.getMessage());
    }

    @ParameterizedTest
    @ValueSource(ints = { 0, -1, -10, -100, -1000 })
    @DisplayName("Should throw exception for non-positive duration")
    void testInvalidDuration(int duration) {
        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class,
                () -> new TestComposition("Title", "Artist", duration, "Genre", 2020));
        assertEquals("Duration must be positive", exception.getMessage());
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t" })
    @DisplayName("Should throw exception for invalid genre")
    void testInvalidGenre(String genre) {
        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class,
                () -> new TestComposition("Title", "Artist", 200, genre, 2020));
        assertEquals("Genre cannot be empty", exception.getMessage());
    }

    @ParameterizedTest
    @ValueSource(ints = { 999, 500, 0, -100, 2101, 3000, 10000 })
    @DisplayName("Should throw exception for invalid year")
    void testInvalidYear(int year) {
        IllegalArgumentException exception = assertThrows(IllegalArgumentException.class,
                () -> new TestComposition("Title", "Artist", 200, "Genre", year));
        assertEquals("Invalid year", exception.getMessage());
    }

    @Test
    @DisplayName("Should accept minimum valid year (1000)")
    void testMinimumValidYear() {
        assertDoesNotThrow(() -> new TestComposition("Title", "Artist", 200, "Genre", 1000));
    }

    @Test
    @DisplayName("Should accept maximum valid year (2100)")
    void testMaximumValidYear() {
        assertDoesNotThrow(() -> new TestComposition("Title", "Artist", 200, "Genre", 2100));
    }

    @Test
    @DisplayName("Should reject year below minimum (999)")
    void testBelowMinimumYear() {
        assertThrows(IllegalArgumentException.class, () -> new TestComposition("Title", "Artist", 200, "Genre", 999));
    }

    @Test
    @DisplayName("Should reject year above maximum (2101)")
    void testAboveMaximumYear() {
        assertThrows(IllegalArgumentException.class, () -> new TestComposition("Title", "Artist", 200, "Genre", 2101));
    }

    @Test
    @DisplayName("Should format toString correctly")
    void testToString() {
        MusicComposition comp = new TestComposition("Amazing Song", "Great Artist", 240, "Pop", 2019);
        String expected = "Amazing Song - Great Artist (Pop, 2019, 240 sec)";
        assertEquals(expected, comp.toString());
    }

    @Test
    @DisplayName("Should handle toString with special characters")
    void testToStringWithSpecialCharacters() {
        MusicComposition comp = new TestComposition("Song: Part 1", "Artist & Band", 180, "Rock/Metal", 2020);
        String result = comp.toString();
        assertTrue(result.contains("Song: Part 1"));
        assertTrue(result.contains("Artist & Band"));
        assertTrue(result.contains("Rock/Metal"));
    }

    @Test
    @DisplayName("Should preserve exact values after creation")
    void testValuePreservation() {
        String title = "Exact Title";
        String artist = "Exact Artist";
        int duration = 315;
        String genre = "Exact Genre";
        int year = 2015;

        MusicComposition comp = new TestComposition(title, artist, duration, genre, year);

        assertSame(title, comp.getTitle());
        assertSame(artist, comp.getArtist());
        assertEquals(duration, comp.getDurationSec());
        assertSame(genre, comp.getGenre());
        assertEquals(year, comp.getYearReleased());
    }

    @Test
    @DisplayName("Should handle very long duration")
    void testVeryLongDuration() {
        int veryLong = 10000; // ~2.7 hours
        assertDoesNotThrow(() -> new TestComposition("Long Track", "Artist", veryLong, "Ambient", 2020));
    }

    @Test
    @DisplayName("Should handle minimum valid duration (1 second)")
    void testMinimumDuration() {
        assertDoesNotThrow(() -> new TestComposition("Short Track", "Artist", 1, "Genre", 2020));
    }

    @Test
    @DisplayName("Should trim whitespace from inputs")
    void testWhitespaceTrimming() {
        // Title and artist with leading/trailing spaces should be rejected
        assertThrows(IllegalArgumentException.class, () -> new TestComposition("   ", "Artist", 200, "Genre", 2020));

        assertThrows(IllegalArgumentException.class, () -> new TestComposition("Title", "   ", 200, "Genre", 2020));

        assertThrows(IllegalArgumentException.class, () -> new TestComposition("Title", "Artist", 200, "   ", 2020));
    }

    @Test
    @DisplayName("Should handle Unicode characters in title and artist")
    void testUnicodeCharacters() {
        assertDoesNotThrow(() -> new TestComposition("Пісня", "Артист", 200, "Жанр", 2020));

        MusicComposition comp = new TestComposition("歌曲", "艺术家", 180, "流行", 2021);
        assertEquals("歌曲", comp.getTitle());
        assertEquals("艺术家", comp.getArtist());
    }

    @Test
    @DisplayName("Should be comparable by implementing natural properties")
    void testCompositionProperties() {
        MusicComposition comp1 = new TestComposition("A Song", "Artist", 200, "Rock", 2020);
        MusicComposition comp2 = new TestComposition("B Song", "Artist", 200, "Pop", 2020);
        MusicComposition comp3 = new TestComposition("A Song", "Artist", 300, "Rock", 2020);

        // Same title
        assertEquals(comp1.getTitle(), comp3.getTitle());
        // Different duration
        assertNotEquals(comp1.getDurationSec(), comp3.getDurationSec());
        // Different genre
        assertNotEquals(comp1.getGenre(), comp2.getGenre());
    }
}