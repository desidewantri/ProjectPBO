#include "Book.h"
#include <iomanip>

Book::Book(int id, const std::string& title, const std::string& author, bool available)
    : id_(id), title_(title), author_(author), available_(available) {}

int Book::getId()            const { return id_; }
std::string Book::getTitle() const { return title_; }
std::string Book::getAuthor()const { return author_; }
bool Book::isAvailable()     const { return available_; }
void Book::setAvailable(bool a)    { available_ = a; }

// Tampilkan buku dalam format kolom untuk tabel CLI
void Book::display() const {
    std::cout << std::left
              << std::setw(4)  << id_
              << std::setw(35) << title_
              << std::setw(25) << author_
              << (available_ ? "Available" : "On Loan")
              << "\n";
}

// Dua buku dianggap sama jika id-nya sama
bool Book::operator==(const Book& other) const {
    return id_ == other.id_;
}

// Format debug: tampilkan semua field buku
std::ostream& operator<<(std::ostream& os, const Book& b) {
    os << "Book{id=" << b.id_
       << ", title=\"" << b.title_
       << "\", author=\"" << b.author_
       << "\", available=" << (b.available_ ? "yes" : "no") << "}";
    return os;
}
