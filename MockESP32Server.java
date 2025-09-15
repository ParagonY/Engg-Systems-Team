import java.io.*;
import java.net.*;

public class MockESP32Server {
    public static void main(String[] args) {
        int port = 3333;

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            System.out.println("Mock ESP32 Server running on port " + port);

            while (true) {
                Socket clientSocket = serverSocket.accept();
                System.out.println("Client connected.");

                BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);

                String command = in.readLine();
                System.out.println("Received from client: " + command);

                // Simulate handling commands
                switch (command) {
                    case "LIFT":
                        System.out.println("Simulating bridge lift.");
                        break;
                    case "DOWN":
                        System.out.println("Simulating bridge lowering.");
                        break;
                    default:
                        System.out.println("Unknown command.");
                        break;
                }

                out.println("ACK: " + command); // Simulate ESP32 reply
                clientSocket.close();
                System.out.println("Client disconnected.\n");
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
