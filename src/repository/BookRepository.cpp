#include "BookRepository.h"
#include "../database/DbUtils.h"
#include <fstream>
#include <iostream>
#include <algorithm>

// Inisialisasi: buat file CSV dengan header jika belum ada
BookRepository::BookRepository(const std::string& fileName)
    : fileName_(fileName) {
    std::ifstream test(fileName_);
    if (!test.good()) {
        std::ofstream create(fileName_);
        create << "id,title,author,available\n";
    }
}

// Cari ID terbesar lalu tambah 1
int BookRepository::nextId() const {
    auto all = listAll();
    if (all.empty()) return 1;
    int maxId = 0;
    for (const auto& b : all) {
        if (b.getId() > maxId) maxId = b.getId();
    }
    return maxId + 1;
}

// Tambah buku baru ke akhir file CSV
void BookRepository::save(const Book& book) {
    std::ofstream file(fileName_, std::ios::app);
    if (!file) { std::cerr << "Error: cannot open " << fileName_ << "\n"; return; }
    file << book.getId() << ","
         << csvEscape(book.getTitle()) << ","
         << csvEscape(book.getAuthor()) << ","
         << (book.isAvailable() ? "1" : "0") << "\n";
}

// Hapus buku: tulis ulang file tanpa buku yang dihapus
void BookRepository::remove(int id) {
    auto all = listAll();
    std::ofstream file(fileName_);
    file << "id,title,author,available\n";
    for (const auto& b : all) {
        if (b.getId() != id) {
            file << b.getId() << ","
                 << csvEscape(b.getTitle()) << ","
                 << csvEscape(b.getAuthor()) << ","
                 << (b.isAvailable() ? "1" : "0") << "\n";
        }
    }
}

// Update buku: tulis ulang file, ganti baris yang id-nya cocok
void BookRepository::update(const Book& book) {
    auto all = listAll();
    std::ofstream file(fileName_);
    file << "id,title,author,available\n";
    for (const auto& b : all) {
        if (b.getId() == book.getId()) {
            // Tulis data yang sudah diupdate
            file << book.getId() << ","
                 << csvEscape(book.getTitle()) << ","
                 << csvEscape(book.getAuthor()) << ","
                 << (book.isAvailable() ? "1" : "0") << "\n";
        } else {
            file << b.getId() << ","
                 << csvEscape(b.getTitle()) << ","
                 << csvEscape(b.getAuthor()) << ","
                 << (b.isAvailable() ? "1" : "0") << "\n";
        }
    }
}

// Cari buku berdasarkan ID, return nullopt jika tidak ditemukan
std::optional<Book> BookRepository::findById(int id) const {
    for (const auto& b : listAll()) {
        if (b.getId() == id) return b;
    }
    return std::nullopt;
}

// Baca semua buku dari file CSV, skip header
std::vector<Book> BookRepository::listAll() const {
    std::vector<Book> books;
    std::ifstream file(fileName_);
    if (!file) return books;

    std::string line;
    std::getline(file, line); // skip baris header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = csvSplit(line);
        if (tokens.size() < 4) continue;
        int id         = std::stoi(tokens[0]);
        std::string title  = tokens[1];
        std::string author = tokens[2];
        bool available = tokens[3] == "1";
        books.emplace_back(id, title, author, available);
    }
    return books;
}

// Filter buku yang statusnya tersedia
std::vector<Book> BookRepository::findAvailable() const {
    auto all = listAll();
    std::vector<Book> result;
    for (const auto& b : all) {
        if (b.isAvailable()) result.push_back(b);
    }
    return result;
}

// Pencarian case-insensitive berdasarkan judul atau pengarang
std::vector<Book> BookRepository::search(const std::string& keyword) const {
    auto all = listAll();
    std::vector<Book> result;
    std::string kw = keyword;
    std::transform(kw.begin(), kw.end(), kw.begin(), ::tolower);

    for (const auto& b : all) {
        std::string title  = b.getTitle();
        std::string author = b.getAuthor();
        std::transform(title.begin(),  title.end(),  title.begin(),  ::tolower);
        std::transform(author.begin(), author.end(), author.begin(), ::tolower);

        if (title.find(kw) != std::string::npos ||
            author.find(kw) != std::string::npos) {
            result.push_back(b);
        }
    }
    return result;
}
