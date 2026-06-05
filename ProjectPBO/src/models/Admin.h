#pragma once
#include "User.h"

class Admin : public User {
public:
    Admin(const std::string& username, const std::string& passwordHash);

    std::string role() const override;

    void addBook() const;
    void editBook() const;
    void deleteBook() const;
    void listMembers() const;
};
