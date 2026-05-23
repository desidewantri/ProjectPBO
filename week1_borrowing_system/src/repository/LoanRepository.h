#pragma once
#include "../models/Loan.h"
#include <vector>
#include <string>
#include <optional>

class LoanRepository {
private:
    std::string fileName_;

public:
    explicit LoanRepository(const std::string& fileName = "data/loans.csv");

    void save(const Loan& loan);
    void remove(int id);
    void update(const Loan& loan);
    std::optional<Loan> findById(int id) const;
    std::vector<Loan> listAll() const;
    std::vector<Loan> findActiveLoans() const;
    std::vector<Loan> findByMember(int memberId) const;
    std::vector<Loan> findByBook(int bookId) const;
    std::vector<Loan> findOverdue() const;

    int nextId() const;
};
