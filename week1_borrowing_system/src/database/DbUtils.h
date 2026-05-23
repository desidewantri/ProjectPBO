#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

// Simple CSV helper: split a line by comma
inline std::vector<std::string> csvSplit(const std::string& line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}

// Escape commas in a field
inline std::string csvEscape(const std::string& s) {
    // Wrap in quotes if contains comma
    if (s.find(',') != std::string::npos) {
        return "\"" + s + "\"";
    }
    return s;
}

// Format tm* to YYYY-MM-DD string
inline std::string formatDate(const tm* t) {
    std::ostringstream oss;
    oss << (t->tm_year + 1900) << "-"
        << std::setfill('0') << std::setw(2) << (t->tm_mon + 1) << "-"
        << std::setfill('0') << std::setw(2) << t->tm_mday;
    return oss.str();
}

// Get today's date as YYYY-MM-DD
inline std::string todayDate() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    return formatDate(t);
}

// Get due date N days from today
inline std::string dueDateFromToday(int days = 14) {
    time_t now = time(nullptr);
    now += static_cast<time_t>(days) * 86400LL;
    tm* t = localtime(&now);
    return formatDate(t);
}
