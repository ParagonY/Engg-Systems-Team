import javax.swing.*;
import java.awt.*;
import javax.swing.border.TitledBorder;

public class MyFirstWindow {
    public static void main(String[] args) {
        JFrame frame = new JFrame("Bridge control System");
        frame.setSize(700, 550);
        frame.setLayout(new GridLayout(7, 1)); // includes Reset section

        // ===== Status Labels =====
        JLabel bridgeStatusLabel = new JLabel("Bridge: Down");
        JLabel carStatusLabel    = new JLabel("Car Light: Red");
        JLabel pedStatusLabel    = new JLabel("Pedestrian Light: Red");
        JLabel boatStatusLabel   = new JLabel("Boat Light: Red");

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
        final boolean[] bridgeUp        = { false };
        final boolean[] carGreen        = { false };
        final boolean[] pedGreen        = { false };
        final boolean[] boatGreen       = { false };
        final boolean[] emergencyActive = { false };

        // ===== Interlocks =====
        Runnable applyInterlocks = () -> {
            if (emergencyActive[0]) {
                liftButton.setEnabled(false);
                downButton.setEnabled(false);
                greenButton.setEnabled(false);
                redButton.setEnabled(false);
                greenPedButton.setEnabled(false);
                redPedButton.setEnabled(false);
                greenBoatButton.setEnabled(false);
                redBoatButton.setEnabled(false);
                return;
            }

            boolean trafficActive = carGreen[0] || pedGreen[0];

            // car/ped green disabled when bridge is UP
            greenButton.setEnabled(!bridgeUp[0]);
            greenPedButton.setEnabled(!bridgeUp[0]);

            // boat green only when bridge is UP and no traffic is moving
            greenBoatButton.setEnabled(bridgeUp[0] && !trafficActive);

            // lift only when bridge is DOWN and no traffic is moving
            liftButton.setEnabled(!bridgeUp[0] && !trafficActive);

            // down only if boat light is not green
            downButton.setEnabled(!boatGreen[0]);

            // reds always enabled (unless emergency)
            redButton.setEnabled(true);
            redPedButton.setEnabled(true);
            redBoatButton.setEnabled(true);
        };

        // Unified reset to initial defaults
        Runnable resetToDefaults = () -> {
            emergencyActive[0] = false;
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
        // Bridge
        liftButton.addActionListener(e -> {
            bridgeUp[0] = true;
            bridgeStatusLabel.setText("Bridge: Up");
            // safety: force traffic to red
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
            // flip everything to red but keep bridge position
            carGreen[0] = false; pedGreen[0] = false; boatGreen[0] = false;
            carStatusLabel.setText("Car Light: Red");
            pedStatusLabel.setText("Pedestrian Light: Red");
            boatStatusLabel.setText("Boat Light: Red");
            applyInterlocks.run();
        });

        // Reset System -> back to original default state
        resetButton.addActionListener(e -> resetToDefaults.run());

        // Start in the exact original state
        resetToDefaults.run();

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}
