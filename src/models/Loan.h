#pragma once
#include <string>
#include <iostream>

// Kelas Loan merepresentasikan satu transaksi peminjaman buku
// Composition: Loan "memiliki" bookId dan memberId
class Loan {
private:
    int loanId_;              // ID unik peminjaman
    int bookId_;              // ID buku yang dipinjam
    int memberId_;            // ID member yang meminjam
    std::string borrowDate_;  // tanggal pinjam (YYYY-MM-DD)
    std::string dueDate_;     // batas pengembalian (YYYY-MM-DD)
    bool returned_;           // true = sudah dikembalikan

public:
    Loan(int loanId, int bookId, int memberId,
         const std::string& borrowDate, const std::string& dueDate,
         bool returned = false);

    // Getter
    int getLoanId()   const;
    int getBookId()   const;
    int getMemberId() const;
    std::string getBorrowDate() const;
    std::string getDueDate()    const;
    bool isReturned() const;

    // Setter untuk menandai buku sudah dikembalikan
    void setReturned(bool r);

    // Cek apakah pinjaman sudah melewati batas tanggal
    bool isOverdue() const;
    void returnBook();

    // Operator overloading: dua loan sama jika loanId-nya sama
    bool operator==(const Loan& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Loan& l);
};
