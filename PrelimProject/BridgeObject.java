package PrelimProject;

public class BridgeObject implements BridgeInterFace {
    String LEDColor; //hexcode for colour
    String bridgeState;
    String vehicularTrafficStatus;
    String shippingTrafficStatus; 
    BridgeObject(String LEDHexCode, String currentBridge, String vehicular, String shipping){
        LEDColor = LEDHexCode;
        bridgeState = currentBridge;
        vehicularTrafficStatus = vehicular;
        shippingTrafficStatus = shipping;
    }

    void updateValues(String LEDHexCode, String currentBridge, String vehicular, String shipping){
        LEDColor = LEDHexCode;
        bridgeState = currentBridge;
        vehicularTrafficStatus = vehicular;
        shippingTrafficStatus = shipping;
    }
}
