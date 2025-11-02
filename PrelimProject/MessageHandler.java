package PrelimProject;

import java.util.ArrayList;

public class MessageHandler {

    //Message Formats will be "bridgeState,ShippingTrafficState,VechilarTraffic,LED"
    //E.G "LIFTED,Shipping Traffic Detected,Vechilar Traffic NOT Dectected,LED FLASHING"

    String receivedMessage;
    String sendingMessage;
    ArrayList<String> messageQueue = new ArrayList<String>();
    BridgeObject bridge;

    public MessageHandler(BridgeObject Bridge){
        bridge = Bridge;
    }

    public void addMessage(String receviedString){
        messageQueue.add(receviedString);
    }

    public BridgeObject filterMessage(){
        receivedMessage = messageQueue.remove(0);
        String[] received = receivedMessage.split(",");
        for(int i = 0; i<received.length; i++){
            if(i == 0){
                bridge.bridgeState = received[i];
            }
            else if(i == 1){
                bridge.shippingTrafficStatus = received[i];
            }
            else if(i == 2){
                bridge.vehicularTrafficStatus = received[i];
            }
            else{
                bridge.LEDColor = received[i];
            }
        }
        return bridge;
    }
}
