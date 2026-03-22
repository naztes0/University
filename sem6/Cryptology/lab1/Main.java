import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("\nSHA-256 generator");
        System.out.println("Enter text for hashing: ");
        String text = scanner.nextLine();
        ShaAlgo el = new ShaAlgo(text);

        System.out.println("Hash: " + el.hashFunc());
        scanner.close();
    }
}
