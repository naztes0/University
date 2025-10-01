package lab1.src.main.java.lab1;

import java.util.ArrayList;

public class Compilation {
    private String title;
    private ArrayList<Song> playlist;

    public Compilation(String title) {
        this.title = title;
        this.playlist = new ArrayList<>(); // empty one
    }

    public String getPlaylistTitle() {
        return title;
    }

    public void addSong(Song song) {
        playlist.add(song);
    }

    public void removeSong(Song song) {
        playlist.remove(song);
    }

    public double totalDuration() {
        double total = 0;
        for (Song s : playlist) {
            total += s.getDurationSec();
        }
        return total;
    }

    public void printPlaylist() {
        System.out.println("\n" + "=".repeat(55));
        System.out.println("Playlist: " + title);
        System.out.println("=".repeat(55));
        for (int i = 0; i < playlist.size(); i++) {
            System.out.println(i + 1 + ": " + playlist.get(i).toString());
        }
        System.out.println("=".repeat(55));
    }
}
