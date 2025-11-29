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
                    if (currentVisual != null) {
                        currentBlade = new Blade();
                    }
                    break;
                case "material":
                    if (currentVisual != null) {
                        currentMaterial = new Material();
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
                    currentKnife.setType(KnifeType.valueOf(value));
                    break;
                case "handy":
                    currentKnife.setHandy(HandyType.valueOf(value));
                    break;
                case "origin":
                    currentKnife.setOrigin(value);
                    break;
                case "visual":
                    currentKnife.setVisual(currentVisual);
                    currentVisual = null;
                    break;
                case "length":
                    if (currentBlade != null) {
                        currentBlade.setLength(Integer.parseInt(value));
                    }
                    break;
                case "width":
                    if (currentBlade != null) {
                        currentBlade.setWidth(Integer.parseInt(value));
                    }
                    break;
                case "blade":
                    if (currentBlade != null) {
                        currentVisual.setBlade(currentBlade);
                        currentBlade = null;
                    } else if (currentMaterial != null && !value.isEmpty()) {
                        currentMaterial.setBlade(BladeMaterial.valueOf(value));
                    }
                    break;
                case "material":
                    if (currentMaterial != null) {
                        currentVisual.setMaterial(currentMaterial);
                        currentMaterial = null;
                    } else if (currentHandle != null && !value.isEmpty()) {
                        currentHandle.setMaterial(HandleMaterial.valueOf(value));
                    }
                    break;
                case "woodType":
                    if (currentHandle != null && !value.isEmpty()) {
                        currentHandle.setWoodType(WoodType.valueOf(value));
                    }
                    break;
                case "handle":
                    currentVisual.setHandle(currentHandle);
                    currentHandle = null;
                    break;
                case "bloodGroove":
                    currentVisual.setBloodGroove(Boolean.parseBoolean(value));
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