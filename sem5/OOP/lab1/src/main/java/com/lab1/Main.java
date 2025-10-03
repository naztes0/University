package com.lab1;

import com.lab1.model.*;
import java.util.ArrayList;
import java.util.List;
import com.lab1.controller.*;
import com.lab1.util.*;
import java.util.List;
import java.util.Scanner;

public class Main {
    private static Compilation compilation;
    private static Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) {
        initializeCompilation();
        showMenu();
    }

    private static void initializeCompilation() {
        compilation = new Compilation("My collection");

        // // Adding songs
        // compilation.addComposition(new Song("Shiver", "Hanover", 215, "Alternative
        // Metal", 2022));
        // compilation.addComposition(new Song("Be Quiet and Drive", "Deftones", 312,
        // "Alternative Metal", 1997));
        // compilation.addComposition(new Song("Silver", "Deftones", 262, "Alternative
        // Metal", 2000));
        // compilation.addComposition(new Song("Ignite", "Icepeak", 198, "Industrial
        // Rock", 2021));
        // compilation.addComposition(new Song("Bites", "MSI", 193, "Electropunk",
        // 2005));
        // compilation.addComposition(new Song("Gore", "Icepeak", 224, "Experimental
        // Rock", 2020));

        // // Adding instrumentals
        // compilation.addComposition(new Instrumental("Eruption", "Van Halen", 102,
        // "Hard Rock", 1978, "Guitar"));
        // compilation.addComposition(new Instrumental("YYZ", "Rush", 264,
        // "Progressive Rock", 1981, "Bass"));

        // // Adding remixes
        // compilation.addComposition(new Remix("Digital Bath", "Deftones", "DJ Shadow",
        // 289, "Electronic", 2005));
        //
    }

    private static void showMenu() {
        while (true) {
            System.out.println("\n=== MUSIC COMPILATION MANAGER ===");
            System.out.println("1. Show playlist");
            System.out.println("2. Add composition");
            System.out.println("3. Remove composition");
            System.out.println("4. Calculate total duration");
            System.out.println("5. Sort by genre");
            System.out.println("6. Find by duration range");
            System.out.println("7. Save to file");
            System.out.println("8. Load from file");
            System.out.println("0. Exit");
            System.out.print("Choose option: ");

            int choice = getIntInput();

            switch (choice) {
                case 1:
                    compilation.printPlaylist();
                    break;
                case 2:
                    addComposition();
                    break;
                case 3:
                    removeComposition();
                    break;
                case 4:
                    showTotalDuration();
                    break;
                case 5:
                    compilation.sortByGenre();
                    System.out.println("Sorted by genre!");
                    compilation.printPlaylist();
                    break;
                case 6:
                    findByDuration();
                    break;
                case 7:
                    saveToFile();
                    break;
                case 8:
                    loadFromFile();
                    break;
                case 0:
                    System.out.println("Goodbye!");
                    scanner.close();
                    return;
                default:
                    System.out.println("Invalid option!");
            }
        }
    }

    private static void addComposition() {
        System.out.println("\n=== ADD COMPOSITION ===");
        System.out.println("Choose type:");
        System.out.println("1. Song");
        System.out.println("2. Instrumental");
        System.out.println("3. Remix");
        System.out.print("Your choice: ");

        int type = getIntInput();

        try {
            switch (type) {
                case 1:
                    addSong();
                    break;
                case 2:
                    addInstrumental();
                    break;
                case 3:
                    addRemix();
                    break;
                default:
                    System.out.println("Invalid type!");
                    return;
            }
            System.out.println("Composition added successfully!");
        } catch (Exception e) {
            System.out.println("Error adding composition: " + e.getMessage());
        }
    }

    private static void addSong() {
        System.out.print("Enter title: ");
        String title = scanner.nextLine();

        System.out.print("Enter band name: ");
        String band = scanner.nextLine();

        System.out.print("Enter duration (seconds): ");
        int duration = getIntInput();

        System.out.print("Enter genre: ");
        String genre = scanner.nextLine();

        System.out.print("Enter year released: ");
        int year = getIntInput();

        Song song = new Song(title, band, duration, genre, year);
        compilation.addComposition(song);
    }

    private static void addInstrumental() {
        System.out.print("Enter title: ");
        String title = scanner.nextLine();

        System.out.print("Enter artist name: ");
        String artist = scanner.nextLine();

        System.out.print("Enter duration (seconds): ");
        int duration = getIntInput();

        System.out.print("Enter genre: ");
        String genre = scanner.nextLine();

        System.out.print("Enter year released: ");
        int year = getIntInput();

        System.out.print("Enter main instrument: ");
        String instrument = scanner.nextLine();

        Instrumental instrumental = new Instrumental(title, artist, duration, genre, year, instrument);
        compilation.addComposition(instrumental);
    }

    private static void addRemix() {
        System.out.print("Enter title: ");
        String title = scanner.nextLine();

        System.out.print("Enter original artist: ");
        String originalArtist = scanner.nextLine();

        System.out.print("Enter remixer name: ");
        String remixer = scanner.nextLine();

        System.out.print("Enter duration (seconds): ");
        int duration = getIntInput();

        System.out.print("Enter genre: ");
        String genre = scanner.nextLine();

        System.out.print("Enter year released: ");
        int year = getIntInput();

        Remix remix = new Remix(title, originalArtist, remixer, duration, genre, year);
        compilation.addComposition(remix);
    }

    private static void removeComposition() {
        if (compilation.getPlaylist().size() == 0) {
            System.out.println("Playlist is empty!");
            return;
        }

        System.out.println("\n=== REMOVE COMPOSITION ===");
        compilation.printPlaylist();

        System.out.print("\nEnter number of composition to remove (1-" + compilation.getPlaylist().size() + "): ");
        int index = getIntInput();

        if (index < 1 || index > compilation.getPlaylist().size()) {
            System.out.println("Invalid number!");
            return;
        }

        List<MusicComposition> playlist = compilation.getPlaylist();
        MusicComposition toRemove = playlist.get(index - 1);

        compilation.removeComposition(toRemove);
        System.out.println("Composition removed successfully!");
    }

    private static void showTotalDuration() {
        int total = compilation.calculateTotalDuration();
        System.out.printf("Total duration: %d minutes %d seconds%n",
                total / 60, total % 60);
    }

    private static void findByDuration() {
        System.out.print("Enter minimum duration (seconds): ");
        int min = getIntInput();
        System.out.print("Enter maximum duration (seconds): ");
        int max = getIntInput();

        try {
            List<MusicComposition> found = compilation.findByDuration(min, max);

            System.out.println("\n=== Found compositions ===");
            if (found.isEmpty()) {
                System.out.println("No compositions found in this range.");
            } else {
                for (int i = 0; i < found.size(); i++) {
                    System.out.printf("%d. %s%n", i + 1, found.get(i).toString());
                }
            }
        } catch (IllegalArgumentException e) {
            System.out.println("Error: " + e.getMessage());
        }
    }

    private static void saveToFile() {
        System.out.print("Enter filename: ");
        String filename = scanner.nextLine();
        try {
            CompilationFileManager.saveToFile(compilation, filename);
            System.out.println("Saved successfully!");
        } catch (Exception e) {
            System.out.println("Error saving: " + e.getMessage());
        }
    }

    private static void loadFromFile() {
        System.out.print("Enter filename: ");
        String filename = scanner.nextLine();
        try {
            compilation = CompilationFileManager.loadFromFile(filename);
            System.out.println("Loaded successfully!");
        } catch (Exception e) {
            System.out.println("Error loading: " + e.getMessage());
        }
    }

    private static int getIntInput() {
        while (!scanner.hasNextInt()) {
            scanner.next();
            System.out.print("Please enter a number: ");
        }
        int result = scanner.nextInt();
        scanner.nextLine(); // consume newline
        return result;
    }
}

