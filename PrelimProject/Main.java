package PrelimProject;

import java.net.*;
import java.io.*;

public class Main extends Thread{
    public static void main(String[] args) throws IOException {
        Sim messageConnection = new Sim();
        messageConnection.start();
        int test = 0;
        while(true){
            System.out.println("            "+test);
            test++;
        }
    }
    
}


