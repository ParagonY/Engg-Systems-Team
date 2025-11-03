package PrelimProject;

import java.awt.*;
import javax.swing.*;
import javax.swing.border.TitledBorder;

public class MyFirstWindow {

    // === Sim reference (TCP handled elsewhere) ===
    private static Sim sim;

    // === Bridge State ===
    static boolean systemActive = false;
    static boolean bridgeUp = false;
    static boolean carGreen = false;
    static boolean pedGreen = false;
    static boolean boatGreen = false;
    static boolean emergencyActive = false;

    // === GUI Labels ===
    static JLabel modeLabel = new JLabel("Mode: AUTOMATIC", SwingConstants.CENTER);
    static JLabel bridgeStatusLabel = new JLabel("Bridge: Down");
    static JLabel carStatusLabel = new JLabel("Car Light: Red");
    static JLabel pedStatusLabel = new JLabel("Pedestrian Light: Red");
    static JLabel boatStatusLabel = new JLabel("Boat Light: Red");

    // === GUI Builder ===
    public static void buildGUI() {
        JFrame frame = new JFrame("Bridge Control System");
        frame.setSize(720, 620);
        frame.setLayout(new GridLayout(9, 1));

        frame.add(makePanel(modeLabel));
        frame.add(makePanelWithButton("Activation", makeButton("ACTIVATE", Color.GREEN, e -> activateSystem())));
        frame.add(makePanelWithButtons("Bridge Buttons", "Lift Bridge", "Down Bridge", e -> liftBridge(), e -> lowerBridge()));
        frame.add(makePanelWithButtons("Car Buttons", "Green Light (Car)", "Red Light (Car)", e -> setLight("CAR", true), e -> setLight("CAR", false)));
        frame.add(makePanelWithButtons("Pedestrian Buttons", "Green Light (Pedestrian)", "Red Light (Pedestrian)", e -> setLight("PED", true), e -> setLight("PED", false)));
        frame.add(makePanelWithButtons("Boat Buttons", "Green Light (Boat)", "Red Light (Boat)", e -> setLight("BOAT", true), e -> setLight("BOAT", false)));
        frame.add(makePanelWithButton("Emergency", makeButton("EMERGENCY STOP", Color.RED, e -> triggerEmergency())));
        frame.add(makePanelWithButton("System Control", makeButton("RESET SYSTEM", Color.CYAN, e -> resetSystem())));
        frame.add(makeStatusPanel());

        resetSystem();
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }

    // === Action Logic ===
    static void activateSystem() {
        systemActive = true;
        applyInterlocks();
    }

    static void liftBridge() {
        bridgeUp = true;
        carGreen = pedGreen = false;
        updateStatus("Bridge", "Up");
        setLight("CAR", false);
        setLight("PED", false);
        if (sim != null) sim.sendMessage("LIFT");
        applyInterlocks();
    }

    static void lowerBridge() {
        bridgeUp = false;
        updateStatus("Bridge", "Down");
        setLight("BOAT", false);
        if (sim != null) sim.sendMessage("DOWN");
        applyInterlocks();
    }

    static void triggerEmergency() {
        emergencyActive = true;
        carGreen = pedGreen = boatGreen = false;
        updateAllLights("Red");
        if (sim != null) sim.sendMessage("EMERGENCY_STOP");
        applyInterlocks();
    }

    static void resetSystem() {
        systemActive = bridgeUp = carGreen = pedGreen = boatGreen = emergencyActive = false;
        updateAllLights("Red");
        updateStatus("Bridge", "Down");
        if (sim != null) sim.sendMessage("RESET_SYSTEM");
        applyInterlocks();
    }

    // === Utility Methods ===
    static void updateStatus(String type, String value) {
        switch (type) {
            case "Bridge" -> bridgeStatusLabel.setText("Bridge: " + value);
            case "Car" -> carStatusLabel.setText("Car Light: " + value);
            case "Pedestrian" -> pedStatusLabel.setText("Pedestrian Light: " + value);
            case "Boat" -> boatStatusLabel.setText("Boat Light: " + value);
        }
    }

    static void updateAllLights(String color) {
        carStatusLabel.setText("Car Light: " + color);
        pedStatusLabel.setText("Pedestrian Light: " + color);
        boatStatusLabel.setText("Boat Light: " + color);
    }

    static void setLight(String type, boolean green) {
        String color = green ? "Green" : "Red";
        switch (type) {
            case "CAR" -> {
                carGreen = green;
                updateStatus("Car", color);
                if (sim != null) sim.sendMessage(green ? "GREEN_CAR" : "RED_CAR");
            }
            case "PED" -> {
                pedGreen = green;
                updateStatus("Pedestrian", color);
                if (sim != null) sim.sendMessage(green ? "GREEN_PEDESTRIAN" : "RED_PEDESTRIAN");
            }
            case "BOAT" -> {
                boatGreen = green;
                updateStatus("Boat", color);
                if (sim != null) sim.sendMessage(green ? "GREEN_BOAT" : "RED_BOAT");
            }
        }
        applyInterlocks();
    }

    static void applyInterlocks() {
        if (emergencyActive) {
            modeLabel.setText("Mode: EMERGENCY");
            return;
        }
        if (!systemActive) {
            modeLabel.setText("Mode: AUTOMATIC");
            return;
        }
        modeLabel.setText("Mode: MANUAL");
    }

    // === GUI Helpers ===
    static JPanel makePanel(JComponent comp) {
        JPanel p = new JPanel(new BorderLayout());
        p.add(comp, BorderLayout.CENTER);
        return p;
    }

    static JPanel makePanelWithButton(String title, JButton button) {
        JPanel p = new JPanel(new BorderLayout());
        p.add(new JLabel(title, SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel inner = new JPanel(new FlowLayout());
        inner.add(button);
        p.add(inner, BorderLayout.CENTER);
        return p;
    }

    static JPanel makePanelWithButtons(String title, String b1, String b2, java.awt.event.ActionListener a1, java.awt.event.ActionListener a2) {
        JPanel p = new JPanel(new BorderLayout());
        p.add(new JLabel(title, SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel buttons = new JPanel(new FlowLayout());
        buttons.add(makeButton(b1, null, a1));
        buttons.add(makeButton(b2, null, a2));
        p.add(buttons, BorderLayout.CENTER);
        return p;
    }

    static JButton makeButton(String text, Color color, java.awt.event.ActionListener action) {
        JButton b = new JButton(text);
        if (color != null) b.setBackground(color);
        b.setFocusPainted(false);
        b.addActionListener(action);
        return b;
    }

    static JPanel makeStatusPanel() {
        JPanel p = new JPanel(new BorderLayout());
        p.setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(Color.GRAY, 2),
                "Status Panel", TitledBorder.CENTER, TitledBorder.TOP));
        JPanel labels = new JPanel(new FlowLayout(FlowLayout.CENTER, 50, 20));
        labels.add(bridgeStatusLabel);
        labels.add(carStatusLabel);
        labels.add(pedStatusLabel);
        labels.add(boatStatusLabel);
        p.add(labels, BorderLayout.CENTER);
        return p;
    }

    // === Setter to receive Sim instance from Main ===
    public static void setSimInstance(Sim simInstance) {
        sim = simInstance;
    }
}
