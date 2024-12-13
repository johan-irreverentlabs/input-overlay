/*************************************************************************
 * This file is part of input-overlay
 * git.vrsal.xyz/alex/input-overlay
 * Copyright 2023 univrsal <uni@vrsal.xyz>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/

#pragma once
#include <obs-module.h>

#include <filesystem>
#include <chrono>
#include <iomanip>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include <fstream>
#include <queue>
#include <thread>
#include <atomic>
#include <sstream>
#include <string>

extern int recording_time_ms();

class Logger {
public:
    Logger() : done(false) {}

    void startLogging(const std::string& folder) {
        done = false;
        std::filesystem::create_directories(folder); // Create the folder if it does not exist
        std::string filename = generateLogFilename(folder);
        logFile.open(filename, std::ios::app);
        logThread = std::thread(&Logger::logWriter, this);
    }

    void stopLogging() {
        done = true;
        if (logThread.joinable()) {
            logThread.join();
        }
        if (logFile.is_open()) {
            logFile.close();
        }
        clearLogQueue();
    }

    void logMessage(const std::string& key, const std::string& value) {
        int recording_time = recording_time_ms(); \
        if (recording_time < 0) {
            return;
        }

        std::stringstream messageStream;
        messageStream << "{ \"time\": " << recording_time << ", \"" << key << "\": \"" << value << "\" }";

        {
            std::lock_guard<std::mutex> lock(logMutex);
            logQueue.push(messageStream.str());
        }
    }

    void clearLogQueue() {
        std::lock_guard<std::mutex> lock(logMutex);
        std::queue<std::string> empty;
        std::swap(logQueue, empty);
    }

private:
    std::ofstream logFile;
    std::queue<std::string> logQueue;
    std::mutex logMutex;
    std::atomic<bool> done;
    std::thread logThread;

    void logWriter() {
        while (!done || !logQueue.empty()) {
            std::unique_lock<std::mutex> lock(logMutex);
            while (!logQueue.empty()) {
                logFile << logQueue.front() << std::endl;
                logQueue.pop();
            }
            lock.unlock();
            logFile.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Adjust the sleep duration as needed
        }
    }

    std::string generateLogFilename(const std::string& folder) {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%H-%M-%S") << ".log";
        return (std::filesystem::path(folder) / ss.str()).string();
    }
};


#define write_log(log_level, format, ...) blog(log_level, "[input-overlay] " format, ##__VA_ARGS__)
#define write_input_event(log_level, format, ...) blog(log_level, "[input-event] " format, ##__VA_ARGS__)
#define write_timed_input_event(format, ...) blog(log_level, "[input-event] " format, ##__VA_ARGS__)
#define bdebug(format, ...) write_log(LOG_DEBUG, format, ##__VA_ARGS__)
#define binfo(format, ...) write_log(LOG_INFO, format, ##__VA_ARGS__)
// #define binput_event(format, ...) \
//     do { \
//         int recording_time = recording_time_ms(); \
//         if (recording_time > 0) { \
//             blog(LOG_INFO, "[input-event]{ time: %d, " format, recording_time, ##__VA_ARGS__); \
//         } \
//     } while (0)
#define bwarn(format, ...) write_log(LOG_WARNING, format, ##__VA_ARGS__)
#define berr(format, ...) write_log(LOG_ERROR, format, ##__VA_ARGS__)
