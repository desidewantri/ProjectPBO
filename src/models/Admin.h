#pragma once
#include "User.h"

// Admin mewarisi User — memiliki akses penuh ke manajemen sistem
class Admin : public User {
public:
    Admin(const std::string& username, const std::string& passwordHash);

    // Override role() dari User
    std::string role() const override;

    // Method placeholder untuk operasi admin (diimplementasi di CLI)
    void addBook() const;
    void editBook() const;
    void deleteBook() const;
    void listMembers() const;
};
