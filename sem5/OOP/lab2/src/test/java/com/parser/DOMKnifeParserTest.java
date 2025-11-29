package com.parser;

import com.model.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for DOMKnifeParser
 */
class DOMKnifeParserTest {

    private DOMKnifeParser parser;
    private static final String XML_FILE = "src/main/java/com/resources/knives.xml";

    @BeforeEach
    void setUp() {
        parser = new DOMKnifeParser();
    }

    @Test
    void testParseKnivesCount() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);
        assertEquals(5, knives.size(), "Should parse 5 knives");
    }

    @Test
    void testParseKnifeAttributes() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        Knife knife = knives.get(1); // K002
        assertEquals("K002", knife.getId());
        assertEquals(KnifeType.SABER, knife.getType());
        assertEquals(HandyType.ONE_HANDED, knife.getHandy());
        assertEquals("Japan", knife.getOrigin());
    }

    @Test
    void testParseBladeDimensions() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        Knife machete = knives.stream()
                .filter(k -> "K005".equals(k.getId()))
                .findFirst()
                .orElseThrow();

        Blade blade = machete.getVisual().getBlade();
        assertEquals(450, blade.getLength());
        assertEquals(50, blade.getWidth());
    }

    @Test
    void testParseDifferentMaterials() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        // K001 - Damascus steel
        assertEquals(BladeMaterial.DAMASCUS_STEEL,
                knives.get(0).getVisual().getMaterial().getBlade());

        // K005 - Cast iron
        Knife machete = knives.stream()
                .filter(k -> "K005".equals(k.getId()))
                .findFirst()
                .orElseThrow();
        assertEquals(BladeMaterial.CAST_IRON,
                machete.getVisual().getMaterial().getBlade());
    }

    @Test
    void testParseHandleMaterials() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        // K004 - Plastic handle
        Knife simpleKnife = knives.stream()
                .filter(k -> "K004".equals(k.getId()))
                .findFirst()
                .orElseThrow();

        Handle handle = simpleKnife.getVisual().getHandle();
        assertEquals(HandleMaterial.PLASTIC, handle.getMaterial());
        assertNull(handle.getWoodType());
    }

    @Test
    void testParseBloodGroove() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        // K001 has blood groove
        assertTrue(knives.get(0).getVisual().isBloodGroove());

        // K002 doesn't have blood groove
        assertFalse(knives.get(1).getVisual().isBloodGroove());
    }

    @Test
    void testParseInvalidFile() {
        assertThrows(Exception.class, () -> parser.parse("nonexistent.xml"));
    }
}