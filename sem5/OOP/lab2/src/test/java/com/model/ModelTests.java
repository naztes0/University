package com.model;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for model classes
 */
class ModelTests {

    @Test
    void testBladeEquality() {
        Blade blade1 = new Blade(250, 30);
        Blade blade2 = new Blade(250, 30);
        Blade blade3 = new Blade(300, 30);

        assertEquals(blade1, blade2);
        assertNotEquals(blade1, blade3);
        assertEquals(blade1.hashCode(), blade2.hashCode());
    }

    @Test
    void testHandleEquality() {
        Handle handle1 = new Handle(HandleMaterial.WOOD, WoodType.OAK);
        Handle handle2 = new Handle(HandleMaterial.WOOD, WoodType.OAK);
        Handle handle3 = new Handle(HandleMaterial.METAL, null);

        assertEquals(handle1, handle2);
        assertNotEquals(handle1, handle3);
    }

    @Test
    void testKnifeEquality() {
        Knife knife1 = createTestKnife("K001", KnifeType.DAGGER, "Germany");
        Knife knife2 = createTestKnife("K001", KnifeType.DAGGER, "Germany");
        Knife knife3 = createTestKnife("K002", KnifeType.SWORD, "Japan");

        assertEquals(knife1, knife2);
        assertNotEquals(knife1, knife3);
    }

    @Test
    void testVisualEquality() {
        Visual visual1 = createTestVisual(250, 30, BladeMaterial.STEEL);
        Visual visual2 = createTestVisual(250, 30, BladeMaterial.STEEL);
        Visual visual3 = createTestVisual(300, 40, BladeMaterial.DAMASCUS_STEEL);

        assertEquals(visual1, visual2);
        assertNotEquals(visual1, visual3);
    }

    @Test
    void testValueEquality() {
        Value value1 = new Value(true);
        Value value2 = new Value(true);
        Value value3 = new Value(false);

        assertEquals(value1, value2);
        assertNotEquals(value1, value3);
    }

    @Test
    void testKnifeToString() {
        Knife knife = createTestKnife("K001", KnifeType.DAGGER, "Germany");
        String str = knife.toString();

        assertTrue(str.contains("K001"));
        assertTrue(str.contains("DAGGER"));
        assertTrue(str.contains("Germany"));
    }

    @Test
    void testBladeToString() {
        Blade blade = new Blade(250, 30);
        String str = blade.toString();

        assertTrue(str.contains("250"));
        assertTrue(str.contains("30"));
    }

    @Test
    void testMaterialEquality() {
        Material mat1 = new Material(BladeMaterial.STEEL);
        Material mat2 = new Material(BladeMaterial.STEEL);
        Material mat3 = new Material(BladeMaterial.DAMASCUS_STEEL);

        assertEquals(mat1, mat2);
        assertNotEquals(mat1, mat3);
    }

    @Test
    void testHandleWithoutWoodType() {
        Handle handle = new Handle(HandleMaterial.METAL, null);

        assertEquals(HandleMaterial.METAL, handle.getMaterial());
        assertNull(handle.getWoodType());
    }

    @Test
    void testKnifeGettersAndSetters() {
        Knife knife = new Knife();
        knife.setId("K999");
        knife.setType(KnifeType.MACHETE);
        knife.setHandy(HandyType.TWO_HANDED);
        knife.setOrigin("Brazil");

        assertEquals("K999", knife.getId());
        assertEquals(KnifeType.MACHETE, knife.getType());
        assertEquals(HandyType.TWO_HANDED, knife.getHandy());
        assertEquals("Brazil", knife.getOrigin());
    }

    private Knife createTestKnife(String id, KnifeType type, String origin) {
        Knife knife = new Knife();
        knife.setId(id);
        knife.setType(type);
        knife.setHandy(HandyType.ONE_HANDED);
        knife.setOrigin(origin);

        Visual visual = createTestVisual(250, 30, BladeMaterial.STEEL);
        knife.setVisual(visual);

        Value value = new Value(true);
        knife.setValue(value);

        return knife;
    }

    private Visual createTestVisual(int length, int width, BladeMaterial bladeMaterial) {
        Blade blade = new Blade(length, width);
        Material material = new Material(bladeMaterial);
        Handle handle = new Handle(HandleMaterial.WOOD, WoodType.OAK);
        return new Visual(blade, material, handle, false);
    }
}