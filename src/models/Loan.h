#pragma once
#include <string>
#include <iostream>

class Loan {
private:
    int loanId_;
    int bookId_;
    int memberId_;
    std::string borrowDate_;
    std::string dueDate_;
    bool returned_;

public:
    Loan(int loanId, int bookId, int memberId,
         const std::string& borrowDate, const std::string& dueDate,
         bool returned = false);

    int getLoanId() const;
    int getBookId() const;
    int getMemberId() const;
    std::string getBorrowDate() const;
    std::string getDueDate() const;
    bool isReturned() const;
    void setReturned(bool r);

    bool isOverdue() const;
    void returnBook();

    bool operator==(const Loan& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Loan& l);
};
