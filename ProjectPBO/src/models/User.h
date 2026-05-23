#pragma once
#include <string>

class User {
protected:
    std::string username_;
    std::string passwordHash_;

public:
    User(const std::string& username, const std::string& passwordHash);
    virtual ~User() = default;

    bool login(const std::string& inputPassword) const;
    void logout();
    std::string getUsername() const;
    std::string getPasswordHash() const;

    virtual std::string role() const = 0;

    // Operator overloading
    bool operator==(const User& other) const;
    friend std::ostream& operator<<(std::ostream& os, const User& u);
};
