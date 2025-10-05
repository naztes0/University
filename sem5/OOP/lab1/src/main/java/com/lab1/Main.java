package com.lab1;

import com.lab1.model.*;
import java.util.ArrayList;
import java.util.List;
import com.lab1.controller.*;
import com.lab1.util.*;
import java.util.Scanner;
import java.io.File;
import java.io.IOException;

public class Main {
    private static Compilation compilation;
    private static Scanner scanner = new Scanner(System.in);
    private static String compilationFolder = "compilations_folder";
    private static final String FILE_EXTENSION = ".txt";

    public static void main(String[] args) {
        // showMenu();
        showCompilationMenu();
    }

    // Compilation manager menu
    private static void showCompilationMenu() {
        while (true) {
            System.out.println("\n=== COMPILATION MANAGER ===");
            System.out.println("1. Create a new compilation");
            System.out.println("2. Select existing compilation");
            System.out.println("3. Remove compilation");
            System.out.println("0. Exit");
            System.out.print("Choose option: ");
            int choice = getIntInput();
            switch (choice) {
                case 1:
                    createCompilation();
                    break;
                case 2:
                    selectExistingCompilation();
                    break;
                case 3:
                    deleteCompilation();
                    break;
                case 0:
                    System.out.println("Goodbye!");
                    scanner.close();
                    return;
            }
        }
    }

    private static void createCompilation() {
        System.out.print("Enter compilation title: ");
        String title = scanner.nextLine();
        compilation = new Compilation(title);
        System.out.println("Compilation created successfully!");
        showMenu();
    }

    private static void selectExistingCompilation() {
        System.out.println("===== Select a compilation =====");
        List<String> compilations = compilationsList();
        if (compilations == null)
            return;
        for (int i = 0; i < compilations.size(); i++) {
            System.out.printf("%2d. %s%n", i + 1, compilations.get(i));
        }
        System.out.println("=".repeat(32));
        System.out.print("Choose the number of compilation: ");
        int choice = getIntInput();
        try {
            compilation = CompilationFileManager
                    .loadFromFile(compilationFolder + File.separator + compilations.get(choice - 1));
        } catch (IOException e) {
            System.out.println("Error while loading compilation: " + e.getMessage());
        }
        System.out.println("Compilation " + compilations.get(choice - 1) + " was successfully chosen");
        showMenu();
    }

    private static List<String> compilationsList() {
        File folder = new File(compilationFolder);
        if (!folder.exists()) {
            folder.mkdirs();
        }
        File[] files = folder.listFiles((dir, name) -> name.endsWith(FILE_EXTENSION));
        List<String> names = new ArrayList<>();
        if (files == null || files.length == 0) {
            System.out.println("Folder is empty. Create a new compilation");
            return null;
        }
        for (File f : files) {
            names.add(f.getName());
        }
        return names;

    }

    private static void deleteCompilation() {
        System.out.println("===== Delete a compilation =====");
        List<String> compilations = compilationsList();
        if (compilations == null || compilations.isEmpty())
            return;
        for (int i = 0; i < compilations.size(); i++) {
            System.out.printf("%2d. %s%n", i + 1, compilations.get(i));
        }
        System.out.println("=".repeat(32));
        System.out.print("Choose the number of compilation to delete: ");
        int choice = getIntInput();
        if (choice < 1 || choice > compilations.size()) {
            System.out.println("Invalid number!");
            return;
        }
        String filename = compilations.get(choice - 1);
        System.out.print("Are you sure you want to delete \"" + filename + "\"? (y/n): ");
        String answer = scanner.nextLine().trim().toLowerCase();

        if (answer.equals("y")) {
            File fileToDelete = new File(compilationFolder, filename);
            if (fileToDelete.delete()) {
                System.out.println("Compilation deleted successfully!");
            } else {
                System.out.println("Error: Could not delete file.");
            }
        } else if (answer.equals("n")) {
            System.out.println("Deletion cancelled.");
        } else {
            System.out.println("Invalid input. Please enter 'y' or 'n'.");
        }
    }

    // Chosen compilation menu
    private static void showMenu() {
        if (compilation == null) {
            System.out.println("No compilation selected!");
            return;
        }
        while (true) {
            System.out.println("\n=== MUSIC COMPILATION MANAGER ===");
            System.out.println("1. Show playlist");
            System.out.println("2. Add composition");
            System.out.println("3. Remove composition");
            System.out.println("4. Calculate total duration");
            System.out.println("5. Sort by genre");
            System.out.println("6. Find by duration range");
            System.out.println("7. Save to file");
            // System.out.println("8. Load from file");
            System.out.println("8. Move to compilation menu");
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
                    return;
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
        if (duration <= 0) {
            System.out.println("Duration must be positive!");
            return;
        }
        System.out.print("Enter genre: ");
        String genre = scanner.nextLine();

        System.out.print("Enter year released: ");
        int year = getIntInput();
        if (year < 1000 || year > 2100) {
            System.out.println("Invalid year!");
            return;
        }

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
        String filename = compilation.getTitle();
        if (!filename.endsWith(FILE_EXTENSION)) {
            filename += FILE_EXTENSION;
        }

        String fullPath = compilationFolder + File.separator + filename;

        try {
            CompilationFileManager.saveToFile(compilation, fullPath);
        } catch (Exception e) {
            System.out.println("Error saving: " + e.getMessage());
        }
    }

    private static int getIntInput() {
        while (!scanner.hasNextInt()) {
            scanner.next();
            System.out.print("Please enter a number: ");
        }
        int result = scanner.nextInt();
        scanner.nextLine();
        return result;
    }
}
