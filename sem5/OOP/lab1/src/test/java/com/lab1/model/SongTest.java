package com.lab1.model;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.NullAndEmptySource;
import org.junit.jupiter.params.provider.ValueSource;

import static org.junit.jupiter.api.Assertions.*;

@DisplayName("Song Tests")
class SongTest {

    @Test
    @DisplayName("Should create valid song")
    void testValidSongCreation() {
        Song song = new Song("Bohemian Rhapsody", "Queen", 354, "Rock", 1975);

        assertEquals("Bohemian Rhapsody", song.getTitle());
        assertEquals("Queen", song.getBand());
        assertEquals(354, song.getDurationSec());
        assertEquals("Rock", song.getGenre());
        assertEquals(1975, song.getYearReleased());
        assertEquals("Song", song.getType());
    }

    @Test
    @DisplayName("Should return correct artist (band)")
    void testGetArtistReturnsBand() {
        Song song = new Song("Test Song", "Test Band", 200, "Pop", 2020);
        assertEquals("Test Band", song.getArtist());
        assertEquals("Test Band", song.getBand());
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t", "\n" })
    @DisplayName("Should throw exception for invalid title")
    void testInvalidTitle(String title) {
        assertThrows(IllegalArgumentException.class, () -> new Song(title, "Queen", 354, "Rock", 1975));
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t", "\n" })
    @DisplayName("Should throw exception for invalid band")
    void testInvalidBand(String band) {
        assertThrows(IllegalArgumentException.class, () -> new Song("Title", band, 354, "Rock", 1975));
    }

    @ParameterizedTest
    @ValueSource(ints = { 0, -1, -100 })
    @DisplayName("Should throw exception for invalid duration")
    void testInvalidDuration(int duration) {
        assertThrows(IllegalArgumentException.class, () -> new Song("Title", "Band", duration, "Rock", 1975));
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t" })
    @DisplayName("Should throw exception for invalid genre")
    void testInvalidGenre(String genre) {
        assertThrows(IllegalArgumentException.class, () -> new Song("Title", "Band", 200, genre, 1975));
    }

    @ParameterizedTest
    @ValueSource(ints = { 999, 2101, 500, 3000 })
    @DisplayName("Should throw exception for invalid year")
    void testInvalidYear(int year) {
        assertThrows(IllegalArgumentException.class, () -> new Song("Title", "Band", 200, "Rock", year));
    }

    @Test
    @DisplayName("Should format toString correctly")
    void testToString() {
        Song song = new Song("Test", "Artist", 180, "Pop", 2020);
        String expected = "Test - Artist (Pop, 2020, 180 sec)";
        assertEquals(expected, song.toString());
    }

    @Test
    @DisplayName("Should handle boundary year values")
    void testBoundaryYears() {
        assertDoesNotThrow(() -> new Song("Title", "Band", 200, "Rock", 1000));
        assertDoesNotThrow(() -> new Song("Title", "Band", 200, "Rock", 2100));
    }
}