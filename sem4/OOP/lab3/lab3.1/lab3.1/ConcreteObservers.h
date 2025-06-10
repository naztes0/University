#pragma once
#include "Observer.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

// Console logger observer
class ConsoleObserver : public AlgorithmObserver {
public:
    void onAlgorithmStart(const std::string& algorithmName) override {
        std::cout << "[INFO] Starting " << algorithmName << " algorithm..." << std::endl;
    }

    void onAlgorithmFinish(const std::string& algorithmName, double executionTime) override {
        std::cout << "[INFO] " << algorithmName << " completed in "
            << std::fixed << std::setprecision(2) << executionTime << " ms" << std::endl;
    }

    void onProgressUpdate(const std::string& algorithmName, int current, int total) override {
        if (total > 0) {
            int percentage = (current * 100) / total;
            std::cout << "[PROGRESS] " << algorithmName << ": " << percentage
                << "% (" << current << "/" << total << ")" << std::endl;
        }
    }

    void onNegativeCycleDetected() override {
        std::cout << "[WARNING] Negative cycle detected in graph!" << std::endl;
    }

    void onStepCompleted(const std::string& stepName) override {
        std::cout << "[STEP] Completed: " << stepName << std::endl;
    }
};

// Progress bar observer
class ProgressBarObserver : public AlgorithmObserver {
private:
    int lastPercentage = -1;

public:
    void onAlgorithmStart(const std::string& algorithmName) override {
        lastPercentage = -1;
        std::cout << "\n" << algorithmName << " Progress:\n";
        std::cout << "0%   25%   50%   75%   100%\n";
        std::cout << "|----|----|----|----|----|\n";
    }

    void onAlgorithmFinish(const std::string& algorithmName, double executionTime) override {
        if (lastPercentage < 100) {
            // Fill remaining progress bar
            for (int i = lastPercentage + 1; i <= 100; i += 5) {
                std::cout << "=";
            }
        }
        std::cout << "| DONE (" << executionTime << " ms)\n" << std::endl;
    }

    void onProgressUpdate(const std::string& algorithmName, int current, int total) override {
        if (total > 0) {
            int percentage = (current * 100) / total;
            // Update progress bar every 5%
            if (percentage > lastPercentage && percentage % 5 == 0) {
                for (int i = lastPercentage + 1; i <= percentage; i += 5) {
                    std::cout << "=" << std::flush;
                }
                lastPercentage = percentage;
            }
        }
    }

    void onNegativeCycleDetected() override {
        std::cout << "\n[!] Negative cycle detected!\n";
    }

    void onStepCompleted(const std::string& stepName) override {
        // Silent for progress bar
    }
};