package com.lab1.model;

//Abstract class to another types pf composition
public abstract class MusicComposition {
    protected String title;
    protected String artist;
    protected int durationSec;
    protected String genre;
    protected int yearReleased;

    public MusicComposition(String title, String artist, int durationSec, String genre, int yearReleased) {
        if (title == null || title.trim().isEmpty()) {
            throw new IllegalArgumentException("Title cannot be empty");
        }
        if (artist == null || artist.trim().isEmpty()) {
            throw new IllegalArgumentException("Artist cannot be empty");
        }
        if (durationSec <= 0) {
            throw new IllegalArgumentException("Duration must be positive");
        }
        if (genre == null || genre.trim().isEmpty()) {
            throw new IllegalArgumentException("Genre cannot be empty");
        }
        if (yearReleased < 1000 || yearReleased > 2100) {
            throw new IllegalArgumentException("Invalid year");
        }
        this.title = title;
        this.artist = artist;
        this.durationSec = durationSec;
        this.genre = genre;
        this.yearReleased = yearReleased;
    }

    public String getTitle() {
        return title;
    }

    public String getArtist() {
        return artist;
    }

    public int getDurationSec() {
        return durationSec;
    }

    public String getGenre() {
        return genre;
    }

    public int getYearReleased() {
        return yearReleased;
    }

    public abstract String getType();

    @Override
    public String toString() {
        return title + " - " + artist + " (" + genre + ", " + yearReleased + ", " + durationSec + " sec)";
    }
}
