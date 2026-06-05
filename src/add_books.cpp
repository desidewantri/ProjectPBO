// add_books.cpp — Script tambah buku ke database
// Jalankan sekali: ./add_books

#include <iostream>
#include <filesystem>
#include "models/Book.h"
#include "repository/BookRepository.h"

int main() {
    std::filesystem::create_directories("data");
    BookRepository bookRepo;

    std::vector<std::pair<std::string,std::string>> books = {
        // 10 Buku Cerita Rakyat
        {"Malin Kundang",                    "Cerita Rakyat Sumatera Barat"},
        {"Timun Mas",                        "Cerita Rakyat Jawa Tengah"},
        {"Sangkuriang",                      "Cerita Rakyat Jawa Barat"},
        {"Bawang Merah Bawang Putih",        "Cerita Rakyat Nusantara"},
        {"Roro Jonggrang",                   "Cerita Rakyat Yogyakarta"},
        {"Si Pitung",                        "Cerita Rakyat Betawi"},
        {"Lutung Kasarung",                  "Cerita Rakyat Sunda"},
        {"Danau Toba",                       "Cerita Rakyat Sumatera Utara"},
        {"Putri Mandalika",                  "Cerita Rakyat Lombok"},
        {"Calon Arang",                      "Cerita Rakyat Bali"},

        // 15 Buku Psikologi
        {"Thinking, Fast and Slow",          "Daniel Kahneman"},
        {"The Power of Habit",               "Charles Duhigg"},
        {"Man's Search for Meaning",         "Viktor Frankl"},
        {"Emotional Intelligence",           "Daniel Goleman"},
        {"The 7 Habits of Highly Effective", "Stephen Covey"},
        {"Atomic Habits",                    "James Clear"},
        {"Flow: The Psychology of Optimal",  "Mihaly Csikszentmihalyi"},
        {"The Psychology of Money",          "Morgan Housel"},
        {"Influence: The Psychology",        "Robert Cialdini"},
        {"Mindset: The New Psychology",      "Carol Dweck"},
        {"Quiet: The Power of Introverts",   "Susan Cain"},
        {"The Subtle Art of Not Giving",     "Mark Manson"},
        {"Daring Greatly",                   "Brene Brown"},
        {"Ikigai",                           "Hector Garcia & Francesc Miralles"},
        {"The Body Keeps the Score",         "Bessel van der Kolk"},

        // 5 Buku Islam
        {"Fikih Sunnah",                     "Sayyid Sabiq"},
        {"Riyadhus Shalihin",                "Imam An-Nawawi"},
        {"La Tahzan",                        "Aidh Al-Qarni"},
        {"Tafsir Al-Misbah",                 "M. Quraish Shihab"},
        {"Ihya Ulumuddin",                   "Imam Al-Ghazali"},

        // 15 Buku Kristen
        {"Alkitab",                          "Lembaga Alkitab Indonesia"},
        {"The Purpose Driven Life",          "Rick Warren"},
        {"Mere Christianity",                "C.S. Lewis"},
        {"The Case for Christ",              "Lee Strobel"},
        {"Experiencing God",                 "Henry Blackaby"},
        {"The Screwtape Letters",            "C.S. Lewis"},
        {"Crazy Love",                       "Francis Chan"},
        {"Forgotten God",                    "Francis Chan"},
        {"The Ragamuffin Gospel",            "Brennan Manning"},
        {"Jesus Calling",                    "Sarah Young"},
        {"Boundaries",                       "Henry Cloud & John Townsend"},
        {"The Problem of Pain",              "C.S. Lewis"},
        {"Knowing God",                      "J.I. Packer"},
        {"The Normal Christian Life",        "Watchman Nee"},
        {"My Utmost for His Highest",        "Oswald Chambers"},
    };

    std::cout << "Menambahkan " << books.size() << " buku...\n\n";
    for (const auto& [title, author] : books) {
        int id = bookRepo.nextId();
        Book b(id, title, author, true);
        bookRepo.save(b);
        std::cout << "  + #" << id << " " << title << "\n";
    }

    std::cout << "\n✓ Selesai! Total buku: " << bookRepo.listAll().size() << "\n";
    return 0;
}
