package com.util;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import java.io.File;
import java.io.FileWriter;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for XMLValidator
 */
class XMLValidatorTest {

    private static final String VALID_XML = "src/main/java/com/resources/knives.xml";
    private static final String VALID_XSD = "src/main/java/com/resources/knives.xsd";

    @Test
    void testValidateValidXML() {
        boolean result = XMLValidator.validate(VALID_XML, VALID_XSD);
        assertTrue(result, "Valid XML should pass validation");
    }

    @Test
    void testValidateInvalidXML(@TempDir Path tempDir) throws Exception {
        File invalidXml = tempDir.resolve("invalid.xml").toFile();
        try (FileWriter writer = new FileWriter(invalidXml)) {
            writer.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
            writer.write("<knives xmlns=\"http://example.com/knives\">\n");
            writer.write("  <knife id=\"K001\">\n");
            writer.write("    <type>INVALID_TYPE</type>\n");
            writer.write("  </knife>\n");
            writer.write("</knives>");
        }

        boolean result = XMLValidator.validate(invalidXml.getAbsolutePath(), VALID_XSD);
        assertFalse(result, "Invalid XML should fail validation");
    }

    @Test
    void testValidateNonExistentFile() {
        boolean result = XMLValidator.validate("nonexistent.xml", VALID_XSD);
        assertFalse(result, "Non-existent file should fail validation");
    }

    @Test
    void testValidateNonExistentSchema() {
        boolean result = XMLValidator.validate(VALID_XML, "nonexistent.xsd");
        assertFalse(result, "Non-existent schema should fail validation");
    }
}