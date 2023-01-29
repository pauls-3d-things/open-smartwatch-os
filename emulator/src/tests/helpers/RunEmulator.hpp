#pragma once

#include "CaptureSerial.hpp"
#include "../../../include/Emulator.hpp"

class RunEmulator {
  public:
    enum emulatorRunResults: int { // using int, so utest can print it
        STARTING,
        RUNNING,
        STOPPED,
        FAILED
    };

    constexpr static int timeout = 10; // Seconds

    CaptureSerial capture; // Shutup the serial output
    emulatorRunResults state = emulatorRunResults::STARTING;
    std::unique_ptr<OswEmulator> oswEmu;

    RunEmulator(bool headless) {
        // Create and run the headless emulator
        oswEmu = std::make_unique<OswEmulator>(headless);
        OswEmulator::instance = oswEmu.get();
        std::thread t([&]() {
            try {
                state = emulatorRunResults::RUNNING;
                oswEmu->run();
                state = emulatorRunResults::STOPPED;
            } catch(...) {
                state = emulatorRunResults::FAILED;
            }
        });
        t.detach(); // If we time out or fail, we don't want to wait for the thread to finish
        // Wait for the thread to start
        std::chrono::time_point startWaitStarted = std::chrono::steady_clock::now();
        while(state == emulatorRunResults::STARTING and std::chrono::steady_clock::now() - startWaitStarted < std::chrono::seconds(this->timeout)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if(state != emulatorRunResults::RUNNING) {
            throw std::runtime_error("Emulator failed to start");
        }
    }

    ~RunEmulator() {
        // Stop the emulator
        oswEmu->exit();
        // Wait for the thread to finish
        std::chrono::time_point stopWaitStarted = std::chrono::steady_clock::now();
        while(state == emulatorRunResults::RUNNING and std::chrono::steady_clock::now() - stopWaitStarted < std::chrono::seconds(this->timeout)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        if(state == emulatorRunResults::RUNNING) {
            std::cerr << "Emulator stopped with state " << state << " - it is likely still running!" << std::endl;
            std::abort();
        }
        OswEmulator::instance = nullptr;
    }
};