package com.util;

import com.model.*;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.File;
import java.util.List;

/**
 * Utility class for transforming knife collection to different XML formats
 */
public class XMLTransformer {

    /**
     * Transforms knife collection to a simplified HTML table format
     * 
     * @param knives         List of knives to transform
     * @param outputFilePath Output file path
     * @throws Exception if transformation fails
     */
    public static void transformToHTML(List<Knife> knives, String outputFilePath) throws Exception {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        Document doc = builder.newDocument();

        // Create HTML structure
        Element html = doc.createElement("html");
        doc.appendChild(html);

        Element head = doc.createElement("head");
        html.appendChild(head);

        Element title = doc.createElement("title");
        title.setTextContent("Knife Collection");
        head.appendChild(title);

        Element style = doc.createElement("style");
        style.setTextContent(
                "table { border-collapse: collapse; width: 100%; }" +
                        "th, td { border: 1px solid black; padding: 8px; text-align: left; }" +
                        "th { background-color: #f2f2f2; }");
        head.appendChild(style);

        Element body = doc.createElement("body");
        html.appendChild(body);

        Element h1 = doc.createElement("h1");
        h1.setTextContent("Knife Collection");
        body.appendChild(h1);

        Element table = doc.createElement("table");
        body.appendChild(table);

        // Create table header
        Element thead = doc.createElement("thead");
        table.appendChild(thead);
        Element headerRow = doc.createElement("tr");
        thead.appendChild(headerRow);

        String[] headers = { "ID", "Type", "Handy", "Origin", "Blade Length (mm)",
                "Blade Width (mm)", "Material", "Collectible" };
        for (String header : headers) {
            Element th = doc.createElement("th");
            th.setTextContent(header);
            headerRow.appendChild(th);
        }

        // Create table body
        Element tbody = doc.createElement("tbody");
        table.appendChild(tbody);

        for (Knife knife : knives) {
            Element row = doc.createElement("tr");
            tbody.appendChild(row);

            addCell(doc, row, knife.getId());
            addCell(doc, row, knife.getType().name());
            addCell(doc, row, knife.getHandy().name());
            addCell(doc, row, knife.getOrigin());

            if (knife.getVisual() != null) {
                Visual visual = knife.getVisual();
                addCell(doc, row, String.valueOf(visual.getBlade().getLength()));
                addCell(doc, row, String.valueOf(visual.getBlade().getWidth()));
                addCell(doc, row, visual.getMaterial().getBlade().name());
            } else {
                addCell(doc, row, "N/A");
                addCell(doc, row, "N/A");
                addCell(doc, row, "N/A");
            }

            addCell(doc, row, knife.getValue().isCollectible() ? "Yes" : "No");
        }

        // Write to file
        writeDocument(doc, outputFilePath);
    }

    public static void transformToSummary(List<Knife> knives, String outputFilePath) throws Exception {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        Document doc = builder.newDocument();

        Element root = doc.createElement("knifeSummary");
        doc.appendChild(root);

        Element totalCount = doc.createElement("totalCount");
        totalCount.setTextContent(String.valueOf(knives.size()));
        root.appendChild(totalCount);

        Element collectibleCount = doc.createElement("collectibleCount");
        long collectibles = knives.stream().filter(k -> k.getValue().isCollectible()).count();
        collectibleCount.setTextContent(String.valueOf(collectibles));
        root.appendChild(collectibleCount);

        Element knivesElement = doc.createElement("knives");
        root.appendChild(knivesElement);

        for (Knife knife : knives) {
            Element knifeElement = doc.createElement("knife");
            knifeElement.setAttribute("id", knife.getId());
            knivesElement.appendChild(knifeElement);

            Element type = doc.createElement("type");
            type.setTextContent(knife.getType().name());
            knifeElement.appendChild(type);

            Element origin = doc.createElement("origin");
            origin.setTextContent(knife.getOrigin());
            knifeElement.appendChild(origin);

            Element collectible = doc.createElement("collectible");
            collectible.setTextContent(String.valueOf(knife.getValue().isCollectible()));
            knifeElement.appendChild(collectible);
        }

        writeDocument(doc, outputFilePath);
    }

    private static void addCell(Document doc, Element row, String text) {
        Element td = doc.createElement("td");
        td.setTextContent(text);
        row.appendChild(td);
    }

    private static void writeDocument(Document doc, String outputFilePath) throws Exception {
        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        Transformer transformer = transformerFactory.newTransformer();
        transformer.setOutputProperty(OutputKeys.INDENT, "yes");
        transformer.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "2");

        DOMSource source = new DOMSource(doc);
        StreamResult result = new StreamResult(new File(outputFilePath));
        transformer.transform(source, result);
    }
}