// public class Main {
// public static void main(String[] args) {
// Song track1 = new Song("Shiver", "Hanover", 215, "Alternative Metal", 2022);
// Song track2 = new Song("Be Quiet and Drive", "Deftones", 312, "Alternative
// Metal", 1997);
// Song track3 = new Song("Silver", "Deftones", 262, "Alternative Metal", 2000);
// Song track4 = new Song("Ignite", "Icepeak", 198, "Industrial Rock", 2021);
// Song track5 = new Song("Bites", "MSI", 193, "Electropunk", 2005);
// Song track6 = new Song("Gore", "Icepeak", 224, "Experimental Rock", 2020);
// Song track7 = new Song("My Songs Know What You Did In The Dark", "MSI", 202,
// "Industrial Rock", 2006);
// Song track8 = new Song("Digital Bath", "Deftones", 269, "Alternative Metal",
// 2000);
// Song track9 = new Song("Collapse", "Hanover", 241, "Nu Metal", 2023);
// Song track10 = new Song("Shut Me Up", "MSI", 195, "Electropunk", 2005);
// Instrumental track11 = new Instrumental("Adagio for Strings", "Samuel
// Barber", 600, "Classical", 1938,
// "Orchestra");

// MusicComposition[] tracks = {
// track1, track2, track3, track4, track5,
// track6, track7, track8, track9, track10, track11
// };

// Compilation playlist = new Compilation("Blablabla");
// for (int i = 0; i < tracks.length; i++) {
// playlist.addComposition(tracks[i]);
// }
// playlist.printPlaylist();
// }
// }