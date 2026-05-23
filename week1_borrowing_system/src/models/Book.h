#pragma once
#include <string>
#include <iostream>

class Book {
private:
    int id_;
    std::string title_;
    std::string author_;
    bool available_;

public:
    Book(int id, const std::string& title, const std::string& author, bool available = true);

    int getId() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    bool isAvailable() const;
    void setAvailable(bool a);

    void display() const;

    // Operator overloading
    bool operator==(const Book& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Book& b);
};
