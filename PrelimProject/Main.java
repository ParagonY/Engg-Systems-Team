package PrelimProject;

import java.io.IOException;
import java.io.PrintWriter;

public class Main extends Thread {
    public static void main(String[] args) throws IOException {
        Sim messageConnection = new Sim();
        messageConnection.start();

        MyFirstWindow.setSimInstance(messageConnection);
        MyFirstWindow.buildGUI();

        int test = 0;
        while (true) {
            if (messageConnection.greeting != null) {
                System.out.println(messageConnection.greeting);
            }
            if (messageConnection.connected == true) {
                PrintWriter message = new PrintWriter(messageConnection.clientSocket.getOutputStream(), true);
                message.println("TEST MESSAGE TO ESP");
            }
            test++;

            //try { Thread.sleep(1000); } catch (InterruptedException e) {}
        }
    }
}
