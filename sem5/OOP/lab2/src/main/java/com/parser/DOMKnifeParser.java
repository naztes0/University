package com.parser;

import com.model.*;
import org.w3c.dom.*;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * DOM parser for knife XML documents
 */
public class DOMKnifeParser {

    public List<Knife> parse(String xmlFilePath) throws Exception {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setNamespaceAware(true);
        DocumentBuilder builder = factory.newDocumentBuilder();
        Document document = builder.parse(new File(xmlFilePath));
        document.getDocumentElement().normalize();

        List<Knife> knives = new ArrayList<>();
        NodeList knifeNodes = document.getElementsByTagNameNS("*", "knife");

        for (int i = 0; i < knifeNodes.getLength(); i++) {
            Element knifeElement = (Element) knifeNodes.item(i);
            knives.add(parseKnife(knifeElement));
        }

        return knives;
    }

    private Knife parseKnife(Element knifeElement) {
        Knife knife = new Knife();

        // Parse ID attribute
        knife.setId(knifeElement.getAttribute("id"));

        // Parse type
        String type = getElementTextContent(knifeElement, "type");
        knife.setType(KnifeType.valueOf(type));

        // Parse handy
        String handy = getElementTextContent(knifeElement, "handy");
        knife.setHandy(HandyType.valueOf(handy));

        // Parse origin
        knife.setOrigin(getElementTextContent(knifeElement, "origin"));

        // Parse visual
        Element visualElement = (Element) knifeElement.getElementsByTagNameNS("*", "visual").item(0);
        knife.setVisual(parseVisual(visualElement));

        // Parse value
        Element valueElement = (Element) knifeElement.getElementsByTagNameNS("*", "value").item(0);
        knife.setValue(parseValue(valueElement));

        return knife;
    }

    private Visual parseVisual(Element visualElement) {
        Visual visual = new Visual();

        // Parse blade
        Element bladeElement = (Element) visualElement.getElementsByTagNameNS("*", "blade").item(0);
        visual.setBlade(parseBlade(bladeElement));

        // Parse material
        Element materialElement = (Element) visualElement.getElementsByTagNameNS("*", "material").item(0);
        visual.setMaterial(parseMaterial(materialElement));

        // Parse handle
        Element handleElement = (Element) visualElement.getElementsByTagNameNS("*", "handle").item(0);
        visual.setHandle(parseHandle(handleElement));

        // Parse blood groove
        String bloodGroove = getElementTextContent(visualElement, "bloodGroove");
        visual.setBloodGroove(Boolean.parseBoolean(bloodGroove));

        return visual;
    }

    private Blade parseBlade(Element bladeElement) {
        Blade blade = new Blade();

        String length = getElementTextContent(bladeElement, "length");
        blade.setLength(Integer.parseInt(length));

        String width = getElementTextContent(bladeElement, "width");
        blade.setWidth(Integer.parseInt(width));

        return blade;
    }

    private Material parseMaterial(Element materialElement) {
        Material material = new Material();

        String bladeMaterial = getElementTextContent(materialElement, "blade");
        material.setBlade(BladeMaterial.valueOf(bladeMaterial));

        return material;
    }

    private Handle parseHandle(Element handleElement) {
        Handle handle = new Handle();

        String materialStr = getElementTextContent(handleElement, "material");
        handle.setMaterial(HandleMaterial.valueOf(materialStr));

        // Wood type is optional
        NodeList woodTypeNodes = handleElement.getElementsByTagNameNS("*", "woodType");
        if (woodTypeNodes.getLength() > 0) {
            String woodType = woodTypeNodes.item(0).getTextContent().trim();
            if (!woodType.isEmpty()) {
                handle.setWoodType(WoodType.valueOf(woodType));
            }
        }

        return handle;
    }

    private Value parseValue(Element valueElement) {
        Value value = new Value();

        String collectible = valueElement.getAttribute("collectible");
        value.setCollectible(Boolean.parseBoolean(collectible));

        return value;
    }

    private String getElementTextContent(Element parent, String tagName) {
        NodeList nodes = parent.getElementsByTagNameNS("*", tagName);
        if (nodes.getLength() > 0) {
            return nodes.item(0).getTextContent().trim();
        }
        return "";
    }
}