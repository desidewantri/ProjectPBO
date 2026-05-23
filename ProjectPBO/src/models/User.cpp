#include "User.h"
#include <iostream>

User::User(const std::string& username, const std::string& passwordHash)
    : username_(username), passwordHash_(passwordHash) {}

bool User::login(const std::string& inputPassword) const {
    return inputPassword == passwordHash_; // simple hash check
}

void User::logout() {
    // Can be extended for session management
}

std::string User::getUsername() const { return username_; }
std::string User::getPasswordHash() const { return passwordHash_; }

bool User::operator==(const User& other) const {
    return username_ == other.username_;
}

std::ostream& operator<<(std::ostream& os, const User& u) {
    os << "[" << u.role() << "] " << u.username_;
    return os;
}
