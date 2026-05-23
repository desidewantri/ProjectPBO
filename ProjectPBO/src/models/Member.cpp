#include "Member.h"
#include <iostream>
#include <iomanip>

Member::Member(int memberId, const std::string& name, const std::string& email,
               const std::string& username, const std::string& passwordHash)
    : User(username, passwordHash),
      memberId_(memberId), name_(name), email_(email) {}

std::string Member::role() const { return "Member"; }

int Member::getMemberId() const { return memberId_; }
std::string Member::getName() const { return name_; }
std::string Member::getEmail() const { return email_; }

bool Member::borrowBook(int bookId) {
    // Logic will be handled by core engine via LoanRepository
    std::cout << "[Member:" << name_ << "] borrowBook(" << bookId << ") called\n";
    return true;
}

bool Member::returnBook(int bookId) {
    std::cout << "[Member:" << name_ << "] returnBook(" << bookId << ") called\n";
    return true;
}

void Member::viewLoans() const {
    std::cout << "[Member:" << name_ << "] viewLoans() called\n";
}

std::ostream& operator<<(std::ostream& os, const Member& m) {
    os << std::left
       << std::setw(6)  << m.memberId_
       << std::setw(25) << m.name_
       << std::setw(30) << m.email_
       << m.username_;
    return os;
}
