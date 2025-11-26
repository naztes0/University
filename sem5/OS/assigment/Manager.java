import java.io.*;
import java.net.*;
import java.nio.*;
import java.nio.channels.*;
import java.util.*;

public class Manager {
    private static final int PORT_F = 5001;
    private static final int PORT_G = 5002;
    private static final long TIMEOUT_MS = 5000;

    private Process workerF;
    private Process workerG;
    private SocketChannel channelF;
    private SocketChannel channelG;
    private Selector selector;

    private Double resultF = null;
    private Double resultG = null;
    private boolean errorF = false;
    private boolean errorG = false;
    private String errorMessageF = null;
    private String errorMessageG = null;

    private long startTime;
    private boolean waitingForInput = false;
    private BufferedReader consoleReader;

    public static void main(String[] args) {
        Manager manager = new Manager();
        manager.run();
    }

    public void run() {
        consoleReader = new BufferedReader(new InputStreamReader(System.in));

        while (true) {
            System.out.println("\n=== Distributed Computation System ===");
            System.out.print("Enter integer x (or 'quit' to exit): ");

            try {
                String input = consoleReader.readLine();
                if (input == null || input.trim().equalsIgnoreCase("quit")) {
                    System.out.println("Exiting...");
                    break;
                }

                int x = Integer.parseInt(input.trim());
                performComputation(x);

            } catch (NumberFormatException e) {
                System.out.println("Invalid input. Please enter an integer.");
            } catch (Exception e) {
                System.out.println("Error: " + e.getMessage());
                e.printStackTrace();
            } finally {
                cleanup();
            }
        }
    }

    private void performComputation(int x) throws Exception {
        System.out.println("\nStarting computation for x = " + x);

        // Reset states
        resultF = null;
        resultG = null;
        errorF = false;
        errorG = false;
        errorMessageF = null;
        errorMessageG = null;

        startWorkers(x);

        startTime = System.currentTimeMillis();
        boolean completed = waitForResults();

        if (completed && !errorF && !errorG) {
            double sum = resultF + resultG;
            System.out.println("\n=== RESULT ===");
            System.out.println("f(x) = x^2 = " + resultF);
            System.out.println("g(x) = 15/x = " + resultG);
            System.out.println("Sum = " + sum);
        } else if (errorF || errorG) {
            System.out.println("\n=== ERROR REPORT ===");
            if (errorF)
                System.out.println("Worker F error: " + errorMessageF);
            if (errorG)
                System.out.println("Worker G error: " + errorMessageG);
            System.out.println("Computation impossible due to worker error.");
        }
    }

    private void startWorkers(int x) throws Exception {
        // Worker F
        ProcessBuilder pbF = new ProcessBuilder("java", "WorkerF", String.valueOf(PORT_F), String.valueOf(x));
        pbF.redirectErrorStream(true);
        // pbF.inheritIO();
        workerF = pbF.start();

        // Worker G
        ProcessBuilder pbG = new ProcessBuilder("java", "WorkerG", String.valueOf(PORT_G), String.valueOf(x));
        pbG.redirectErrorStream(true);
        // pbG.inheritIO();
        workerG = pbG.start();

        // Time for launching
        Thread.sleep(500);

        // connecting
        selector = Selector.open();

        // trying connect with selector
        channelF = connectToWorker(PORT_F);
        if (channelF != null)
            channelF.register(selector, SelectionKey.OP_READ, "F");
        else {
            errorF = true;
            errorMessageF = "Could not connect to Worker F";
        }

        channelG = connectToWorker(PORT_G);
        if (channelG != null)
            channelG.register(selector, SelectionKey.OP_READ, "G");
        else {
            errorG = true;
            errorMessageG = "Could not connect to Worker G";
        }
    }

    // Additional method for connection (to gaurantee the connect)
    private SocketChannel connectToWorker(int port) {
        for (int i = 0; i < 5; i++) {
            try {
                SocketChannel channel = SocketChannel.open();
                channel.configureBlocking(false);
                channel.connect(new InetSocketAddress("localhost", port));
                while (!channel.finishConnect()) {
                    Thread.sleep(10);
                }
                return channel;
            } catch (Exception e) {
                try {
                    Thread.sleep(200);
                } catch (InterruptedException ie) {
                }
            }
        }
        return null;
    }

