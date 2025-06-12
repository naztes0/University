#include <gtest/gtest.h>
#include "Observer.h"
#include "ConcreteObservers.h"
#include <memory>
#include <sstream>
#include <iostream>

// Mock subject for testing
class MockAlgorithmSubject : public AlgorithmSubject {
public:
    void triggerStart(const std::string& name) {
        notifyAlgorithmStart(name);
    }

    void triggerFinish(const std::string& name, double time) {
        notifyAlgorithmFinish(name, time);
    }

    void triggerProgress(const std::string& name, int current, int total) {
        notifyProgressUpdate(name, current, total);
    }

    void triggerNegativeCycle() {
        notifyNegativeCycleDetected();
    }

    void triggerStep(const std::string& step) {
        notifyStepCompleted(step);
    }
};

class ObserverTest : public ::testing::Test {
protected:
    void SetUp() override {
        subject = std::make_unique<MockAlgorithmSubject>();

        // Redirect cout to capture output
        originalCoutBuffer = std::cout.rdbuf();
        std::cout.rdbuf(capturedOutput.rdbuf());
    }

    void TearDown() override {
        // Restore cout
        std::cout.rdbuf(originalCoutBuffer);
    }

    std::string getCapturedOutput() {
        return capturedOutput.str();
    }

    void clearOutput() {
        capturedOutput.str("");
        capturedOutput.clear();
    }

    std::unique_ptr<MockAlgorithmSubject> subject;
    std::ostringstream capturedOutput;
    std::streambuf* originalCoutBuffer;
};

TEST_F(ObserverTest, ConsoleObserverBasicTest) {
    auto observer = std::make_shared<ConsoleObserver>();
    subject->addObserver(observer);

    subject->triggerStart("Test Algorithm");

    std::string output = getCapturedOutput();
    EXPECT_TRUE(output.find("Starting Test Algorithm") != std::string::npos);
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);
}

TEST_F(ObserverTest, ConsoleObserverAllEventsTest) {
    auto observer = std::make_shared<ConsoleObserver>();
    subject->addObserver(observer);

    subject->triggerStart("Test");
    subject->triggerProgress("Test", 50, 100);
    subject->triggerStep("Step 1");
    subject->triggerNegativeCycle();
    subject->triggerFinish("Test", 123.45);

    std::string output = getCapturedOutput();
    EXPECT_TRUE(output.find("Starting Test") != std::string::npos);
    EXPECT_TRUE(output.find("50%") != std::string::npos);
    EXPECT_TRUE(output.find("Step 1") != std::string::npos);
    EXPECT_TRUE(output.find("Negative cycle") != std::string::npos);
    EXPECT_TRUE(output.find("123.45 ms") != std::string::npos);
}

TEST_F(ObserverTest, ProgressBarObserverBasicTest) {
    auto observer = std::make_shared<ProgressBarObserver>();
    subject->addObserver(observer);

    subject->triggerStart("Progress Test");

    std::string output = getCapturedOutput();
    EXPECT_TRUE(output.find("Progress Test Progress:") != std::string::npos);
    EXPECT_TRUE(output.find("0%   25%   50%   75%   100%") != std::string::npos);
}

TEST_F(ObserverTest, ProgressBarObserverProgressTest) {
    auto observer = std::make_shared<ProgressBarObserver>();
    subject->addObserver(observer);

    subject->triggerStart("Progress Test");
    clearOutput();

    // Trigger progress updates
    subject->triggerProgress("Progress Test", 25, 100);
    subject->triggerProgress("Progress Test", 50, 100);
    subject->triggerProgress("Progress Test", 75, 100);

    std::string output = getCapturedOutput();
    // Should contain progress bars (=)
    EXPECT_TRUE(output.find("=") != std::string::npos);
}

TEST_F(ObserverTest, MultipleObserversTest) {
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    auto progressObserver = std::make_shared<ProgressBarObserver>();

    subject->addObserver(consoleObserver);
    subject->addObserver(progressObserver);

    subject->triggerStart("Multi Test");

    std::string output = getCapturedOutput();
    // Should contain output from both observers
    EXPECT_TRUE(output.find("[INFO]") != std::string::npos);
    EXPECT_TRUE(output.find("Progress:") != std::string::npos);
}

TEST_F(ObserverTest, RemoveObserverTest) {
    auto observer = std::make_shared<ConsoleObserver>();
    subject->addObserver(observer);

    subject->triggerStart("Before Remove");
    clearOutput();

    subject->removeObserver(observer);
    subject->triggerStart("After Remove");

    std::string output = getCapturedOutput();
    // Should be empty after observer removal
    EXPECT_TRUE(output.empty() || output.find("After Remove") == std::string::npos);
}

TEST_F(ObserverTest, ZeroProgressTest) {
    auto observer = std::make_shared<ConsoleObserver>();
    subject->addObserver(observer);

    // Test with zero total (edge case)
    subject->triggerProgress("Test", 5, 0);

    // Should not crash
    EXPECT_NO_THROW(subject->triggerProgress("Test", 5, 0));
}