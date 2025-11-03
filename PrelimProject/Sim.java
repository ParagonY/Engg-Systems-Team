    package PrelimProject;

    import java.io.*;
    import java.net.*;

    public class Sim extends Thread{
        public volatile ServerSocket serverSocket;
        public volatile Socket clientSocket;
        public volatile PrintWriter out;
        public volatile BufferedReader in;
        public volatile boolean connected;
        public volatile String message;
        public volatile String greeting;
        
        Sim(){
        }

        public void run(){
            connected = false;
            try {
                startListener(5050);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    public void startListener(int port)  throws IOException {
        System.out.println("works here");
            InetAddress inetAddress = InetAddress.getByName("192.168.4.2");
            //serverSocket = new ServerSocket(port,5,inetAddress);
            serverSocket = new ServerSocket(port);
            
            while(true){
            clientSocket = serverSocket.accept();
            connected = true;
            out = new PrintWriter(clientSocket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            
            greeting = in.readLine();
            //System.out.println(greeting);
            if(greeting != null){
                if ("Hello".equals(greeting)) {
                    //out.println("hello client");
                    greeting = null;
                }
                else {
                    //out.println("unrecognised greeting");
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
         // Sends a command message to the connected ESP client
        public void sendMessage(String msg) {
            if (connected && out != null) {
                out.println(msg);
            } else {
                System.out.println("Not connected to any client.");
            }
        }
    }




