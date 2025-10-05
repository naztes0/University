package com.lab1.model;

public class Instrumental extends MusicComposition {
    private String instrument;

    public Instrumental(String title, String artist, int durationSec, String genre, int yearReleased,
            String instrument) {
        super(title, artist, durationSec, genre, yearReleased);
        if (instrument == null || instrument.trim().isEmpty())
            throw new IllegalArgumentException("Instrument cannot be empty");
        this.instrument = instrument;
    }

    public String getInstrument() {
        return instrument;
    }

    @Override
    public String getType() {
        return "Instrumental";
    }

    @Override
    public String toString() {
        return String.format("%s - %s [%s] (%s, %d, %d sec)",
                title, artist, instrument, genre, yearReleased, durationSec);
    }
}
