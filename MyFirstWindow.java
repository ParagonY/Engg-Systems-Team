import javax.swing.*;
import java.awt.*;
import javax.swing.border.TitledBorder;

public class MyFirstWindow {
    public static void main(String[] args) {
        JFrame frame = new JFrame("Bridge control System");
        frame.setSize(720, 620);
        frame.setLayout(new GridLayout(9, 1)); // +1 row for Mode display

        // ===== Mode / Status Labels =====
        JLabel modeLabel         = new JLabel("Mode: AUTOMATIC (locked)", SwingConstants.CENTER);
        JLabel bridgeStatusLabel = new JLabel("Bridge: Down");
        JLabel carStatusLabel    = new JLabel("Car Light: Red");
        JLabel pedStatusLabel    = new JLabel("Pedestrian Light: Red");
        JLabel boatStatusLabel   = new JLabel("Boat Light: Red");

        // ===== Mode Panel =====
        JPanel modePanel = new JPanel(new BorderLayout());
        modePanel.add(modeLabel, BorderLayout.CENTER);
        frame.add(modePanel);

        // ===== Activation Panel =====
        JPanel activationPanel = new JPanel(new BorderLayout());
        activationPanel.add(new JLabel("Activation", SwingConstants.CENTER), BorderLayout.NORTH);
        JButton activateButton = new JButton("ACTIVATE");
        activateButton.setBackground(new Color(46, 204, 113));
        activateButton.setForeground(Color.BLACK);
        activateButton.setOpaque(true);
        activateButton.setFocusPainted(false);
        JPanel activateWrap = new JPanel(new FlowLayout());
        activateWrap.add(activateButton);
        activationPanel.add(activateWrap, BorderLayout.CENTER);
        frame.add(activationPanel);

        // ===== Bridge Panel =====
        JPanel bridgePanel = new JPanel(new BorderLayout());
        bridgePanel.add(new JLabel("Bridge Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel bridgeButtons = new JPanel(new FlowLayout());
        JButton liftButton = new JButton("Lift Bridge");
        JButton downButton = new JButton("Down Bridge");
        bridgeButtons.add(liftButton);
        bridgeButtons.add(downButton);
        bridgePanel.add(bridgeButtons, BorderLayout.CENTER);
        frame.add(bridgePanel);

        // ===== Car Panel =====
        JPanel carPanel = new JPanel(new BorderLayout());
        carPanel.add(new JLabel("Car Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel carButtons = new JPanel(new FlowLayout());
        JButton greenButton = new JButton("Green Light (Car)");
        JButton redButton   = new JButton("Red Light (Car)");
        carButtons.add(greenButton);
        carButtons.add(redButton);
        carPanel.add(carButtons, BorderLayout.CENTER);
        frame.add(carPanel);

        // ===== Pedestrian Panel =====
        JPanel pedPanel = new JPanel(new BorderLayout());
        pedPanel.add(new JLabel("Pedestrians Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel pedButtons = new JPanel(new FlowLayout());
        JButton greenPedButton = new JButton("Green Light (Pedestrian)");
        JButton redPedButton   = new JButton("Red Light (Pedestrian)");
        pedButtons.add(greenPedButton);
        pedButtons.add(redPedButton);
        pedPanel.add(pedButtons, BorderLayout.CENTER);
        frame.add(pedPanel);

        // ===== Boat Panel =====
        JPanel boatPanel = new JPanel(new BorderLayout());
        boatPanel.add(new JLabel("Boat Buttons", SwingConstants.CENTER), BorderLayout.NORTH);
        JPanel boatButtons = new JPanel(new FlowLayout());
        JButton greenBoatButton = new JButton("Green Light (Boat)");
        JButton redBoatButton   = new JButton("Red Light (Boat)");
        boatButtons.add(greenBoatButton);
        boatButtons.add(redBoatButton);
        boatPanel.add(boatButtons, BorderLayout.CENTER);
        frame.add(boatPanel);

        // ===== Emergency Panel =====
        JPanel emergencyPanel = new JPanel(new BorderLayout());
        emergencyPanel.add(new JLabel("Emergency", SwingConstants.CENTER), BorderLayout.NORTH);
        JButton emergencyStop = new JButton("EMERGENCY STOP");
        emergencyStop.setBackground(new Color(220, 20, 60));
        emergencyStop.setForeground(Color.BLACK);
        emergencyStop.setOpaque(true);
        emergencyStop.setFocusPainted(false);
        JPanel emergencyBtnWrap = new JPanel(new FlowLayout());
        emergencyBtnWrap.add(emergencyStop);
        emergencyPanel.add(emergencyBtnWrap, BorderLayout.CENTER);
        frame.add(emergencyPanel);

        // ===== Reset Panel =====
        JPanel resetPanel = new JPanel(new BorderLayout());
        resetPanel.add(new JLabel("System Control", SwingConstants.CENTER), BorderLayout.NORTH);
        JButton resetButton = new JButton("RESET SYSTEM");
        resetButton.setBackground(new Color(30, 144, 255));
        resetButton.setForeground(Color.BLACK);
        resetButton.setOpaque(true);
        resetButton.setFocusPainted(false);
        JPanel resetBtnWrap = new JPanel(new FlowLayout());
        resetBtnWrap.add(resetButton);
        resetPanel.add(resetBtnWrap, BorderLayout.CENTER);
        frame.add(resetPanel);

        // ===== Status Panel =====
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

        // ===== State =====
        final boolean[] systemActive    = { false }; // false = Automatic/locked, true = Manual
        final boolean[] bridgeUp        = { false };
        final boolean[] carGreen        = { false };
        final boolean[] pedGreen        = { false };
        final boolean[] boatGreen       = { false };
        final boolean[] emergencyActive = { false };

        // ===== Interlocks =====
        Runnable applyInterlocks = () -> {
            if (emergencyActive[0]) {
                modeLabel.setText("Mode: EMERGENCY (locked)");
                liftButton.setEnabled(false);
                downButton.setEnabled(false);
                greenButton.setEnabled(false);
                redButton.setEnabled(false);
                greenPedButton.setEnabled(false);
                redPedButton.setEnabled(false);
                greenBoatButton.setEnabled(false);
                redBoatButton.setEnabled(false);
                activateButton.setEnabled(false);
                resetButton.setEnabled(true);  // allow reset from emergency
                return;
            }

            if (!systemActive[0]) {
                modeLabel.setText("Mode: AUTOMATIC (locked)");
                liftButton.setEnabled(false);
                downButton.setEnabled(false);
                greenButton.setEnabled(false);
                redButton.setEnabled(false);
                greenPedButton.setEnabled(false);
                redPedButton.setEnabled(false);
                greenBoatButton.setEnabled(false);
                redBoatButton.setEnabled(false);
                activateButton.setEnabled(true); // can move to Manual
                resetButton.setEnabled(false);   // no need to reset while already automatic
                return;
            }

            // Manual (active) mode
            modeLabel.setText("Mode: MANUAL (interlocked)");
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
            resetButton.setEnabled(true); // <-- enable reset during manual so you can return to Automatic
        };

        // Unified reset to initial defaults (Automatic/locked)
        Runnable resetToDefaults = () -> {
            emergencyActive[0] = false;
            systemActive[0]    = false;   // back to AUTOMATIC mode
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

        // ===== Logic =====
        // Activate -> Manual mode
        activateButton.addActionListener(e -> {
            systemActive[0] = true;
            applyInterlocks.run();
        });

        // Bridge
        liftButton.addActionListener(e -> {
            bridgeUp[0] = true;
            bridgeStatusLabel.setText("Bridge: Up");
            carGreen[0] = false; pedGreen[0] = false;
            carStatusLabel.setText("Car Light: Red");
            pedStatusLabel.setText("Pedestrian Light: Red");
            applyInterlocks.run();
        });

        downButton.addActionListener(e -> {
            bridgeUp[0] = false;
            bridgeStatusLabel.setText("Bridge: Down");
            boatGreen[0] = false;
            boatStatusLabel.setText("Boat Light: Red");
            applyInterlocks.run();
        });

        // Car
        greenButton.addActionListener(e -> {
            if (!bridgeUp[0]) {
                carGreen[0] = true;
                carStatusLabel.setText("Car Light: Green");
                boatGreen[0] = false;
                boatStatusLabel.setText("Boat Light: Red");
                applyInterlocks.run();
            }
        });
        redButton.addActionListener(e -> {
            carGreen[0] = false;
            carStatusLabel.setText("Car Light: Red");
            applyInterlocks.run();
        });

        // Pedestrian
        greenPedButton.addActionListener(e -> {
            if (!bridgeUp[0]) {
                pedGreen[0] = true;
                pedStatusLabel.setText("Pedestrian Light: Green");
                boatGreen[0] = false;
                boatStatusLabel.setText("Boat Light: Red");
                applyInterlocks.run();
            }
        });
        redPedButton.addActionListener(e -> {
            pedGreen[0] = false;
            pedStatusLabel.setText("Pedestrian Light: Red");
            applyInterlocks.run();
        });

        // Boat
        greenBoatButton.addActionListener(e -> {
            boatGreen[0] = true;
            boatStatusLabel.setText("Boat Light: Green");
            applyInterlocks.run();
        });
        redBoatButton.addActionListener(e -> {
            boatGreen[0] = false;
            boatStatusLabel.setText("Boat Light: Red");
            applyInterlocks.run();
        });

        // Emergency Stop
        emergencyStop.addActionListener(e -> {
            emergencyActive[0] = true;
            carGreen[0] = false; pedGreen[0] = false; boatGreen[0] = false;
            carStatusLabel.setText("Car Light: Red");
            pedStatusLabel.setText("Pedestrian Light: Red");
            boatStatusLabel.setText("Boat Light: Red");
            applyInterlocks.run();
        });

        // Reset -> return to AUTOMATIC (locked)
        resetButton.addActionListener(e -> resetToDefaults.run());

        // Start in AUTOMATIC (locked)
        resetToDefaults.run();

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}
