import java.io.*;
import java.net.*;

public class WorkerF {
    public static void main(String[] args) {
        if (args.length < 2) {
            System.err.println("Usage: java WorkerF <port> <x>");
            System.exit(1);
        }

        int port = Integer.parseInt(args[0]);
        int x = Integer.parseInt(args[1]);

        System.out.println("Worker F started on port " + port + " with x=" + x);

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            // 30 second timeout
            serverSocket.setSoTimeout(30000);

            System.out.println("Worker F waiting for Manager connection");
            Socket socket = serverSocket.accept();
            System.out.println("Worker F connected to Manager");

            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

            // Simulate hanging
            if (x == 5) {
                System.out.println("Worker F simulating hang...");
                Thread.sleep(20000);
            }

            // Calculate f(x) = x^2
            double result = (double) x * x;
            System.out.println("Worker F computed f(" + x + ") = " + result);

            // Send result to Manager
            out.println(result);
            System.out.println("Worker F sent result to Manager");

            Thread.sleep(1000); // Give time for message to be sent

        } catch (SocketTimeoutException e) {
            System.err.println("Worker F: Timeout waiting for Manager connection");
        } catch (Exception e) {
            System.err.println("Worker F error: " + e.getMessage());
            e.printStackTrace();
        }

        System.out.println("Worker F shutting down");
    }
}