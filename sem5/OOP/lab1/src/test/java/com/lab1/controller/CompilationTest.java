package com.lab1.controller;

import com.lab1.model.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

@DisplayName("Compilation Controller Tests")
class CompilationTest {

    private Compilation compilation;
    private Song song1;
    private Song song2;
    private Instrumental instrumental;

    @BeforeEach
    void setUp() {
        compilation = new Compilation("Test Compilation");
        song1 = new Song("Song A", "Artist 1", 180, "Rock", 2020);
        song2 = new Song("Song B", "Artist 2", 240, "Pop", 2021);
        instrumental = new Instrumental("Inst", "Artist 3", 300, "Jazz", 2019, "Piano");
    }

    @Test
    @DisplayName("Should create compilation with title")
    void testCompilationCreation() {
        assertEquals("Test Compilation", compilation.getTitle());
        assertTrue(compilation.getPlaylist().isEmpty());
    }

    @Test
    @DisplayName("Should add composition to playlist")
    void testAddComposition() {
        compilation.addComposition(song1);
        assertEquals(1, compilation.getPlaylist().size());
        assertTrue(compilation.getPlaylist().contains(song1));
    }

    @Test
    @DisplayName("Should throw exception when adding null composition")
    void testAddNullComposition() {
        assertThrows(IllegalArgumentException.class, () -> compilation.addComposition(null));
    }

    @Test
    @DisplayName("Should remove composition from playlist")
    void testRemoveComposition() {
        compilation.addComposition(song1);
        compilation.addComposition(song2);

        compilation.removeComposition(song1);

        assertEquals(1, compilation.getPlaylist().size());
        assertFalse(compilation.getPlaylist().contains(song1));
        assertTrue(compilation.getPlaylist().contains(song2));
    }

    @Test
    @DisplayName("Should calculate total duration correctly")
    void testCalculateTotalDuration() {
        compilation.addComposition(song1); // 180 sec
        compilation.addComposition(song2); // 240 sec
        compilation.addComposition(instrumental); // 300 sec

        assertEquals(720, compilation.calculateTotalDuration());
    }

    @Test
    @DisplayName("Should return zero duration for empty playlist")
    void testCalculateTotalDurationEmpty() {
        assertEquals(0, compilation.calculateTotalDuration());
    }

    @Test
    @DisplayName("Should sort compositions by genre then by title")
    void testSortByGenre() {
        Song jazzSong = new Song("A Song", "Artist", 100, "Jazz", 2020);
        Song popSong = new Song("B Song", "Artist", 100, "Pop", 2020);
        Song rockSong = new Song("C Song", "Artist", 100, "Rock", 2020);

        compilation.addComposition(rockSong);
        compilation.addComposition(jazzSong);
        compilation.addComposition(popSong);

        compilation.sortByGenre();
        List<MusicComposition> sorted = compilation.getPlaylist();

        assertEquals("Jazz", sorted.get(0).getGenre());
        assertEquals("Pop", sorted.get(1).getGenre());
        assertEquals("Rock", sorted.get(2).getGenre());
    }

    @Test
    @DisplayName("Should find compositions in duration range")
    void testFindByDuration() {
        compilation.addComposition(song1); // 180 sec
        compilation.addComposition(song2); // 240 sec
        compilation.addComposition(instrumental); // 300 sec

        List<MusicComposition> found = compilation.findByDuration(200, 250);

        assertEquals(1, found.size());
        assertEquals(song2, found.get(0));
    }

    @Test
    @DisplayName("Should return empty list when no compositions in range")
    void testFindByDurationNoResults() {
        compilation.addComposition(song1); // 180 sec

        List<MusicComposition> found = compilation.findByDuration(300, 400);

        assertTrue(found.isEmpty());
    }

    @Test
    @DisplayName("Should throw exception for invalid duration range")
    void testFindByDurationInvalidRange() {
        assertThrows(IllegalArgumentException.class, () -> compilation.findByDuration(-10, 100));
        assertThrows(IllegalArgumentException.class, () -> compilation.findByDuration(200, 100));
    }

    @Test
    @DisplayName("Should return defensive copy of playlist")
    void testGetPlaylistDefensiveCopy() {
        compilation.addComposition(song1);
        List<MusicComposition> playlist = compilation.getPlaylist();

        playlist.clear();

        assertEquals(1, compilation.getPlaylist().size());
    }

    @Test
    @DisplayName("Should handle multiple compositions with same genre")
    void testSortByGenreSameGenre() {
        Song song1 = new Song("B Title", "Artist", 100, "Rock", 2020);
        Song song2 = new Song("A Title", "Artist", 100, "Rock", 2020);

        compilation.addComposition(song1);
        compilation.addComposition(song2);
        compilation.sortByGenre();

        List<MusicComposition> sorted = compilation.getPlaylist();
        assertEquals("A Title", sorted.get(0).getTitle());
        assertEquals("B Title", sorted.get(1).getTitle());
    }
}