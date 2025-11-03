package PrelimProject;

import java.io.*;
import java.net.*;

public class TestClient1 {

    public static void main(String[] args) {
        String serverIP = "127.0.0.1"; // Localhost (same PC)
        int serverPort = 5050;         // Must match Sim.java

        try (Socket socket = new Socket(serverIP, serverPort)) {
            System.out.println("Connected to GUI server at " + serverIP + ":" + serverPort);

            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            // Send initial handshake
            out.println("Hello");
            System.out.println("Sent handshake: Hello");

            // Loop to simulate ESP receiving commands
            String line;
            while ((line = in.readLine()) != null) {
                System.out.println("Received from GUI: " + line);

                // Respond (simulating ESP acknowledging)
                if (line.equalsIgnoreCase("LIFT")) {
                    out.println("Bridge lifting...");
                } else if (line.equalsIgnoreCase("DOWN")) {
                    out.println("Bridge lowering...");
                } else if (line.equalsIgnoreCase("RESET_SYSTEM")) {
                    out.println("System reset acknowledged.");
                } else if (line.equalsIgnoreCase("EMERGENCY_STOP")) {
                    out.println("Emergency stop triggered!");
                } else {
                    out.println("Received command: " + line);
                }
            }

        } catch (IOException e) {
            System.out.println("Connection failed: " + e.getMessage());
        }
    }
}
