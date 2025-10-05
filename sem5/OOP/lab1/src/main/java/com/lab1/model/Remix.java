package com.lab1.model;

public class Remix extends MusicComposition {
    private String originalArtist;
    private String remixer;

    public Remix(String title, String originalArtist, String remixer, int durationSec, String genre, int yearReleased) {
        super(title, originalArtist, durationSec, genre, yearReleased);
        this.originalArtist = originalArtist;
        this.remixer = remixer;
    }

    @Override
    public String getType() {
        return "Remix";
    }

    public String getOriginalArtist() {
        return originalArtist;
    }

    public String getRemixer() {
        return remixer;
    }

    @Override
    public String toString() {
        return String.format("%s - %s (remix by %s) (%s, %d, %d sec)",
                title, originalArtist, remixer, genre, yearReleased, durationSec);
    }
}
