package com.lab1.model;

public class Song extends MusicComposition {
    private String band;

    public Song(String title, String band, int durationSec, String genre, int yearReleased) {
        super(title, band, durationSec, genre, yearReleased);
        this.band = band;
    }

    public String getBand() {
        return band;
    }

    @Override
    public String getType() {
        return "Song";
    }

    @Override
    public String toString() {
        return title + " - " + band + " (" + genre + ", " + yearReleased + ", " + durationSec + " sec)";
    }

}
