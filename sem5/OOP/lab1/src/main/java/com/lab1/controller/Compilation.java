package com.lab1.controller;

import com.lab1.model.MusicComposition;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

public class Compilation {
    private String title;
    private List<MusicComposition> playlist;

    public Compilation(String title) {
        this.title = title;
        this.playlist = new ArrayList<>();
    }

    public String getTitle() {
        return title;
    }

    public List<MusicComposition> getPlaylist() {
        return new ArrayList<>(playlist);
    }

    public void addComposition(MusicComposition composition) {
        if (composition == null) {
            throw new IllegalArgumentException("Composition cannot be null");
        }
        playlist.add(composition);
    }

    public void removeComposition(MusicComposition composition) {
        playlist.remove(composition);
    }

    public int calculateTotalDuration() {
        int total = 0;
        for (MusicComposition comp : playlist) {
            total += comp.getDurationSec();
        }
        return total;
    }

    // Sorting composition by genre(->then by title) in playlist
    public void sortByGenre() {
        playlist.sort(Comparator.comparing(MusicComposition::getGenre)
                .thenComparing(MusicComposition::getTitle));
    }

    // Find compositions in the given range of possible duration
    public List<MusicComposition> findByDuration(int minDur, int maxDur) {
        if (minDur < 0 || maxDur < minDur)
            throw new IllegalArgumentException("Invalid renge");
        List<MusicComposition> result = new ArrayList<>();

        for (MusicComposition comp : playlist) {
            if ((comp.getDurationSec() >= minDur) && (comp.getDurationSec() <= maxDur)) {
                result.add(comp);
            }
        }
        return result;
    }

    public void printPlaylist() {
        System.out.println("\n" + "=".repeat(70));
        System.out.println("Compilation: " + title);
        System.out.println("Total duration: " + formatDuration(calculateTotalDuration()));
        System.out.println("=".repeat(70));

        for (int i = 0; i < playlist.size(); i++) {
            System.out.printf("%2d. %s%n", i + 1, playlist.get(i).toString());
        }

        System.out.println("=".repeat(70));
    }

    private String formatDuration(int seconds) {
        int minutes = seconds / 60;
        int secs = seconds % 60;
        return String.format("%d:%02d", minutes, secs);
    }
}
