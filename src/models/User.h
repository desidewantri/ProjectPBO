#pragma once
#include <string>

// Kelas dasar untuk semua pengguna sistem (Admin dan Member)
// Menggunakan abstract class dengan pure virtual method role()
class User {
protected:
    std::string username_;      // username untuk login
    std::string passwordHash_;  // password yang sudah di-hash

public:
    User(const std::string& username, const std::string& passwordHash);
    virtual ~User() = default;

    // Cek apakah password yang dimasukkan cocok dengan hash
    bool login(const std::string& inputPassword) const;
    void logout();

    std::string getUsername() const;
    std::string getPasswordHash() const;

    // Pure virtual — setiap subclass wajib implementasi role()
    virtual std::string role() const = 0;

    // Operator overloading: bandingkan user berdasarkan username
    bool operator==(const User& other) const;
    friend std::ostream& operator<<(std::ostream& os, const User& u);
};
