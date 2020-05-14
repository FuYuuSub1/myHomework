package model;

import java.awt.image.BufferedImage;
import java.io.*;

public class Piece implements Comparable<Piece> {

    /**class fields*/
    private boolean survive;
    private Animal animal;
    private boolean team; // true if red, false if black
    private int rank;
    private int x;
    private int y;

    final private BufferedImage image;

    public Piece(Animal animal , int rank, BufferedImage image, boolean team) {
        this.animal = animal;
        this.image = image;
        this.rank = rank;
        this.team = team;
        this.survive = true;
    }

    /** captures chess piece */
    public void capture() {
        this.survive = false;
    }

    /** revive piece (for reset purpose) */
    public void revive(){
        this.survive = true;
    }

    /** returns true if chess piece is alive, false otherwise */
    public boolean isSurvive(){
        return survive;
    }

    /** return chess piece's image */
    public BufferedImage getImage(){
        return image;
    }

    /** set rank */
    public int getRank(){
        return rank;
    }

    /** return rank */
    public void setRank(){
        this.rank = rank;
    }

    /** return current x coordinate */
    public int getX(){
        return x;
    }

    /** return current y coordinate */
    public int getY(){
        return y;
    }

    /** set x coordinate */
    public void setX(int x){
        this.x = x;
    }

    /** set y coordinate */
    public void setY(int y){
        this.y = y;
    }

    /** return animal */
    public Animal getAnimal() {
        return animal;
    }

    /** returns team */
    public boolean getTeam(){
        return team;
    }

    /** toString method */
    @Override
    public String toString(){
        String a = "";
        a = animal.toString().toLowerCase();
        if (team) {
            a = "red " + a;
        }
        else {
            a = "black " + a;
        }
        return a;
    }

    @Override
    public int compareTo(Piece other){
        Animal o = other.getAnimal();
        return (animal.ordinal() - o.ordinal()); 
    }

    @Override
    public boolean equals(Object other){

        // optimization for alias
        if (this == other) {
            return true;
        }
        if (other == null) { 
            return false; 
        }
        if (other instanceof Piece){
            Piece o = (Piece) other;
            return (toString().equals(o.toString()));
        }
        return false;
    }


}
