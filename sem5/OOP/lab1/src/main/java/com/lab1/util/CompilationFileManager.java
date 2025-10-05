package com.lab1.util;

import com.lab1.model.*;
import com.lab1.controller.Compilation;
import java.io.*;
import java.util.ArrayList;
import java.util.List;

/**
 * Manages saving and loading compilations from files
 */
public class CompilationFileManager {

    // Method for saving to file
    public static void saveToFile(Compilation compilation, String filename) throws IOException {
        File folder = new File("compilations_folder");
        if (!folder.exists()) {
            folder.mkdirs();
        }

        // Add .txt extension if not present
        if (!filename.endsWith(".txt")) {
            filename += ".txt";
        }
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(filename))) {
            writer.write("COMPILATION:" + compilation.getTitle());
            writer.newLine();

            for (MusicComposition comp : compilation.getPlaylist()) {
                writer.write(serializeComposition(comp));
                writer.newLine();
            }
        }
    }

    // Method for loading compilation from file
    public static Compilation loadFromFile(String filename) throws IOException {
        List<String> lines = new ArrayList<>();

        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = reader.readLine()) != null) {
                lines.add(line);
            }
        }

        if (lines.isEmpty() || !lines.get(0).startsWith("COMPILATION:")) {
            throw new IOException("Invalid file format");
        }

        String title = lines.get(0).substring("COMPILATION:".length());
        Compilation compilation = new Compilation(title);

        for (int i = 1; i < lines.size(); i++) {
            MusicComposition comp = deserializeComposition(lines.get(i));
            if (comp != null) {
                compilation.addComposition(comp);
            }
        }

        return compilation;
    }

    // "Rules" for writing down compositions to compilation file
    private static String serializeComposition(MusicComposition comp) {
        if (comp instanceof Song) {
            Song song = (Song) comp;
            return String.format("SONG|%s|%s|%d|%s|%d",
                    song.getTitle(), song.getBand(), song.getDurationSec(),
                    song.getGenre(), song.getYearReleased());
        } else if (comp instanceof Instrumental) {
            Instrumental inst = (Instrumental) comp;
            return String.format("INSTRUMENTAL|%s|%s|%d|%s|%d|%s",
                    inst.getTitle(), inst.getArtist(), inst.getDurationSec(),
                    inst.getGenre(), inst.getYearReleased(), inst.getInstrument());
        } else if (comp instanceof Remix) {
            Remix remix = (Remix) comp;
            return String.format("REMIX|%s|%s|%s|%d|%s|%d",
                    remix.getTitle(), remix.getOriginalArtist(), remix.getRemixer(),
                    remix.getDurationSec(), remix.getGenre(), remix.getYearReleased());
        }
        return "";
    }

    // Compilation file reader
    private static MusicComposition deserializeComposition(String line) {
        String[] parts = line.split("\\|");
        if (parts.length == 0)
            return null;

        try {
            switch (parts[0]) {
                case "SONG":
                    return new Song(parts[1], parts[2], Integer.parseInt(parts[3]),
                            parts[4], Integer.parseInt(parts[5]));
                case "INSTRUMENTAL":
                    return new Instrumental(parts[1], parts[2], Integer.parseInt(parts[3]),
                            parts[4], Integer.parseInt(parts[5]), parts[6]);
                case "REMIX":
                    return new Remix(parts[1], parts[2], parts[3],
                            Integer.parseInt(parts[4]), parts[5], Integer.parseInt(parts[6]));
                default:
                    return null;
            }
        } catch (Exception e) {
            System.err.println("Error parsing line: " + line);
            return null;
        }
    }

}