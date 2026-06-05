#include "LoanRepository.h"
#include "../database/DbUtils.h"
#include <fstream>
#include <iostream>

// Inisialisasi: buat file CSV dengan header jika belum ada
LoanRepository::LoanRepository(const std::string& fileName)
    : fileName_(fileName) {
    std::ifstream test(fileName_);
    if (!test.good()) {
        std::ofstream create(fileName_);
        create << "loanId,bookId,memberId,borrowDate,dueDate,returned\n";
    }
}

// Cari ID terbesar lalu tambah 1
int LoanRepository::nextId() const {
    auto all = listAll();
    if (all.empty()) return 1;
    int maxId = 0;
    for (const auto& l : all) {
        if (l.getLoanId() > maxId) maxId = l.getLoanId();
    }
    return maxId + 1;
}

// Catat pinjaman baru ke akhir file CSV
void LoanRepository::save(const Loan& loan) {
    std::ofstream file(fileName_, std::ios::app);
    if (!file) { std::cerr << "Error: cannot open " << fileName_ << "\n"; return; }
    file << loan.getLoanId() << ","
         << loan.getBookId() << ","
         << loan.getMemberId() << ","
         << loan.getBorrowDate() << ","
         << loan.getDueDate() << ","
         << (loan.isReturned() ? "1" : "0") << "\n";
}

// Hapus pinjaman: tulis ulang file tanpa pinjaman yang dihapus
void LoanRepository::remove(int id) {
    auto all = listAll();
    std::ofstream file(fileName_);
    file << "loanId,bookId,memberId,borrowDate,dueDate,returned\n";
    for (const auto& l : all) {
        if (l.getLoanId() != id) {
            file << l.getLoanId() << "," << l.getBookId() << ","
                 << l.getMemberId() << "," << l.getBorrowDate() << ","
                 << l.getDueDate() << "," << (l.isReturned() ? "1" : "0") << "\n";
        }
    }
}

// Update pinjaman: tulis ulang file, ganti baris yang id-nya cocok
void LoanRepository::update(const Loan& loan) {
    auto all = listAll();
    std::ofstream file(fileName_);
    file << "loanId,bookId,memberId,borrowDate,dueDate,returned\n";
    for (const auto& l : all) {
        const Loan& target = (l.getLoanId() == loan.getLoanId()) ? loan : l;
        file << target.getLoanId() << "," << target.getBookId() << ","
             << target.getMemberId() << "," << target.getBorrowDate() << ","
             << target.getDueDate() << "," << (target.isReturned() ? "1" : "0") << "\n";
    }
}

// Cari pinjaman berdasarkan ID
std::optional<Loan> LoanRepository::findById(int id) const {
    for (const auto& l : listAll()) {
        if (l.getLoanId() == id) return l;
    }
    return std::nullopt;
}

// Baca semua pinjaman dari CSV, skip header
std::vector<Loan> LoanRepository::listAll() const {
    std::vector<Loan> loans;
    std::ifstream file(fileName_);
    if (!file) return loans;

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto t = csvSplit(line);
        if (t.size() < 6) continue;
        int loanId   = std::stoi(t[0]);
        int bookId   = std::stoi(t[1]);
        int memberId = std::stoi(t[2]);
        std::string borrowDate = t[3];
        std::string dueDate    = t[4];
        bool returned = t[5] == "1";
        loans.emplace_back(loanId, bookId, memberId, borrowDate, dueDate, returned);
    }
    return loans;
}

// Filter pinjaman yang belum dikembalikan
std::vector<Loan> LoanRepository::findActiveLoans() const {
    std::vector<Loan> result;
    for (const auto& l : listAll()) {
        if (!l.isReturned()) result.push_back(l);
    }
    return result;
}

// Filter pinjaman milik member tertentu
std::vector<Loan> LoanRepository::findByMember(int memberId) const {
    std::vector<Loan> result;
    for (const auto& l : listAll()) {
        if (l.getMemberId() == memberId) result.push_back(l);
    }
    return result;
}

// Filter pinjaman untuk buku tertentu
std::vector<Loan> LoanRepository::findByBook(int bookId) const {
    std::vector<Loan> result;
    for (const auto& l : listAll()) {
        if (l.getBookId() == bookId) result.push_back(l);
    }
    return result;
}

// Filter pinjaman yang sudah melewati batas tanggal
std::vector<Loan> LoanRepository::findOverdue() const {
    std::vector<Loan> result;
    for (const auto& l : listAll()) {
        if (l.isOverdue()) result.push_back(l);
    }
    return result;
}
