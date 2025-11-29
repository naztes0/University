package com;

import com.model.Knife;
import com.parser.DOMKnifeParser;
import com.parser.SAXKnifeParser;
import com.parser.StAXKnifeParser;
import com.util.KnifeComparator;
import com.util.XMLTransformer;
import com.util.XMLValidator;

import java.util.List;

/**
 * Main application class demonstrating XML parsing and processing
 */
public class Main {
    private static final String XML_FILE = "src/main/java/com/resources/knives.xml";
    private static final String XSD_FILE = "src/main/java/com/resources/knives.xsd";
    private static final String HTML_OUTPUT = "knives_output.html";
    private static final String SUMMARY_OUTPUT = "knives_summary.xml";

    public static void main(String[] args) {
        try {
            System.out.println("=== Knife Collection XML Parser ===\n");

            // Step 1: Validate XML against XSD
            System.out.println("1. Validating XML against XSD schema...");
            if (XMLValidator.validate(XML_FILE, XSD_FILE)) {
                System.out.println("✓ Validation successful!\n");
            } else {
                System.out.println("✗ Validation failed!\n");
                return;
            }

            // Step 2: Parse using SAX
            System.out.println("2. Parsing with SAX parser...");
            SAXKnifeParser saxParser = new SAXKnifeParser();
            List<Knife> saxKnives = saxParser.parse(XML_FILE);
            System.out.println("✓ SAX parsed " + saxKnives.size() + " knives\n");

            // Step 3: Parse using DOM
            System.out.println("3. Parsing with DOM parser...");
            DOMKnifeParser domParser = new DOMKnifeParser();
            List<Knife> domKnives = domParser.parse(XML_FILE);
            System.out.println("✓ DOM parsed " + domKnives.size() + " knives\n");

            // Step 4: Parse using StAX
            System.out.println("4. Parsing with StAX parser...");
            StAXKnifeParser staxParser = new StAXKnifeParser();
            List<Knife> staxKnives = staxParser.parse(XML_FILE);
            System.out.println("✓ StAX parsed " + staxKnives.size() + " knives\n");

            // Step 5: Display parsed data
            System.out.println("5. Displaying knife collection (using DOM results):");
            System.out.println("-".repeat(80));
            for (Knife knife : domKnives) {
                System.out.println(knife);
            }
            System.out.println();

            // Step 6: Sort by different criteria
            System.out.println("6. Sorting knives by type:");
            domKnives.sort(KnifeComparator.BY_TYPE);
            System.out.println("-".repeat(80));
            domKnives.forEach(k -> System.out.println(k.getId() + " - " + k.getType()));
            System.out.println();

            System.out.println("7. Sorting knives by origin:");
            domKnives.sort(KnifeComparator.BY_ORIGIN);
            System.out.println("-".repeat(80));
            domKnives.forEach(k -> System.out.println(k.getId() + " - " + k.getOrigin()));
            System.out.println();

            System.out.println("8. Sorting knives by blade length:");
            domKnives.sort(KnifeComparator.BY_BLADE_LENGTH);
            System.out.println("-".repeat(80));
            domKnives.forEach(k -> {
                if (k.getVisual() != null && k.getVisual().getBlade() != null) {
                    System.out.println(k.getId() + " - " + k.getVisual().getBlade().getLength() + " cm");
                } else {
                    System.out.println(k.getId() + " - N/A");
                }
            });
            System.out.println();

            System.out.println("9. Sorting by collectible value and blade length:");
            domKnives.sort(KnifeComparator.BY_VALUE_AND_LENGTH);
            System.out.println("-".repeat(80));
            domKnives.forEach(k -> {
                String collectible = k.getValue().isCollectible() ? "Collectible" : "Regular";
                int length = 0;
                if (k.getVisual() != null && k.getVisual().getBlade() != null) {
                    length = k.getVisual().getBlade().getLength();
                }
                System.out.println(k.getId() + " - " + collectible + " - " + length + " cm");
            });
            System.out.println();

            // Step 7: Transform to HTML
            System.out.println("10. Transforming to HTML table...");
            XMLTransformer.transformToHTML(domKnives, HTML_OUTPUT);
            System.out.println("✓ HTML output saved to: " + HTML_OUTPUT + "\n");

            // Step 8: Transform to summary XML
            System.out.println("11. Transforming to summary XML...");
            XMLTransformer.transformToSummary(domKnives, SUMMARY_OUTPUT);
            System.out.println("✓ Summary XML saved to: " + SUMMARY_OUTPUT + "\n");

            System.out.println("=== All operations completed successfully! ===");

        } catch (Exception e) {
            System.err.println("Error occurred: " + e.getMessage());
            e.printStackTrace();
        }
    }
}