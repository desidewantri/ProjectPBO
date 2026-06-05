#include "Loan.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

Loan::Loan(int loanId, int bookId, int memberId,
           const std::string& borrowDate, const std::string& dueDate,
           bool returned)
    : loanId_(loanId), bookId_(bookId), memberId_(memberId),
      borrowDate_(borrowDate), dueDate_(dueDate), returned_(returned) {}

int Loan::getLoanId()   const { return loanId_; }
int Loan::getBookId()   const { return bookId_; }
int Loan::getMemberId() const { return memberId_; }
std::string Loan::getBorrowDate() const { return borrowDate_; }
std::string Loan::getDueDate()    const { return dueDate_; }
bool Loan::isReturned() const { return returned_; }
void Loan::setReturned(bool r) { returned_ = r; }

// Cek keterlambatan dengan membandingkan dueDate_ dan tanggal hari ini
bool Loan::isOverdue() const {
    if (returned_) return false;  // sudah dikembalikan, tidak terlambat

    time_t now = time(nullptr);
    tm* today = localtime(&now);

    int ty = today->tm_year + 1900;
    int tm_ = today->tm_mon + 1;
    int td = today->tm_mday;

    // Parse format YYYY-MM-DD dari dueDate_
    int dy, dm, dd;
    sscanf(dueDate_.c_str(), "%d-%d-%d", &dy, &dm, &dd);

    if (ty > dy) return true;
    if (ty == dy && tm_ > dm) return true;
    if (ty == dy && tm_ == dm && td > dd) return true;
    return false;
}

void Loan::returnBook() {
    returned_ = true;
}

// Dua loan sama jika loanId-nya sama
bool Loan::operator==(const Loan& other) const {
    return loanId_ == other.loanId_;
}

// Tampilkan data loan dalam format kolom untuk tabel CLI
std::ostream& operator<<(std::ostream& os, const Loan& l) {
    os << std::left
       << std::setw(6)  << l.loanId_
       << std::setw(8)  << l.bookId_
       << std::setw(10) << l.memberId_
       << std::setw(14) << l.borrowDate_
       << std::setw(14) << l.dueDate_
       << (l.returned_ ? "Returned" : (l.isOverdue() ? "OVERDUE" : "Active"));
    return os;
}
