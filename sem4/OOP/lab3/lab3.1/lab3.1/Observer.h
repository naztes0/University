#pragma once
#include <vector>
#include <memory>
#include <string>


/**
 * @file Observer.h
 * @brief Observer pattern implementation for algorithm monitoring
 */

 /**
  * @class AlgorithmObserver
  * @brief Abstract observer interface for algorithm execution events
  */
// Observer interface
class AlgorithmObserver {
public:
    virtual ~AlgorithmObserver() = default;

    // Events for algorithm execution
    virtual void onAlgorithmStart(const std::string& algorithmName) = 0;
    virtual void onAlgorithmFinish(const std::string& algorithmName, double executionTime) = 0;
    virtual void onProgressUpdate(const std::string& algorithmName, int current, int total) = 0;
    virtual void onNegativeCycleDetected() = 0;
    virtual void onStepCompleted(const std::string& stepName) = 0;
};


/**
 * @class AlgorithmSubject
 * @brief Subject class for observer pattern implementation
 */
// Subject interface
class AlgorithmSubject {
private:
    std::vector<std::shared_ptr<AlgorithmObserver>> observers;

public:
    virtual ~AlgorithmSubject() = default;

    void addObserver(std::shared_ptr<AlgorithmObserver> observer) {
        observers.push_back(observer);
    }

    void removeObserver(std::shared_ptr<AlgorithmObserver> observer) {
        observers.erase(
            std::remove_if(observers.begin(), observers.end(),
                [&observer](const std::weak_ptr<AlgorithmObserver>& obs) {
                    return obs.lock() == observer;
                }),
            observers.end()
        );
    }

protected:
    void notifyAlgorithmStart(const std::string& algorithmName) {
        for (auto& observer : observers) {
            if (observer) observer->onAlgorithmStart(algorithmName);
        }
    }

    void notifyAlgorithmFinish(const std::string& algorithmName, double executionTime) {
        for (auto& observer : observers) {
            if (observer) observer->onAlgorithmFinish(algorithmName, executionTime);
        }
    }

    void notifyProgressUpdate(const std::string& algorithmName, int current, int total) {
        for (auto& observer : observers) {
            if (observer) observer->onProgressUpdate(algorithmName, current, total);
        }
    }

    void notifyNegativeCycleDetected() {
        for (auto& observer : observers) {
            if (observer) observer->onNegativeCycleDetected();
        }
    }

    void notifyStepCompleted(const std::string& stepName) {
        for (auto& observer : observers) {
            if (observer) observer->onStepCompleted(stepName);
        }
    }
};