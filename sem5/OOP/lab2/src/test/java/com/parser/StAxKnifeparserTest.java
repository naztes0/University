package com.parser;

import com.model.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for StAXKnifeParser
 */
class StAXKnifeParserTest {

    private StAXKnifeParser parser;
    private static final String XML_FILE = "src/main/java/com/resources/knives.xml";

    @BeforeEach
    void setUp() {
        parser = new StAXKnifeParser();
    }

    @Test
    void testParseKnivesCount() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);
        assertEquals(5, knives.size(), "Should parse 5 knives");
    }

    @Test
    void testParseAllKnifeTypes() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        assertTrue(knives.stream().anyMatch(k -> k.getType() == KnifeType.DAGGER));
        assertTrue(knives.stream().anyMatch(k -> k.getType() == KnifeType.SABER));
        assertTrue(knives.stream().anyMatch(k -> k.getType() == KnifeType.SWORD));
        assertTrue(knives.stream().anyMatch(k -> k.getType() == KnifeType.KNIFE));
        assertTrue(knives.stream().anyMatch(k -> k.getType() == KnifeType.MACHETE));
    }

    @Test
    void testParseTwoHandedSword() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        Knife sword = knives.stream()
                .filter(k -> k.getType() == KnifeType.SWORD)
                .findFirst()
                .orElseThrow();

        assertEquals(HandyType.TWO_HANDED, sword.getHandy());
        assertEquals(900, sword.getVisual().getBlade().getLength());
        assertEquals(80, sword.getVisual().getBlade().getWidth());
    }

    @Test
    void testParseOrigins() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        assertTrue(knives.stream().anyMatch(k -> "Germany".equals(k.getOrigin())));
        assertTrue(knives.stream().anyMatch(k -> "Japan".equals(k.getOrigin())));
        assertTrue(knives.stream().anyMatch(k -> "Ukraine".equals(k.getOrigin())));
        assertTrue(knives.stream().anyMatch(k -> "Brazil".equals(k.getOrigin())));
    }

    @Test
    void testParseCollectibleValues() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        long collectibleCount = knives.stream()
                .filter(k -> k.getValue().isCollectible())
                .count();

        assertEquals(3, collectibleCount, "Should have 3 collectible knives");
    }

    @Test
    void testParseInvalidFile() {
        assertThrows(Exception.class, () -> parser.parse("nonexistent.xml"));
    }
}