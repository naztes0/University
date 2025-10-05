package com.lab1.model;

public class Remix extends MusicComposition {
    private String originalArtist;
    private String remixer;

    public Remix(String title, String originalArtist, String remixer, int durationSec, String genre, int yearReleased) {
        super(title, originalArtist, durationSec, genre, yearReleased);
        if (originalArtist == null || originalArtist.trim().isEmpty())
            throw new IllegalArgumentException("Original artist cannot be empty");
        if (remixer == null || remixer.trim().isEmpty())
            throw new IllegalArgumentException("Remixer cannot be empty");
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
