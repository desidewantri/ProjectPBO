#pragma once
#include <string>
#include <iostream>

// Kelas Book merepresentasikan satu buku dalam sistem perpustakaan
class Book {
private:
    int id_;              // ID unik buku
    std::string title_;   // judul buku
    std::string author_;  // nama pengarang
    bool available_;      // true = tersedia, false = sedang dipinjam

public:
    Book(int id, const std::string& title, const std::string& author, bool available = true);

    // Getter
    int getId() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    bool isAvailable() const;

    // Setter untuk mengubah status ketersediaan
    void setAvailable(bool a);

    // Tampilkan buku dalam format tabel
    void display() const;

    // Operator overloading: dua buku sama jika id-nya sama
    bool operator==(const Book& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Book& b);
};
