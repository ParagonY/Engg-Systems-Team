package PrelimProject;

import java.net.*;
import java.io.*;

public class Sim extends Thread{
    private ServerSocket serverSocket;
    private Socket clientSocket;
    private PrintWriter out;
    public BufferedReader in;
    
    Sim(){
    }

    public void run(){
        try {
            startListener(5050);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }



public void startListener(int port)  throws IOException {
    System.out.println("works here");
        serverSocket = new ServerSocket(port);
        
        while(true){
        clientSocket = serverSocket.accept();
        out = new PrintWriter(clientSocket.getOutputStream(), true);
        in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        
        String greeting = in.readLine();
        
        if(greeting != null){
            if ("Hello".equals(greeting)) {
                out.println("hello client");
                greeting = null;
            }
            else {
                out.println("unrecognised greeting");
                System.out.println("WORKS");
                greeting = null;
            }
        }
        }
           
    }
    
    public void StopSocket() throws IOException {
        in.close();
        out.close();
        clientSocket.close();
        serverSocket.close();
    }
}




