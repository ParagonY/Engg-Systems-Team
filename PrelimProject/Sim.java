package PrelimProject;

import java.net.*;
import java.io.*;

public class Sim extends Thread{
    private ServerSocket serverSocket;
    private Socket clientSocket;
    private PrintWriter out;
    private BufferedReader in;
    
    Sim(){
    }

    public void run(){
        try {
            startListener(1111);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }



public void startListener(int port)  throws IOException {
    System.out.println("works here");
        serverSocket = new ServerSocket(port);
        clientSocket = serverSocket.accept();
        while(true){
        out = new PrintWriter(clientSocket.getOutputStream(), true);
        in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        String greeting = in.readLine();
        
            if(greeting != null){
                if ("hello server".equals(greeting)) {
                out.println("hello client");
            }
            else {
                out.println("unrecognised greeting");
            }
            greeting = null;
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




