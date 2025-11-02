package PrelimProject;

import java.net.*;
import java.io.*;

public class Main extends Thread{
    public static void main(String[] args) throws IOException {
        Sim messageConnection = new Sim();
        messageConnection.start();
        int test = 0;
        while(true){
            if(messageConnection.greeting != null){
                System.out.println(messageConnection.greeting);
            }
            if(messageConnection.connected == true){
                PrintWriter message = new PrintWriter(messageConnection.clientSocket.getOutputStream(), true);
                message.println("IT FUCKING WOORRKKSSSSSSS");
            }
            //System.out.println(test);
            test++;
        }
    }
    
}


