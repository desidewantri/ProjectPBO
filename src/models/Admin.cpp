#include "Admin.h"
#include <iostream>

Admin::Admin(const std::string& username, const std::string& passwordHash)
    : User(username, passwordHash) {}

// Mengembalikan role sebagai string "Admin"
std::string Admin::role() const { return "Admin"; }

// Method berikut adalah placeholder — logika sebenarnya ada di admin.cpp
void Admin::addBook() const {
    std::cout << "[Admin] addBook() called\n";
}

void Admin::editBook() const {
    std::cout << "[Admin] editBook() called\n";
}

void Admin::deleteBook() const {
    std::cout << "[Admin] deleteBook() called\n";
}

void Admin::listMembers() const {
    std::cout << "[Admin] listMembers() called\n";
}
