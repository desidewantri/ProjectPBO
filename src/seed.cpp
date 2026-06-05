// seed.cpp — Script untuk mengisi data awal ke database CSV
// Jalankan sekali sebelum menggunakan admin atau web
// Jika data sudah ada, script ini akan skip tanpa mengubah data

#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>

#include "models/Book.h"
#include "models/Member.h"
#include "models/Admin.h"
#include "models/Loan.h"
#include "repository/BookRepository.h"
#include "repository/MemberRepository.h"
#include "repository/LoanRepository.h"
#include "database/DbUtils.h"

// Simple "hash" for demo (Week 3 bonus: replace with SHA-256)
std::string simpleHash(const std::string& s) {
    size_t h = 0;
    for (char c : s) h = h * 31 + c;
    return std::to_string(h);
}

void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

int main() {
    // Create data directory
    std::filesystem::create_directories("data");

    BookRepository   bookRepo("data/books.csv");
    MemberRepository memberRepo("data/members.csv");
    LoanRepository   loanRepo("data/loans.csv");

    //SEED BOOKS (10 records) 
    printSeparator("Seeding Books");

    if (bookRepo.listAll().empty()) {
        std::vector<std::pair<std::string,std::string>> books = {
            {"Clean Code",                    "Robert C. Martin"},
            {"The C++ Standard Library",      "Nicolai Josuttis"},
            {"Algorithms (4th ed.)",           "Sedgewick & Wayne"},
            {"Effective Modern C++",           "Scott Meyers"},
            {"Design Patterns",                "Gang of Four"},
            {"The Pragmatic Programmer",       "Hunt & Thomas"},
            {"Introduction to Algorithms",     "CLRS"},
            {"Operating System Concepts",      "Silberschatz"},
            {"Computer Networks",              "Tanenbaum"},
            {"Embedded Systems with C++",      "Stroustrup"}
        };

        for (const auto& [title, author] : books) {
            int id = bookRepo.nextId();
            Book b(id, title, author, true);
            bookRepo.save(b);
            std::cout << "  + Book #" << id << ": " << title << "\n";
        }
    } else {
        std::cout << "  Books already seeded, skipping.\n";
    }

    // SEED MEMBERS (5 records)
    printSeparator("Seeding Members");

    if (memberRepo.listAll().empty()) {
        struct MemberData { std::string name, email, uname, pw; };
        std::vector<MemberData> members = {
            {"Budi Santoso",    "budi@ugm.ac.id",    "budi",    "budi123"},
            {"Siti Rahayu",     "siti@ugm.ac.id",    "siti",    "siti123"},
            {"Andi Wijaya",     "andi@ugm.ac.id",    "andi",    "andi123"},
            {"Dewi Lestari",    "dewi@ugm.ac.id",    "dewi",    "dewi123"},
            {"Rizky Pratama",   "rizky@ugm.ac.id",   "rizky",   "rizky123"}
        };

        for (const auto& m : members) {
            int id = memberRepo.nextId();
            Member mem(id, m.name, m.email, m.uname, simpleHash(m.pw));
            memberRepo.save(mem);
            std::cout << "  + Member #" << id << ": " << m.name << "\n";
        }
    } else {
        std::cout << "  Members already seeded, skipping.\n";
    }

    // SEED LOANS (3 sample loans)
    printSeparator("Seeding Loans");

    if (loanRepo.listAll().empty()) {
        std::string today = todayDate();
        std::string due14 = dueDateFromToday(14);
        std::string due7  = dueDateFromToday(7);
        std::string overdueDate = "2026-04-01";  // deliberately overdue

        std::vector<Loan> loans = {
            {1, 1, 1, today,       due14,       false},  // Budi borrows Clean Code
            {2, 2, 2, today,       due7,        false},  // Siti borrows C++ Std Lib
            {3, 3, 3, "2026-04-20", overdueDate, false}, // Andi overdue
        };

        // Mark borrowed books as unavailable
        for (const auto& loan : loans) {
            loanRepo.save(loan);
            auto book = bookRepo.findById(loan.getBookId());
            if (book) {
                Book updated = *book;
                updated.setAvailable(false);
                bookRepo.update(updated);
            }
            std::cout << "  + Loan #" << loan.getLoanId()
                      << ": Book #" << loan.getBookId()
                      << " → Member #" << loan.getMemberId()
                      << " (due: " << loan.getDueDate() << ")"
                      << (loan.isOverdue() ? " [OVERDUE]" : "")
                      << "\n";
        }
    } else {
        std::cout << "  Loans already seeded, skipping.\n";
    }

    // Tampilkan ringkasan data yang tersimpan untuk verifikasi
    printSeparator("Verification — All Books");
    std::cout << std::left
              << std::setw(4)  << "ID"
              << std::setw(35) << "Title"
              << std::setw(25) << "Author"
              << "Status\n"
              << std::string(70, '-') << "\n";
    for (const auto& b : bookRepo.listAll()) b.display();

    printSeparator("Verification — All Members");
    std::cout << std::left
              << std::setw(6)  << "ID"
              << std::setw(25) << "Name"
              << std::setw(30) << "Email"
              << "Username\n"
              << std::string(70, '-') << "\n";
    for (const auto& m : memberRepo.listAll()) {
        std::cout << m << "\n";
    }

    printSeparator("Verification — Active Loans");
    std::cout << std::left
              << std::setw(6)  << "LoanID"
              << std::setw(8)  << "BookID"
              << std::setw(10) << "MemberID"
              << std::setw(14) << "Borrowed"
              << std::setw(14) << "Due"
              << "Status\n"
              << std::string(70, '-') << "\n";
    for (const auto& l : loanRepo.listAll()) {
        std::cout << l << "\n";
    }

    // Demo konsep OOP yang diimplementasikan dalam proyek ini
    printSeparator("OOP Concepts Demo");

    // Polymorphism: User* pointing to Admin
    Admin admin("admin", simpleHash("admin123"));
    std::cout << "Admin via User ref: " << admin << "\n";
    std::cout << "Login test (correct pw): "
              << (admin.login(simpleHash("admin123")) ? "OK" : "FAIL") << "\n";
    std::cout << "Login test (wrong pw):   "
              << (admin.login(simpleHash("wrong")) ? "OK" : "FAIL") << "\n";

    // operator== on Book
    Book b1 = *bookRepo.findById(1);
    Book b2 = *bookRepo.findById(1);
    Book b3 = *bookRepo.findById(2);
    std::cout << "\nBook #1 == Book #1: " << (b1 == b2 ? "true" : "false") << "\n";
    std::cout << "Book #1 == Book #2: " << (b1 == b3 ? "true" : "false") << "\n";

    // isOverdue
    auto overdueLoan = loanRepo.findById(3);
    if (overdueLoan) {
        std::cout << "\nLoan #3 isOverdue: "
                  << (overdueLoan->isOverdue() ? "YES" : "no") << "\n";
    }

    std::cout << "\n✓ Seed complete. Data persisted in data/ directory.\n\n";
    return 0;
}