    private boolean waitForResults() throws Exception {
        ByteBuffer buffer = ByteBuffer.allocate(1024);

        while (true) {
            // If all data got or errors => output it
            if ((resultF != null || errorF) && (resultG != null || errorG)) {
                return !errorF && !errorG;
            }

            // Timeout
            long elapsed = System.currentTimeMillis() - startTime;
            if (elapsed > TIMEOUT_MS && !waitingForInput) {
                handleTimeout();
                if ((resultF != null || errorF) && (resultG != null || errorG)) {
                    return !errorF && !errorG;
                }
            }

            int ready = selector.select(100);

            if (ready > 0) {
                Iterator<SelectionKey> keys = selector.selectedKeys().iterator();
                while (keys.hasNext()) {
                    SelectionKey key = keys.next();
                    keys.remove();

                    if (!key.isValid())
                        continue;

                    if (key.isReadable()) {
                        SocketChannel channel = (SocketChannel) key.channel();
                        String worker = (String) key.attachment();

                        buffer.clear();
                        int bytesRead = -1;

                        try {
                            bytesRead = channel.read(buffer);
                        } catch (IOException e) {
                            System.out.println("[Debug: Connection reset by " + worker + " (likely shutdown)]");
                            bytesRead = -1;
                        }

                        if (bytesRead > 0) {
                            buffer.flip();
                            String message = new String(buffer.array(), 0, buffer.limit()).trim();
                            processWorkerResponse(worker, message);
                        } else if (bytesRead == -1) {
                            // Chanel closed
                            channel.close();
                            if ("F".equals(worker) && resultF == null && !errorF) {
                                errorF = true;
                                errorMessageF = "Worker F disconnected unexpectedly";
                            }
                            if ("G".equals(worker) && resultG == null && !errorG) {
                                if (!errorG) {
                                    errorG = true;
                                    errorMessageG = "Worker G disconnected unexpectedly";
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    private void processWorkerResponse(String worker, String message) {
        if (message.startsWith("ERROR:")) {
            if ("F".equals(worker)) {
                errorF = true;
                errorMessageF = message.substring(6).trim();
            } else {
                errorG = true;
                errorMessageG = message.substring(6).trim();
            }
            System.out.println("\n[Manager received error from " + worker + ": " + message + "]");
        } else {
            try {
                double value = Double.parseDouble(message);
                if ("F".equals(worker)) {
                    resultF = value;
                    System.out.println("[Worker F completed: " + value + "]");
                } else {
                    resultG = value;
                    System.out.println("[Worker G completed: " + value + "]");
                }
            } catch (NumberFormatException e) {
            }
        }
    }

    private void handleTimeout() throws IOException {
        waitingForInput = true;
        System.out.println("\n=== TIMEOUT DETECTED ===");

        while (true) {
            displayStatus();
            System.out.println("\nOptions:");
            System.out.println("1. Continue for 10 more seconds");
            System.out.println("2. Continue indefinitely");
            System.out.println("3. Show status");
            System.out.println("4. Cancel");
            System.out.print("Choose (1-4): ");

            String choice = consoleReader.readLine();

            if ("1".equals(choice)) {
                System.out.println("Extending timeout...");
                startTime = System.currentTimeMillis();
                waitingForInput = false;
                return;
            } else if ("2".equals(choice)) {
                System.out.println("Continuing indefinitely...");
                startTime = Long.MAX_VALUE - TIMEOUT_MS - 1000;
                waitingForInput = false;
                return;
            } else if ("3".equals(choice)) {
                continue;
            } else if ("4".equals(choice)) {
                System.out.println("Cancelling...");
                if (resultF == null && !errorF) {
                    errorF = true;
                    errorMessageF = "Cancelled by user";
                }
                if (resultG == null && !errorG) {
                    errorG = true;
                    errorMessageG = "Cancelled by user";
                }
                return;
            }
        }
    }

    private void displayStatus() {
        System.out.println("\n--- Status ---");
        System.out.println(
                "Worker F: " + (resultF != null ? "Done (" + resultF + ")" : (errorF ? "Error" : "Running...")));
        System.out.println(
                "Worker G: " + (resultG != null ? "Done (" + resultG + ")" : (errorG ? "Error" : "Running...")));
    }

    private void cleanup() {
        try {
            if (channelF != null)
                channelF.close();
            if (channelG != null)
                channelG.close();
            if (selector != null)
                selector.close();
            if (workerF != null)
                workerF.destroyForcibly();
            if (workerG != null)
                workerG.destroyForcibly();
        } catch (Exception e) {
        }
    }
}