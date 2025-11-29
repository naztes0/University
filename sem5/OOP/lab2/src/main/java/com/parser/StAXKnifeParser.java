package com.parser;

import com.model.*;

import javax.xml.stream.XMLInputFactory;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamReader;
import java.io.FileInputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * StAX parser for knife XML documents
 */
public class StAXKnifeParser {

    public List<Knife> parse(String xmlFilePath) throws Exception {
        List<Knife> knives = new ArrayList<>();
        XMLInputFactory factory = XMLInputFactory.newInstance();

        try (FileInputStream fis = new FileInputStream(xmlFilePath)) {
            XMLStreamReader reader = factory.createXMLStreamReader(fis);

            Knife currentKnife = null;
            Visual currentVisual = null;
            Blade currentBlade = null;
            Material currentMaterial = null;
            Handle currentHandle = null;
            Value currentValue = null;

            String currentElement = null;
            boolean inBlade = false;
            boolean inMaterial = false;
            boolean inHandle = false;

            while (reader.hasNext()) {
                int event = reader.next();

                switch (event) {
                    case XMLStreamConstants.START_ELEMENT:
                        currentElement = reader.getLocalName();

                        switch (currentElement) {
                            case "knife":
                                currentKnife = new Knife();
                                String id = reader.getAttributeValue(null, "id");
                                currentKnife.setId(id);
                                break;
                            case "visual":
                                currentVisual = new Visual();
                                break;
                            case "blade":
                                if (currentVisual != null && !inMaterial) {
                                    currentBlade = new Blade();
                                    inBlade = true;
                                }
                                break;
                            case "material":
                                if (currentVisual != null) {
                                    currentMaterial = new Material();
                                    inMaterial = true;
                                }
                                break;
                            case "handle":
                                currentHandle = new Handle();
                                inHandle = true;
                                break;
                            case "value":
                                currentValue = new Value();
                                String collectible = reader.getAttributeValue(null, "collectible");
                                if (collectible != null) {
                                    currentValue.setCollectible(Boolean.parseBoolean(collectible));
                                }
                                break;
                        }
                        break;

                    case XMLStreamConstants.CHARACTERS:
                        String text = reader.getText().trim();
                        if (!text.isEmpty() && currentElement != null) {
                            processCharacters(text, currentElement, currentKnife, currentVisual,
                                    currentBlade, currentMaterial, currentHandle,
                                    inBlade, inMaterial, inHandle);
                        }
                        break;

                    case XMLStreamConstants.END_ELEMENT:
                        String endElement = reader.getLocalName();

                        switch (endElement) {
                            case "knife":
                                knives.add(currentKnife);
                                currentKnife = null;
                                break;
                            case "visual":
                                if (currentKnife != null && currentVisual != null) {
                                    currentKnife.setVisual(currentVisual);
                                }
                                currentVisual = null;
                                break;
                            case "blade":
                                if (inBlade && currentBlade != null) {
                                    currentVisual.setBlade(currentBlade);
                                    currentBlade = null;
                                    inBlade = false;
                                }
                                break;
                            case "material":
                                if (inMaterial && currentMaterial != null) {
                                    currentVisual.setMaterial(currentMaterial);
                                    currentMaterial = null;
                                    inMaterial = false;
                                }
                                break;
                            case "handle":
                                if (currentHandle != null) {
                                    currentVisual.setHandle(currentHandle);
                                    currentHandle = null;
                                    inHandle = false;
                                }
                                break;
                            case "value":
                                if (currentKnife != null && currentValue != null) {
                                    currentKnife.setValue(currentValue);
                                    currentValue = null;
                                }
                                break;
                        }
                        break;
                }
            }

            reader.close();
        }

        return knives;
    }

    private void processCharacters(String text, String element, Knife knife, Visual visual,
            Blade blade, Material material, Handle handle,
            boolean inBlade, boolean inMaterial, boolean inHandle) {
        switch (element) {
            case "type":
                if (knife != null)
                    knife.setType(KnifeType.valueOf(text));
                break;
            case "handy":
                if (knife != null)
                    knife.setHandy(HandyType.valueOf(text));
                break;
            case "origin":
                if (knife != null)
                    knife.setOrigin(text);
                break;
            case "length":
                if (blade != null && inBlade)
                    blade.setLength(Integer.parseInt(text));
                break;
            case "width":
                if (blade != null && inBlade)
                    blade.setWidth(Integer.parseInt(text));
                break;
            case "blade":
                if (material != null && inMaterial) {
                    material.setBlade(BladeMaterial.valueOf(text));
                }
                break;
            case "material":
                if (handle != null && inHandle) {
                    handle.setMaterial(HandleMaterial.valueOf(text));
                }
                break;
            case "woodType":
                if (handle != null)
                    handle.setWoodType(WoodType.valueOf(text));
                break;
            case "bloodGroove":
                if (visual != null)
                    visual.setBloodGroove(Boolean.parseBoolean(text));
                break;
        }
    }
}