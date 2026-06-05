#pragma once
#include "../models/Loan.h"
#include <vector>
#include <string>
#include <optional>

// LoanRepository mengelola operasi CRUD peminjaman ke file loans.csv
class LoanRepository {
private:
    std::string fileName_;  // path ke file CSV

public:
    explicit LoanRepository(const std::string& fileName = "data/loans.csv");

    void save(const Loan& loan);                   // catat pinjaman baru
    void remove(int id);                           // hapus data pinjaman
    void update(const Loan& loan);                 // update status pinjaman
    std::optional<Loan> findById(int id) const;   // cari pinjaman by ID
    std::vector<Loan> listAll() const;             // semua pinjaman
    std::vector<Loan> findActiveLoans() const;     // pinjaman yang belum dikembalikan
    std::vector<Loan> findByMember(int memberId) const;  // riwayat pinjaman member
    std::vector<Loan> findByBook(int bookId) const;      // pinjaman per buku
    std::vector<Loan> findOverdue() const;         // pinjaman yang terlambat

    int nextId() const;
};
