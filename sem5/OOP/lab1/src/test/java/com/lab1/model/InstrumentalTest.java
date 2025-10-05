package com.lab1.model;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.NullAndEmptySource;
import org.junit.jupiter.params.provider.ValueSource;

import static org.junit.jupiter.api.Assertions.*;

@DisplayName("Instrumental Tests")
class InstrumentalTest {

    @Test
    @DisplayName("Should create valid instrumental")
    void testValidInstrumentalCreation() {
        Instrumental inst = new Instrumental("Eruption", "Van Halen", 102, "Rock", 1978, "Guitar");

        assertEquals("Eruption", inst.getTitle());
        assertEquals("Van Halen", inst.getArtist());
        assertEquals(102, inst.getDurationSec());
        assertEquals("Rock", inst.getGenre());
        assertEquals(1978, inst.getYearReleased());
        assertEquals("Guitar", inst.getInstrument());
        assertEquals("Instrumental", inst.getType());
    }

    @ParameterizedTest
    @NullAndEmptySource
    @ValueSource(strings = { "  ", "\t", "\n" })
    @DisplayName("Should throw exception for invalid instrument")
    void testInvalidInstrument(String instrument) {
        assertThrows(IllegalArgumentException.class,
                () -> new Instrumental("Title", "Artist", 200, "Jazz", 2000, instrument));
    }

    @Test
    @DisplayName("Should format toString correctly")
    void testToString() {
        Instrumental inst = new Instrumental("Night Owl", "ODESZA", 260, "Electronica", 2017, "Synthesizer");
        String expected = "Night Owl - ODESZA [Synthesizer] (Electronica, 2017, 260 sec)";
        assertEquals(expected, inst.toString());
    }

    @Test
    @DisplayName("Should inherit validation from MusicComposition")
    void testInheritedValidation() {
        assertThrows(IllegalArgumentException.class,
                () -> new Instrumental(null, "Artist", 200, "Jazz", 2000, "Piano"));

        assertThrows(IllegalArgumentException.class, () -> new Instrumental("Title", "", 200, "Jazz", 2000, "Piano"));

        assertThrows(IllegalArgumentException.class,
                () -> new Instrumental("Title", "Artist", -5, "Jazz", 2000, "Piano"));

        assertThrows(IllegalArgumentException.class, () -> new Instrumental("Title", "Artist", 200, "", 2000, "Piano"));

        assertThrows(IllegalArgumentException.class,
                () -> new Instrumental("Title", "Artist", 200, "Jazz", 999, "Piano"));
    }

    @Test
    @DisplayName("Should handle various instruments")
    void testVariousInstruments() {
        assertDoesNotThrow(() -> new Instrumental("T", "A", 100, "G", 2000, "Piano"));
        assertDoesNotThrow(() -> new Instrumental("T", "A", 100, "G", 2000, "Guitar"));
        assertDoesNotThrow(() -> new Instrumental("T", "A", 100, "G", 2000, "Drums"));
        assertDoesNotThrow(() -> new Instrumental("T", "A", 100, "G", 2000, "Synthesizer"));
    }
}