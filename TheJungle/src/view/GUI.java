package view;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

import controller.Controller;
import model.Board;

@SuppressWarnings("serial")
public class GUI extends JPanel {

    private static BoardPanel panel;
    private static Controller controller;
    private static JFrame instructions;
    private static JLabel score;
    private static JFrame winFrame;
    private static JLabel winMessage;
    private static JFrame IllegalMove;
    private static JLabel IllegalMessage;
    private static final JPanel toolbar = new JPanel();
    private static final JButton rematch = new JButton("Re-match");
    private static final JButton quit = new JButton("Quit");
    private static final JButton save = new JButton("Save");
    private static final JButton load = new JButton("Load");

    public GUI () {
        setLayout(new BorderLayout());
        // initialize chess board
        panel = new BoardPanel(new Board());
        // add Chess board 
        add(panel, BorderLayout.CENTER);
        // add options tool bar at the top
        add (createToolbar(), BorderLayout.NORTH);
        setUpWinFrame();
        setUpIllegalMove();
        // initialize controller
        controller = new Controller(panel, winFrame, winMessage, IllegalMove, IllegalMessage, score);

    }

    /** creates options tool bar */
    private static JPanel createToolbar() {
        toolbar.setLayout(new FlowLayout());
        // add action listeners to quit, rematch, and show instruction buttons
        addActionListeners();
        // add buttons to toolbar
        toolbar.add(quit);
        toolbar.add(rematch);
        toolbar.add(save);
        toolbar.add(load);
        // add score display
        score = new JLabel("Red: 0 Black: 0");
        toolbar.add(score);
        return toolbar;
    }

    /** sets up frame to be displayed when one player wins */
    private void setUpWinFrame() {
        winFrame = new JFrame("Congratulations");
        // add message label
        winMessage = new JLabel();
        winMessage.setHorizontalAlignment(SwingConstants.CENTER);
        winMessage.setPreferredSize(new Dimension(250,20));
        winFrame.add(winMessage);

        // set frame properties
        winFrame.setLocation(400,300);
        winFrame.pack();
        winFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        winFrame.setVisible(false);
    }

    private void setUpIllegalMove(){
        IllegalMove = new JFrame("Illegal Move");
        // add message label
        IllegalMessage = new JLabel();
        IllegalMessage.setHorizontalAlignment(SwingConstants.CENTER);
        IllegalMessage.setPreferredSize(new Dimension(100, 20));
        IllegalMove.add(IllegalMessage);

        // set frame properties
        IllegalMove.setLocation(300, 200);
        IllegalMove.pack();
        IllegalMove.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        IllegalMove.setVisible(false);
    }

    private static void addActionListeners() {
        rematch.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent arg0) {
                reset();
            }
        });
        quit.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent arg0) {
                System.exit(0);
            }
        });
        save.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent arg0) {
                save();
            }
        });
        load.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent arg0) {
                load();
            }
        });
    }

    /** resets panel */
    private static void reset() {
        controller.reset();
    }

    /** save */
    private static void save(){controller.save();}

    /** load */
    private static void load(){controller.load();}


}
