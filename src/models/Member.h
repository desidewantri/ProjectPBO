#pragma once
#include "User.h"

// Member adalah pengguna yang bisa meminjam buku
// Mewarisi User dan menambahkan data: id, nama, email
class Member : public User {
private:
    int memberId_;        // ID unik member
    std::string name_;    // nama lengkap
    std::string email_;   // email member

public:
    Member(int memberId, const std::string& name, const std::string& email,
           const std::string& username, const std::string& passwordHash);

    // Override role() dari User
    std::string role() const override;

    int getMemberId() const;
    std::string getName() const;
    std::string getEmail() const;

    // Logika pinjam/kembali dikelola LoanRepository, ini hanya stub
    bool borrowBook(int bookId);
    bool returnBook(int bookId);
    void viewLoans() const;

    // Output stream: tampilkan data member dalam format tabel
    friend std::ostream& operator<<(std::ostream& os, const Member& m);
};
