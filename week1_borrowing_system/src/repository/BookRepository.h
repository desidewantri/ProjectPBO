#pragma once
#include "../models/Book.h"
#include <vector>
#include <string>
#include <optional>

class BookRepository {
private:
    std::string fileName_;

    int getNextId() const;

public:
    explicit BookRepository(const std::string& fileName = "data/books.csv");

    void save(const Book& book);
    void remove(int id);
    void update(const Book& book);
    std::optional<Book> findById(int id) const;
    std::vector<Book> listAll() const;
    std::vector<Book> findAvailable() const;
    std::vector<Book> search(const std::string& keyword) const;

    // Auto-generate next ID for new books
    int nextId() const;
};
