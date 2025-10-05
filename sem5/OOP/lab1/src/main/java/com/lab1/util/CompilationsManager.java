package com.lab1.util;

import com.lab1.controller.Compilation;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import com.lab1.util.AppConstants;

public class CompilationsManager {
    private Scanner scanner;

    public CompilationsManager(Scanner scanner) {
        this.scanner = scanner;
    }

    public Compilation createCompilation() {
        System.out.print("Enter compilation title: ");
        String title = scanner.nextLine();
        Compilation compilation = new Compilation(title);
        System.out.println("Compilation created successfully!");
        return compilation;
    }

    public Compilation selectExistingCompilation() {
        System.out.println("===== Select a compilation =====");
        List<String> compilations = getCompilationsList();

        if (compilations == null || compilations.isEmpty()) {
            return null;
        }

        for (int i = 0; i < compilations.size(); i++) {
            System.out.printf("%2d. %s%n", i + 1, compilations.get(i));
        }
        System.out.println("=".repeat(32));
        System.out.print("Choose the number of compilation: ");

        int choice = getIntInput();

        if (choice < 1 || choice > compilations.size()) {
            System.out.println("Invalid number!");
            return null;
        }

        try {
            String filename = compilations.get(choice - 1);
            Compilation compilation = CompilationFileManager
                    .loadFromFile(AppConstants.COMPILATION_FOLDER + File.separator + filename);
            System.out.println("Compilation \"" + filename + "\" was successfully chosen");
            return compilation;
        } catch (IOException e) {
            System.out.println("Error while loading compilation: " + e.getMessage());
            return null;
        }
    }

    public void deleteCompilation() {
        System.out.println("===== Delete a compilation =====");
        List<String> compilations = getCompilationsList();

        if (compilations == null || compilations.isEmpty()) {
            return;
        }

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
            File fileToDelete = new File(AppConstants.COMPILATION_FOLDER, filename);
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

    public List<String> getCompilationsList() {
        File folder = new File(AppConstants.COMPILATION_FOLDER);
        if (!folder.exists()) {
            folder.mkdirs();
        }

        File[] files = folder.listFiles((dir, name) -> name.endsWith(AppConstants.FILE_EXTENSION));

        if (files == null || files.length == 0) {
            System.out.println("Folder is empty. Create a new compilation");
            return null;
        }

        List<String> names = new ArrayList<>();
        for (File f : files) {
            names.add(f.getName());
        }
        return names;
    }

    public void saveCompilation(Compilation compilation) {
        if (compilation == null) {
            System.out.println("No compilation to save!");
            return;
        }

        String filename = compilation.getTitle();
        if (!filename.endsWith(AppConstants.FILE_EXTENSION)) {
            filename += AppConstants.FILE_EXTENSION;
        }

        String fullPath = AppConstants.COMPILATION_FOLDER + File.separator + filename;

        try {
            CompilationFileManager.saveToFile(compilation, fullPath);
        } catch (Exception e) {
            System.out.println("Error saving: " + e.getMessage());
        }
    }

    private int getIntInput() {
        while (!scanner.hasNextInt()) {
            scanner.next();
            System.out.print("Please enter a number: ");
        }
        int result = scanner.nextInt();
        scanner.nextLine(); // consume newline
        return result;
    }

}
