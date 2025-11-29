package com.util;

import org.xml.sax.SAXException;

import javax.xml.XMLConstants;
import javax.xml.transform.stream.StreamSource;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;
import java.io.File;
import java.io.IOException;

/**
 * Utility class for validating XML documents against XSD schema
 */
public class XMLValidator {

    public static boolean validate(String xmlFilePath, String xsdFilePath) {
        try {
            SchemaFactory factory = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
            Schema schema = factory.newSchema(new File(xsdFilePath));
            Validator validator = schema.newValidator();
            validator.validate(new StreamSource(new File(xmlFilePath)));
            return true;
        } catch (SAXException | IOException e) {
            System.err.println("Validation error: " + e.getMessage());
            return false;
        }
    }
}