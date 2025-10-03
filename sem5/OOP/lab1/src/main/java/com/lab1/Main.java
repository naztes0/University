package com.lab1;

import com.lab1.model.Instrumental;
import com.lab1.model.MusicComposition;
import com.lab1.model.Song;
import com.lab1.model.Instrumental;
import com.lab1.model.Remix;
import java.util.ArrayList;
import java.util.List;
import com.lab1.controller.Compilation;

public class Main {
    public static void main(String[] args) {
        Song track1 = new Song("Shiver", "Hanover", 215, "Alternative Metal", 2022);
        Song track2 = new Song("Be Quiet and Drive", "Deftones", 312, "Alternative Metal", 1997);
        Song track3 = new Song("Silver", "Deftones", 262, "Alternative Metal", 2000);
        Song track4 = new Song("Ignite", "Icepeak", 198, "Industrial Rock", 2021);
        Song track5 = new Song("Bites", "MSI", 193, "Electropunk", 2005);
        Song track6 = new Song("Gore", "Icepeak", 224, "Experimental Rock", 2020);
        Song track7 = new Song("My Songs Know What You Did In The Dark", "MSI", 202, "Industrial Rock", 2006);
        Song track8 = new Song("Digital Bath", "Deftones", 269, "Alternative Metal", 2000);
        Song track9 = new Song("Collapse", "Hanover", 241, "Nu Metal", 2023);
        Song track10 = new Song("Shut Me Up", "MSI", 195, "Electropunk", 2005);
        Instrumental track11 = new Instrumental("Adagio for Strings", "Samuel Barber", 600, "Classical", 1938,
                "Orchestra");

        MusicComposition[] tracks = {
                track1, track2, track3, track4, track5,
                track6, track7, track8, track9, track10, track11
        };

        Compilation playlist = new Compilation("Blablabla");
        for (int i = 0; i < tracks.length; i++) {
            playlist.addComposition(tracks[i]);
        }
        playlist.printPlaylist();
    }
}