import java.awt.*;
import java.io.*;
import java.net.*;
import javax.swing.*;
import javax.swing.border.TitledBorder;

public class MyFirstWindow {
    public static void sendToESP(String command) {
        try (Socket socket = new Socket("localhost", 3333); // Replace with ESP32 IP later
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {

            out.println(command);
            String response = in.readLine();
            System.out.println("ESP32 responded: " + response);

        } catch (IOException e) {
            System.out.println("Could not send command: " + command);
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("Bridge control System");
        frame.setSize(720, 620);
        frame.setLayout(new GridLayout(9, 1));

        JLabel modeLabel         = new JLabel("Mode: AUTOMATIC", SwingConstants.CENTER);
        JLabel bridgeStatusLabel = new JLabel("Bridge: Down");
        JLabel carStatusLabel    = new JLabel("Car Light: Red");
        JLabel pedStatusLabel    = new JLabel("Pedestrian Light: Red");
        JLabel boatStatusLabel   = new JLabel("Boat Light: Red");

        JPanel modePanel = new JPanel(new BorderLayout());
        modePanel.add(modeLabel, BorderLayout.CENTER);
        frame.add(modePanel);

        JPanel activationPanel = new JPanel(new BorderLayout());
        activationPanel.add(new JLabel("Activation", SwingConstants.CENTER), BorderLayout.NORTH);
        JButton activateButton = new JButton("ACTIVATE");
        activateButton.setBackground(new Color(46, 204, 113));
        JPanel activateWrap = new JPanel(new FlowLayout());
        activateWrap.add(activateButton);
        activationPanel.add(activateWrap, BorderLayout.CENTER);
        frame.add(activationPanel);

        JPanel bridgePanel = new JPanel(new BorderLayout());
        bridgePanel.add(new JLabel("Bridge Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel bridgeButtons = new JPanel(new FlowLayout());
        JButton liftButton = new JButton("Lift Bridge");
        JButton downButton = new JButton("Down Bridge");
        bridgeButtons.add(liftButton);
        bridgeButtons.add(downButton);
        bridgePanel.add(bridgeButtons, BorderLayout.CENTER);
        frame.add(bridgePanel);

        JPanel carPanel = new JPanel(new BorderLayout());
        carPanel.add(new JLabel("Car Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel carButtons = new JPanel(new FlowLayout());
        JButton greenButton = new JButton("Green Light (Car)");
        JButton redButton   = new JButton("Red Light (Car)");
        carButtons.add(greenButton);
        carButtons.add(redButton);
        carPanel.add(carButtons, BorderLayout.CENTER);
        frame.add(carPanel);

        JPanel pedPanel = new JPanel(new BorderLayout());
        pedPanel.add(new JLabel("Pedestrians Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel pedButtons = new JPanel(new FlowLayout());
        JButton greenPedButton = new JButton("Green Light (Pedestrian)");
        JButton redPedButton   = new JButton("Red Light (Pedestrian)");
        pedButtons.add(greenPedButton);
        pedButtons.add(redPedButton);
        pedPanel.add(pedButtons, BorderLayout.CENTER);
        frame.add(pedPanel);

        JPanel boatPanel = new JPanel(new BorderLayout());
        boatPanel.add(new JLabel("Boat Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel boatButtons = new JPanel(new FlowLayout());
        JButton greenBoatButton = new JButton("Green Light (Boat)");
        JButton redBoatButton   = new JButton("Red Light (Boat)");
        boatButtons.add(greenBoatButton);
        boatButtons.add(redBoatButton);
        boatPanel.add(boatButtons, BorderLayout.CENTER);
        frame.add(boatPanel);

        JPanel emergencyPanel = new JPanel(new BorderLayout());
        emergencyPanel.add(new JLabel("Emergency", SwingConstants.CENTER), BorderLayout.NORTH);
        JButton emergencyStop = new JButton("EMERGENCY STOP");
        emergencyStop.setBackground(new Color(220, 20, 60));
        JPanel emergencyBtnWrap = new JPanel(new FlowLayout());
        emergencyBtnWrap.add(emergencyStop);
        emergencyPanel.add(emergencyBtnWrap, BorderLayout.CENTER);
        frame.add(emergencyPanel);

        JPanel resetPanel = new JPanel(new BorderLayout());
        resetPanel.add(new JLabel("System Control", SwingConstants.CENTER), BorderLayout.NORTH);
        JButton resetButton = new JButton("RESET SYSTEM");
        resetButton.setBackground(new Color(30, 144, 255));
        JPanel resetBtnWrap = new JPanel(new FlowLayout());
        resetBtnWrap.add(resetButton);
        resetPanel.add(resetBtnWrap, BorderLayout.CENTER);
        frame.add(resetPanel);

        JPanel statusPanel = new JPanel(new BorderLayout());
        statusPanel.setBorder(BorderFactory.createTitledBorder(
            BorderFactory.createLineBorder(Color.GRAY, 2), "Status Panel",
            TitledBorder.CENTER, TitledBorder.TOP));
        JPanel statusLabels = new JPanel(new FlowLayout(FlowLayout.CENTER, 50, 20));
        statusLabels.add(bridgeStatusLabel);
        statusLabels.add(carStatusLabel);
        statusLabels.add(pedStatusLabel);
        statusLabels.add(boatStatusLabel);
        statusPanel.add(statusLabels, BorderLayout.CENTER);
        frame.add(statusPanel);

        final boolean[] systemActive    = { false };
        final boolean[] bridgeUp        = { false };
        final boolean[] carGreen        = { false };
        final boolean[] pedGreen        = { false };
        final boolean[] boatGreen       = { false };
        final boolean[] emergencyActive = { false };

        Runnable applyInterlocks = () -> {
            if (emergencyActive[0]) {
                modeLabel.setText("Mode: EMERGENCY");
                liftButton.setEnabled(false);
                downButton.setEnabled(false);
                greenButton.setEnabled(false);
                redButton.setEnabled(false);
                greenPedButton.setEnabled(false);
                redPedButton.setEnabled(false);
                greenBoatButton.setEnabled(false);
                redBoatButton.setEnabled(false);
                activateButton.setEnabled(false);
                resetButton.setEnabled(true);
                return;
            }

            if (!systemActive[0]) {
                modeLabel.setText("Mode: AUTOMATIC");
                liftButton.setEnabled(false);
                downButton.setEnabled(false);
                greenButton.setEnabled(false);
                redButton.setEnabled(false);
                greenPedButton.setEnabled(false);
                redPedButton.setEnabled(false);
                greenBoatButton.setEnabled(false);
                redBoatButton.setEnabled(false);
                activateButton.setEnabled(true);
                resetButton.setEnabled(false);
                return;
            }

            modeLabel.setText("Mode: MANUAL");
            boolean trafficActive = carGreen[0] || pedGreen[0];

            greenButton.setEnabled(!bridgeUp[0]);
            greenPedButton.setEnabled(!bridgeUp[0]);
            greenBoatButton.setEnabled(bridgeUp[0] && !trafficActive);
            liftButton.setEnabled(!bridgeUp[0] && !trafficActive);
            downButton.setEnabled(!boatGreen[0]);
            redButton.setEnabled(true);
            redPedButton.setEnabled(true);
            redBoatButton.setEnabled(true);

            activateButton.setEnabled(false);
            resetButton.setEnabled(true);
        };

        Runnable resetToDefaults = () -> {
            emergencyActive[0] = false;
            systemActive[0]    = false;
            bridgeUp[0]  = false;
            carGreen[0]  = false;
            pedGreen[0]  = false;
            boatGreen[0] = false;

            bridgeStatusLabel.setText("Bridge: Down");
            carStatusLabel.setText("Car Light: Red");
            pedStatusLabel.setText("Pedestrian Light: Red");
            boatStatusLabel.setText("Boat Light: Red");

            applyInterlocks.run();
        };

        // TCP + GUI Bindings
        activateButton.addActionListener(e -> {
            systemActive[0] = true;
            applyInterlocks.run();
        });

        liftButton.addActionListener(e -> {
            bridgeUp[0] = true;
            bridgeStatusLabel.setText("Bridge: Up");
            carGreen[0] = false; pedGreen[0] = false;
            carStatusLabel.setText("Car Light: Red");
            pedStatusLabel.setText("Pedestrian Light: Red");
            sendToESP("LIFT");
            applyInterlocks.run();
        });

        downButton.addActionListener(e -> {
            bridgeUp[0] = false;
            bridgeStatusLabel.setText("Bridge: Down");
            boatGreen[0] = false;
            boatStatusLabel.setText("Boat Light: Red");
            sendToESP("DOWN");
            applyInterlocks.run();
        });

        greenButton.addActionListener(e -> {
            if (!bridgeUp[0]) {
                carGreen[0] = true;
                carStatusLabel.setText("Car Light: Green");
                boatGreen[0] = false;
                boatStatusLabel.setText("Boat Light: Red");
                sendToESP("GREEN_CAR");
                applyInterlocks.run();
            }
        });

        redButton.addActionListener(e -> {
            carGreen[0] = false;
            carStatusLabel.setText("Car Light: Red");
            sendToESP("RED_CAR");
            applyInterlocks.run();
        });

        greenPedButton.addActionListener(e -> {
            if (!bridgeUp[0]) {
                pedGreen[0] = true;
                pedStatusLabel.setText("Pedestrian Light: Green");
                boatGreen[0] = false;
                boatStatusLabel.setText("Boat Light: Red");
                sendToESP("GREEN_PEDESTRIAN");
                applyInterlocks.run();
            }
        });

        redPedButton.addActionListener(e -> {
            pedGreen[0] = false;
            pedStatusLabel.setText("Pedestrian Light: Red");
            sendToESP("RED_PEDESTRIAN");
            applyInterlocks.run();
        });

        greenBoatButton.addActionListener(e -> {
            boatGreen[0] = true;
            boatStatusLabel.setText("Boat Light: Green");
            sendToESP("GREEN_BOAT");
            applyInterlocks.run();
        });

        redBoatButton.addActionListener(e -> {
            boatGreen[0] = false;
            boatStatusLabel.setText("Boat Light: Red");
            sendToESP("RED_BOAT");
            applyInterlocks.run();
        });

        emergencyStop.addActionListener(e -> {
            emergencyActive[0] = true;
            carGreen[0] = false; pedGreen[0] = false; boatGreen[0] = false;
            carStatusLabel.setText("Car Light: Red");
            pedStatusLabel.setText("Pedestrian Light: Red");
            boatStatusLabel.setText("Boat Light: Red");
            sendToESP("EMERGENCY");
            applyInterlocks.run();
        });

        resetButton.addActionListener(e -> {
            resetToDefaults.run();
            sendToESP("RESET");
        });

        resetToDefaults.run();
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}
