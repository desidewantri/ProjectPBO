#include "User.h"
#include <iostream>

User::User(const std::string& username, const std::string& passwordHash)
    : username_(username), passwordHash_(passwordHash) {}

// Login: bandingkan input dengan hash yang tersimpan
bool User::login(const std::string& inputPassword) const {
    return inputPassword == passwordHash_;
}

void User::logout() {
    // Bisa dikembangkan untuk session management
}

std::string User::getUsername() const { return username_; }
std::string User::getPasswordHash() const { return passwordHash_; }

// Dua user dianggap sama jika username-nya sama
bool User::operator==(const User& other) const {
    return username_ == other.username_;
}

// Tampilkan role dan username ke stream
std::ostream& operator<<(std::ostream& os, const User& u) {
    os << "[" << u.role() << "] " << u.username_;
    return os;
}
