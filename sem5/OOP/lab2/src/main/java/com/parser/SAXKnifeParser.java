package com.parser;

import com.model.*;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * SAX parser for knife XML documents
 */
public class SAXKnifeParser {

    public List<Knife> parse(String xmlFilePath) throws Exception {
        SAXParserFactory factory = SAXParserFactory.newInstance();
        factory.setNamespaceAware(true);
        SAXParser saxParser = factory.newSAXParser();

        KnifeHandler handler = new KnifeHandler();
        saxParser.parse(new File(xmlFilePath), handler);

        return handler.getKnives();
    }

    /**
     * SAX Handler for processing knife elements
     */
    private static class KnifeHandler extends DefaultHandler {
        private List<Knife> knives = new ArrayList<>();
        private Knife currentKnife;
        private Visual currentVisual;
        private Blade currentBlade;
        private Material currentMaterial;
        private Handle currentHandle;
        private Value currentValue;

        private StringBuilder elementValue = new StringBuilder();
        private String currentElement;

        // Context flags to track where we are in the XML structure
        private boolean inVisualBlade = false;
        private boolean inMaterialBlade = false;
        private boolean inHandleMaterial = false;

        @Override
        public void startElement(String uri, String localName, String qName,
                Attributes attributes) throws SAXException {
            elementValue.setLength(0);
            currentElement = localName;

            switch (localName) {
                case "knife":
                    currentKnife = new Knife();
                    currentKnife.setId(attributes.getValue("id"));
                    break;
                case "visual":
                    currentVisual = new Visual();
                    break;
                case "blade":
                    // Check if we're inside visual but not inside material
                    if (currentVisual != null && currentMaterial == null) {
                        currentBlade = new Blade();
                        inVisualBlade = true;
                    } else if (currentMaterial != null) {
                        // We're inside material element
                        inMaterialBlade = true;
                    }
                    break;
                case "material":
                    // Check if we're inside visual but not inside handle
                    if (currentVisual != null && currentHandle == null) {
                        currentMaterial = new Material();
                    } else if (currentHandle != null) {
                        // We're inside handle element
                        inHandleMaterial = true;
                    }
                    break;
                case "handle":
                    currentHandle = new Handle();
                    break;
                case "value":
                    currentValue = new Value();
                    String collectible = attributes.getValue("collectible");
                    if (collectible != null) {
                        currentValue.setCollectible(Boolean.parseBoolean(collectible));
                    }
                    break;
            }
        }

        @Override
        public void characters(char[] ch, int start, int length) throws SAXException {
            elementValue.append(ch, start, length);
        }

        @Override
        public void endElement(String uri, String localName, String qName) throws SAXException {
            String value = elementValue.toString().trim();

            switch (localName) {
                case "knife":
                    knives.add(currentKnife);
                    currentKnife = null;
                    break;
                case "type":
                    if (!value.isEmpty()) {
                        currentKnife.setType(KnifeType.valueOf(value));
                    }
                    break;
                case "handy":
                    if (!value.isEmpty()) {
                        currentKnife.setHandy(HandyType.valueOf(value));
                    }
                    break;
                case "origin":
                    currentKnife.setOrigin(value);
                    break;
                case "visual":
                    currentKnife.setVisual(currentVisual);
                    currentVisual = null;
                    break;
                case "length":
                    if (currentBlade != null && inVisualBlade && !value.isEmpty()) {
                        currentBlade.setLength(Integer.parseInt(value));
                    }
                    break;
                case "width":
                    if (currentBlade != null && inVisualBlade && !value.isEmpty()) {
                        currentBlade.setWidth(Integer.parseInt(value));
                    }
                    break;
                case "blade":
                    if (inVisualBlade && currentBlade != null) {
                        // Ending the blade element inside visual
                        currentVisual.setBlade(currentBlade);
                        currentBlade = null;
                        inVisualBlade = false;
                    } else if (inMaterialBlade && !value.isEmpty()) {
                        // Ending the blade element inside material
                        currentMaterial.setBlade(BladeMaterial.valueOf(value));
                        inMaterialBlade = false;
                    }
                    break;
                case "material":
                    if (currentMaterial != null && !inHandleMaterial) {
                        // Ending the material element inside visual
                        currentVisual.setMaterial(currentMaterial);
                        currentMaterial = null;
                    } else if (inHandleMaterial && !value.isEmpty()) {
                        // Ending the material element inside handle
                        currentHandle.setMaterial(HandleMaterial.valueOf(value));
                        inHandleMaterial = false;
                    }
                    break;
                case "woodType":
                    if (currentHandle != null && !value.isEmpty()) {
                        currentHandle.setWoodType(WoodType.valueOf(value));
                    }
                    break;
                case "handle":
                    if (currentVisual != null && currentHandle != null) {
                        currentVisual.setHandle(currentHandle);
                    }
                    currentHandle = null;
                    break;
                case "bloodGroove":
                    if (currentVisual != null && !value.isEmpty()) {
                        currentVisual.setBloodGroove(Boolean.parseBoolean(value));
                    }
                    break;
                case "value":
                    currentKnife.setValue(currentValue);
                    currentValue = null;
                    break;
            }
        }

        public List<Knife> getKnives() {
            return knives;
        }
    }
}