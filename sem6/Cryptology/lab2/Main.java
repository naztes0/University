
import java.nio.charset.StandardCharsets;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        String myPassword = "secretKey";
        byte[] key = myPassword.getBytes(StandardCharsets.UTF_8);

        System.out.printf("\n Enter data for encryption: ");
        Scanner scanner = new Scanner(System.in);
        String myText = scanner.nextLine();
        byte[] plaintext = myText.getBytes(StandardCharsets.UTF_8);

        System.out.println("\n=== TEST RC6 ===");
        System.out.println("Orig data: " + new String(plaintext));

        RC6Algo rc6 = new RC6Algo(key);

        byte[] encrypted = rc6.encrypt(plaintext);

        System.out.print("Encrypted hex: ");
        for (byte b : encrypted) {
            System.out.printf("%02x", b);
        }
        System.out.println();

        byte[] decrypted = rc6.decrypt(encrypted);

        System.out.println("Decrypted text: " + new String(decrypted));
        System.out.println("");
        scanner.close();
    }
}