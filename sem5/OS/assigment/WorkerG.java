import java.io.*;
import java.net.*;

public class WorkerG {
    public static void main(String[] args) {
        if (args.length < 2) {
            System.err.println("Usage: java WorkerG <port> <x>");
            System.exit(1);
        }

        int port = Integer.parseInt(args[0]);
        int x = Integer.parseInt(args[1]);

        System.out.println("Worker G started on port " + port + " with x=" + x);

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            serverSocket.setSoTimeout(30000); // 30 second timeout

            System.out.println("Worker G waiting for Manager connection...");
            Socket socket = serverSocket.accept();
            System.out.println("Worker G connected to Manager");

            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

            // Simulating hanging
            // Uncomment to test timeout behavior:
            // if (x == 7) {
            // System.out.println("Worker G simulating hang...");
            // Thread.sleep(20000);
            // }

            // Calculate g(x) = 15/x
            if (x == 0) {
                System.err.println("Worker G: Division by zero detected!");
                out.println("ERROR: Division by zero");
                System.out.println("Error sent, closing connection gracefully...");
                return;
            }

            double result = 15.0 / x;
            System.out.println("Worker G computed g(" + x + ") = " + result);

            // Send result to Manager
            out.println(result);
            System.out.println("Worker G sent result to Manager");

            Thread.sleep(1000); // Give time for message to be sent

        } catch (ArithmeticException e) {
            System.err.println("Worker G: Arithmetic error - " + e.getMessage());
        } catch (SocketTimeoutException e) {
            System.err.println("Worker G: Timeout waiting for Manager connection");
        } catch (Exception e) {
            System.err.println("Worker G error: " + e.getMessage());
            e.printStackTrace();
        }

        System.out.println("Worker G shutting down");
    }
}