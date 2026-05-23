#include "BookRepository.h"
#include "../database/DbUtils.h"
#include <fstream>
#include <iostream>
#include <algorithm>

BookRepository::BookRepository(const std::string& fileName)
    : fileName_(fileName) {
    // Ensure data directory and file exist
    std::ifstream test(fileName_);
    if (!test.good()) {
        std::ofstream create(fileName_);
        create << "id,title,author,available\n";
    }
}

int BookRepository::nextId() const {
    auto all = listAll();
    if (all.empty()) return 1;
    int maxId = 0;
    for (const auto& b : all) {
        if (b.getId() > maxId) maxId = b.getId();
    }
    return maxId + 1;
}

void BookRepository::save(const Book& book) {
    std::ofstream file(fileName_, std::ios::app);
    if (!file) {
        std::cerr << "Error: cannot open " << fileName_ << "\n";
        return;
    }
    file << book.getId() << ","
         << csvEscape(book.getTitle()) << ","
         << csvEscape(book.getAuthor()) << ","
         << (book.isAvailable() ? "1" : "0") << "\n";
}

void BookRepository::remove(int id) {
    auto all = listAll();

    // Rewrite file without the removed book
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

void BookRepository::update(const Book& book) {
    auto all = listAll();
    std::ofstream file(fileName_);
    file << "id,title,author,available\n";
    for (const auto& b : all) {
        if (b.getId() == book.getId()) {
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

std::optional<Book> BookRepository::findById(int id) const {
    auto all = listAll();
    for (const auto& b : all) {
        if (b.getId() == id) return b;
    }
    return std::nullopt;
}

std::vector<Book> BookRepository::listAll() const {
    std::vector<Book> books;
    std::ifstream file(fileName_);
    if (!file) return books;

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = csvSplit(line);
        if (tokens.size() < 4) continue;
        int id          = std::stoi(tokens[0]);
        std::string title  = tokens[1];
        std::string author = tokens[2];
        bool available  = tokens[3] == "1";
        books.emplace_back(id, title, author, available);
    }
    return books;
}

std::vector<Book> BookRepository::findAvailable() const {
    auto all = listAll();
    std::vector<Book> result;
    for (const auto& b : all) {
        if (b.isAvailable()) result.push_back(b);
    }
    return result;
}

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
