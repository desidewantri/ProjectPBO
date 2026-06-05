#pragma once
#include "../models/Book.h"
#include <vector>
#include <string>
#include <optional>

// BookRepository mengelola operasi CRUD buku ke file books.csv
// Menggunakan std::optional untuk hasil pencarian yang mungkin tidak ada
class BookRepository {
private:
    std::string fileName_;  // path ke file CSV

    int getNextId() const;

public:
    explicit BookRepository(const std::string& fileName = "data/books.csv");

    void save(const Book& book);              // tambah buku baru
    void remove(int id);                      // hapus buku berdasarkan ID
    void update(const Book& book);            // perbarui data buku
    std::optional<Book> findById(int id) const;  // cari buku, return nullopt jika tidak ada
    std::vector<Book> listAll() const;        // ambil semua buku
    std::vector<Book> findAvailable() const;  // hanya buku yang tersedia
    std::vector<Book> search(const std::string& keyword) const;  // cari by judul/pengarang

    // Generate ID berikutnya berdasarkan ID terbesar yang ada
    int nextId() const;
};
