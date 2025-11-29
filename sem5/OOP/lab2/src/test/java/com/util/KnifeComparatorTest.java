package com.util;

import com.model.*;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for KnifeComparator
 */
class KnifeComparatorTest {

    private List<Knife> knives;

    @BeforeEach
    void setUp() {
        knives = new ArrayList<>();

        // Create test knives
        Knife knife1 = createKnife("K001", KnifeType.SWORD, "Germany", 800, true);
        Knife knife2 = createKnife("K002", KnifeType.DAGGER, "Japan", 200, false);
        Knife knife3 = createKnife("K003", KnifeType.KNIFE, "Ukraine", 150, true);
        Knife knife4 = createKnife("K004", KnifeType.DAGGER, "Brazil", 250, false);

        knives.add(knife1);
        knives.add(knife2);
        knives.add(knife3);
        knives.add(knife4);
    }

    @Test
    void testSortByType() {
        knives.sort(KnifeComparator.BY_TYPE);

        assertEquals(KnifeType.DAGGER, knives.get(0).getType());
        assertEquals(KnifeType.DAGGER, knives.get(1).getType());
        assertEquals(KnifeType.KNIFE, knives.get(2).getType());
        assertEquals(KnifeType.SWORD, knives.get(3).getType());
    }

    @Test
    void testSortByOrigin() {
        knives.sort(KnifeComparator.BY_ORIGIN);

        assertEquals("Brazil", knives.get(0).getOrigin());
        assertEquals("Germany", knives.get(1).getOrigin());
        assertEquals("Japan", knives.get(2).getOrigin());
        assertEquals("Ukraine", knives.get(3).getOrigin());
    }

    @Test
    void testSortByBladeLength() {
        knives.sort(KnifeComparator.BY_BLADE_LENGTH);

        assertEquals(150, knives.get(0).getVisual().getBlade().getLength());
        assertEquals(200, knives.get(1).getVisual().getBlade().getLength());
        assertEquals(250, knives.get(2).getVisual().getBlade().getLength());
        assertEquals(800, knives.get(3).getVisual().getBlade().getLength());
    }

    @Test
    void testSortByCollectible() {
        knives.sort(KnifeComparator.BY_COLLECTIBLE);

        assertFalse(knives.get(0).getValue().isCollectible());
        assertFalse(knives.get(1).getValue().isCollectible());
        assertTrue(knives.get(2).getValue().isCollectible());
        assertTrue(knives.get(3).getValue().isCollectible());
    }

    @Test
    void testSortByValueAndLength() {
        knives.sort(KnifeComparator.BY_VALUE_AND_LENGTH);

        // Non-collectibles first, sorted by length
        assertFalse(knives.get(0).getValue().isCollectible());
        assertFalse(knives.get(1).getValue().isCollectible());

        // Collectibles next, sorted by length
        assertTrue(knives.get(2).getValue().isCollectible());
        assertEquals(150, knives.get(2).getVisual().getBlade().getLength());
        assertTrue(knives.get(3).getValue().isCollectible());
        assertEquals(800, knives.get(3).getVisual().getBlade().getLength());
    }

    @Test
    void testSortByTypeAndOrigin() {
        knives.sort(KnifeComparator.BY_TYPE_AND_ORIGIN);

        // First two should be DAGGERs sorted by origin
        assertEquals(KnifeType.DAGGER, knives.get(0).getType());
        assertEquals("Brazil", knives.get(0).getOrigin());
        assertEquals(KnifeType.DAGGER, knives.get(1).getType());
        assertEquals("Japan", knives.get(1).getOrigin());
    }

    private Knife createKnife(String id, KnifeType type, String origin,
            int bladeLength, boolean collectible) {
        Knife knife = new Knife();
        knife.setId(id);
        knife.setType(type);
        knife.setHandy(HandyType.ONE_HANDED);
        knife.setOrigin(origin);

        Blade blade = new Blade(bladeLength, 30);
        Material material = new Material(BladeMaterial.STEEL);
        Handle handle = new Handle(HandleMaterial.WOOD, WoodType.OAK);
        Visual visual = new Visual(blade, material, handle, false);
        knife.setVisual(visual);

        Value value = new Value(collectible);
        knife.setValue(value);

        return knife;
    }
}