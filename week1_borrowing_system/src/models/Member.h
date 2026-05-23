#pragma once
#include "User.h"

class Member : public User {
private:
    int memberId_;
    std::string name_;
    std::string email_;

public:
    Member(int memberId, const std::string& name, const std::string& email,
           const std::string& username, const std::string& passwordHash);

    std::string role() const override;

    int getMemberId() const;
    std::string getName() const;
    std::string getEmail() const;

    bool borrowBook(int bookId);
    bool returnBook(int bookId);
    void viewLoans() const;

    friend std::ostream& operator<<(std::ostream& os, const Member& m);
};
