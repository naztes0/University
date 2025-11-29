package com.parser;

import com.model.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for SAXKnifeParser
 */
class SAXKnifeParserTest {

    private SAXKnifeParser parser;
    private static final String XML_FILE = "src/main/java/com/resources/knives.xml";

    @BeforeEach
    void setUp() {
        parser = new SAXKnifeParser();
    }

    @Test
    void testParseKnivesCount() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);
        assertEquals(5, knives.size(), "Should parse 5 knives");
    }

    @Test
    void testParseFirstKnife() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        Knife firstKnife = knives.get(0);
        assertEquals("K001", firstKnife.getId());
        assertEquals(KnifeType.DAGGER, firstKnife.getType());
        assertEquals(HandyType.ONE_HANDED, firstKnife.getHandy());
        assertEquals("Germany", firstKnife.getOrigin());
        assertTrue(firstKnife.getValue().isCollectible());
    }

    @Test
    void testParseKnifeVisual() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        Visual visual = knives.get(0).getVisual();
        assertNotNull(visual, "Visual should not be null");

        // Check blade - SAX parser should correctly parse nested blade element
        Blade blade = visual.getBlade();
        assertNotNull(blade, "Blade should not be null");
        assertEquals(250, blade.getLength(), "Blade length should be 250");
        assertEquals(30, blade.getWidth(), "Blade width should be 30");

        // Check material
        Material material = visual.getMaterial();
        assertNotNull(material, "Material should not be null");
        assertEquals(BladeMaterial.DAMASCUS_STEEL, material.getBlade());

        // Check handle
        Handle handle = visual.getHandle();
        assertNotNull(handle, "Handle should not be null");
        assertEquals(HandleMaterial.WOOD, handle.getMaterial());
        assertEquals(WoodType.OAK, handle.getWoodType());

        assertTrue(visual.isBloodGroove());
    }

    @Test
    void testParseKnifeWithMetalHandle() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        // K003 has metal handle without wood type
        Knife sword = knives.stream()
                .filter(k -> "K003".equals(k.getId()))
                .findFirst()
                .orElseThrow(() -> new AssertionError("K003 not found"));

        assertNotNull(sword.getVisual(), "Visual should not be null");
        Handle handle = sword.getVisual().getHandle();
        assertNotNull(handle, "Handle should not be null");
        assertEquals(HandleMaterial.METAL, handle.getMaterial(), "Handle material should be METAL");
        assertNull(handle.getWoodType(), "Wood type should be null for metal handle");
    }

    @Test
    void testParseKnifeWithPlasticHandle() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        // K004 has plastic handle
        Knife simpleKnife = knives.stream()
                .filter(k -> "K004".equals(k.getId()))
                .findFirst()
                .orElseThrow(() -> new AssertionError("K004 not found"));

        Handle handle = simpleKnife.getVisual().getHandle();
        assertNotNull(handle);
        assertEquals(HandleMaterial.PLASTIC, handle.getMaterial());
        assertNull(handle.getWoodType());
    }

    @Test
    void testParseNonCollectibleKnife() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        // K003 is not collectible
        Knife sword = knives.stream()
                .filter(k -> "K003".equals(k.getId()))
                .findFirst()
                .orElseThrow();

        assertFalse(sword.getValue().isCollectible());
    }

    @Test
    void testParseAllKnifeIds() throws Exception {
        List<Knife> knives = parser.parse(XML_FILE);

        assertTrue(knives.stream().anyMatch(k -> "K001".equals(k.getId())));
        assertTrue(knives.stream().anyMatch(k -> "K002".equals(k.getId())));
        assertTrue(knives.stream().anyMatch(k -> "K003".equals(k.getId())));
        assertTrue(knives.stream().anyMatch(k -> "K004".equals(k.getId())));
        assertTrue(knives.stream().anyMatch(k -> "K005".equals(k.getId())));
    }

    @Test
    void testParseInvalidFile() {
        assertThrows(Exception.class, () -> parser.parse("nonexistent.xml"));
    }
}