package lab1.src.main.java.lab1;

public class Song {
    private String title;
    private String band;
    private Integer durationSec;
    private String genre;
    private Integer yearReleased;

    // Setter
    public Song(String title, String band, Integer durationSec, String genre, Integer yearReleased) {
        this.title = title;
        this.band = band;
        this.durationSec = durationSec;
        this.genre = genre;
        this.yearReleased = yearReleased;
    }

    // getter
    public double getDurationSec() {
        return this.durationSec;
    }

    @Override
    public String toString() {
        return title + " - " + band + " (" + genre + ", " + yearReleased + ", " + durationSec + " sec)";
    }
}
