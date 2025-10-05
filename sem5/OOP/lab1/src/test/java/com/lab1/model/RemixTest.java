package com.lab1.model;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.NullAndEmptySource;
import org.junit.jupiter.params.provider.ValueSource;

import static org.junit.jupiter.api.Assertions.*;

@DisplayName("Remix Tests")
class RemixTest {

    @Test
    @DisplayName("Should create valid remix")
    void testValidRemixCreation() {
        Remix remix = new Remix("Smells Like Teen Spirit", "Nirvana", "DJ Shadow", 289, "Grunge", 2021);

        assertEquals("Smells Like Teen Spirit", remix.getTitle());
        assertEquals("Nirvana", remix.getOriginalArtist());
        assertEquals("DJ Shadow", remix.getRemixer());
        assertEquals("DJ Shadow", remix.getArtist());
        assertEquals(289, remix.getDurationSec());
        assertEquals("Grunge", remix.getGenre());
        assertEquals(2021, remix.getYearReleased());
        assertEquals("Remix", remix.getType());
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t", "\n" })
    @DisplayName("Should throw exception for invalid original artist")
    void testInvalidOriginalArtist(String originalArtist) {
        assertThrows(IllegalArgumentException.class,
                () -> new Remix("Title", originalArtist, "Remixer", 200, "Pop", 2020));
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t", "\n" })
    @DisplayName("Should throw exception for invalid remixer")
    void testInvalidRemixer(String remixer) {
        assertThrows(IllegalArgumentException.class, () -> new Remix("Title", "Original", remixer, 200, "Pop", 2020));
    }

    @Test
    @DisplayName("Should format toString correctly")
    void testToString() {
        Remix remix = new Remix("Cold Little Heart", "Michael Kiwanuka", "Kaytranada", 225, "Chillhop", 2020);
        String expected = "Cold Little Heart - Michael Kiwanuka (remix by Kaytranada) (Chillhop, 2020, 225 sec)";
        assertEquals(expected, remix.toString());
    }

    @Test
    @DisplayName("Artist should return remixer name")
    void testArtistReturnsRemixer() {
        Remix remix = new Remix("Title", "Original Artist", "Cool Remixer", 200, "EDM", 2020);
        assertEquals("Cool Remixer", remix.getArtist());
        assertEquals("Cool Remixer", remix.getRemixer());
    }

    @Test
    @DisplayName("Should handle same original artist and remixer names")
    void testSameArtistAndRemixer() {
        assertDoesNotThrow(() -> new Remix("Self Remix", "Artist", "Artist", 200, "Electronic", 2020));
    }

    @Test
    @DisplayName("Should inherit validation from MusicComposition")
    void testInheritedValidation() {
        assertThrows(IllegalArgumentException.class, () -> new Remix("", "Original", "Remixer", 200, "Pop", 2020));

        assertThrows(IllegalArgumentException.class, () -> new Remix("Title", "Original", "Remixer", 0, "Pop", 2020));

        assertThrows(IllegalArgumentException.class, () -> new Remix("Title", "Original", "Remixer", 200, null, 2020));

        assertThrows(IllegalArgumentException.class, () -> new Remix("Title", "Original", "Remixer", 200, "Pop", 2150));
    }
}