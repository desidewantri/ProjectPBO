// admin.cpp — Admin CLI untuk sistem peminjaman buku UGM Library
// Week 2: Login, CRUD buku & member, manajemen peminjaman
//
// Cara pakai: ./admin
// Login: username=admin, password=admin123

/**
 * admin.cpp — Admin CLI (Week 2)
 * UGM Library Borrowing System
 *
 * Fitur:
 *  - Login dengan password masking, max 3 attempts
 *  - Menu CRUD buku & member
 *  - Alur peminjaman & pengembalian
 *  - Validasi input menyeluruh
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <filesystem>

#include "models/Admin.h"
#include "models/Book.h"
#include "models/Member.h"
#include "models/Loan.h"
#include "repository/BookRepository.h"
#include "repository/MemberRepository.h"
#include "repository/LoanRepository.h"
#include "database/DbUtils.h"

// ── ANSI Color Codes (bonus: coloured CLI output) ───────────────────────────
#ifdef _WIN32
#  include <windows.h>
// Enable ANSI on Windows terminal
static bool enableAnsi() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(h, &mode);
    SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    return true;
}
static bool _ansiEnabled = enableAnsi();
#endif

namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string BG_RED  = "\033[41m";
}

// ── Terminal helpers ────────────────────────────────────────────────────────

#ifdef _WIN32
#  include <conio.h>
static std::string readPassword() {
    std::string pw;
    char c;
    while ((c = _getch()) != '\r') {
        if (c == '\b' && !pw.empty()) { pw.pop_back(); std::cout << "\b \b"; }
        else if (c != '\b')           { pw += c;        std::cout << '*'; }
    }
    std::cout << '\n';
    return pw;
}
#else
#  include <termios.h>
#  include <unistd.h>
static std::string readPassword() {
    // Jika input bukan TTY (misal piped), baca biasa tanpa masking
    if (!isatty(STDIN_FILENO)) {
        std::string pw;
        std::getline(std::cin, pw);
        return pw;
    }

    termios oldt{};
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~(ECHO);          // matikan echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::string pw;
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != '\n') {
        if ((c == 127 || c == '\b') && !pw.empty()) {
            pw.pop_back();
            std::cout << "\b \b" << std::flush;
        } else if (c != 127 && c != '\b') {
            pw += c;
            std::cout << '*' << std::flush;
        }
    }
    std::cout << '\n';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return pw;
}
#endif

// Print helpers 

static void printSep(char ch = '-', int w = 72) {
    std::cout << std::string(w, ch) << '\n';
}

static void printHeader(const std::string& title) {
    std::cout << '\n';
    printSep('=');
    std::cout << Color::BOLD << Color::CYAN << "  " << title << Color::RESET << '\n';
    printSep('=');
}

// Baca satu baris; kembalikan false jika kosong
static bool readLine(const std::string& prompt, std::string& out) {
    std::cout << prompt;
    std::getline(std::cin, out);
    // trim
    out.erase(0, out.find_first_not_of(" \t"));
    out.erase(out.find_last_not_of(" \t") + 1);
    return !out.empty();
}

// Baca integer positif; return -1 jika gagal
static int readInt(const std::string& prompt) {
    std::string s;
    if (!readLine(prompt, s)) return -1;
    try {
        size_t pos;
        int v = std::stoi(s, &pos);
        if (pos != s.size()) return -1;   // ada karakter non-numerik
        return v;
    } catch (...) { return -1; }
}

// Tabel buku 

static void printBooksTable(const std::vector<Book>& books) {
    if (books.empty()) { std::cout << "  (tidak ada data)\n"; return; }
    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(36) << "Judul"
              << std::setw(22) << "Pengarang"
              << "Status\n";
    printSep();
    for (const auto& b : books) {
        std::cout << std::left
                  << std::setw(5)  << b.getId()
                  << std::setw(36) << b.getTitle().substr(0, 34)
                  << std::setw(22) << b.getAuthor().substr(0, 20)
                  << (b.isAvailable() ? Color::GREEN + "Tersedia" + Color::RESET : Color::YELLOW + "Dipinjam" + Color::RESET) << '\n';
    }
}

//  Tabel member
static void printMembersTable(const std::vector<Member>& members) {
    if (members.empty()) { std::cout << "  (tidak ada data)\n"; return; }
    std::cout << std::left
              << std::setw(6)  << "ID"
              << std::setw(24) << "Nama"
              << std::setw(28) << "Email"
              << "Username\n";
    printSep();
    for (const auto& m : members) {
        std::cout << std::left
                  << std::setw(6)  << m.getMemberId()
                  << std::setw(24) << m.getName().substr(0, 22)
                  << std::setw(28) << m.getEmail().substr(0, 26)
                  << m.getUsername() << '\n';
    }
}

//  Tabel loan 

static void printLoansTable(const std::vector<Loan>& loans,
                             BookRepository& bookRepo,
                             MemberRepository& memberRepo) {
    if (loans.empty()) { std::cout << "  (tidak ada data)\n"; return; }
    std::cout << std::left
              << std::setw(7)  << "LoanID"
              << std::setw(5)  << "BkID"
              << std::setw(30) << "Judul Buku"
              << std::setw(7)  << "MbrID"
              << std::setw(20) << "Nama Member"
              << std::setw(13) << "Tgl Pinjam"
              << std::setw(13) << "Tgl Kembali"
              << "Status\n";
    printSep();
    for (const auto& l : loans) {
        auto book   = bookRepo.findById(l.getBookId());
        auto member = memberRepo.findById(l.getMemberId());
        std::string title  = book   ? book->getTitle().substr(0, 28)   : "?";
        std::string name   = member ? member->getName().substr(0, 18)  : "?";
        std::string status = l.isReturned()  ? Color::GREEN  + "Kembali"   + Color::RESET
                           : l.isOverdue()   ? Color::RED    + "TERLAMBAT" + Color::RESET
                                             : Color::YELLOW + "Aktif"     + Color::RESET;
        std::cout << std::left
                  << std::setw(7)  << l.getLoanId()
                  << std::setw(5)  << l.getBookId()
                  << std::setw(30) << title
                  << std::setw(7)  << l.getMemberId()
                  << std::setw(20) << name
                  << std::setw(13) << l.getBorrowDate()
                  << std::setw(13) << l.getDueDate()
                  << status << '\n';
    }
}

//  MENU: Kelola Buku


static void menuBooks(BookRepository& bookRepo, LoanRepository& loanRepo) {
    while (true) {
        printHeader("Kelola Buku");
        printBooksTable(bookRepo.listAll());

        std::cout << "\n[a] Tambah  [e] Edit  [d] Hapus  [s] Cari  [b] Kembali\n";
        std::string choice;
        readLine("Pilihan: ", choice);
        if (choice.empty()) continue;
        char c = std::tolower(choice[0]);

        // TAMBAH BUKU 
        if (c == 'a') {
            printHeader("Tambah Buku Baru");
            std::string title, author;
            if (!readLine("Judul    : ", title))  { std::cout << "  Judul tidak boleh kosong.\n"; continue; }
            if (!readLine("Pengarang: ", author)) { std::cout << "  Pengarang tidak boleh kosong.\n"; continue; }

            int id = bookRepo.nextId();
            Book b(id, title, author, true);
            bookRepo.save(b);
            std::cout << Color::GREEN << "  ✓ Buku #" << id << " \"" << title << "\" berhasil ditambahkan." << Color::RESET << '\n';
        }

        //  EDIT BUKU
        else if (c == 'e') {
            int id = readInt("ID buku yang akan diedit: ");
            if (id <= 0) { std::cout << "  ID tidak valid.\n"; continue; }

            auto opt = bookRepo.findById(id);
            if (!opt) { std::cout << "  Buku #" << id << " tidak ditemukan.\n"; continue; }

            Book existing = *opt;
            std::cout << "  Judul saat ini    : " << existing.getTitle()  << '\n';
            std::cout << "  Pengarang saat ini: " << existing.getAuthor() << '\n';
            std::cout << "  (Tekan Enter untuk tetap)\n";

            std::string newTitle, newAuthor;
            std::cout << "  Judul baru    : ";
            std::getline(std::cin, newTitle);
            std::cout << "  Pengarang baru: ";
            std::getline(std::cin, newAuthor);

            // Trim
            auto trim = [](std::string& s) {
                s.erase(0, s.find_first_not_of(" \t"));
                s.erase(s.find_last_not_of(" \t") + 1);
            };
            trim(newTitle); trim(newAuthor);

            Book updated(id,
                         newTitle.empty()  ? existing.getTitle()  : newTitle,
                         newAuthor.empty() ? existing.getAuthor() : newAuthor,
                         existing.isAvailable());
            bookRepo.update(updated);
            std::cout << Color::GREEN << "  ✓ Buku #" << id << " berhasil diperbarui." << Color::RESET << '\n';
        }

        //  HAPUS BUKU 
        else if (c == 'd') {
            int id = readInt("ID buku yang akan dihapus: ");
            if (id <= 0) { std::cout << "  ID tidak valid.\n"; continue; }

            auto opt = bookRepo.findById(id);
            if (!opt) { std::cout << "  Buku #" << id << " tidak ditemukan.\n"; continue; }

            // Cek apakah sedang dipinjam
            auto activeLoans = loanRepo.findActiveLoans();
            bool onLoan = false;
            for (const auto& l : activeLoans) {
                if (l.getBookId() == id) { onLoan = true; break; }
            }
            if (onLoan) {
                std::cout << Color::RED << "  ✗ Buku #" << id << " sedang dipinjam. Tidak dapat dihapus." << Color::RESET << '\n';
                continue;
            }

            std::string confirm;
            std::cout << "  Hapus \"" << opt->getTitle() << "\"? (y/N): ";
            std::getline(std::cin, confirm);
            if (!confirm.empty() && std::tolower(confirm[0]) == 'y') {
                bookRepo.remove(id);
                std::cout << Color::GREEN << "  ✓ Buku #" << id << " berhasil dihapus." << Color::RESET << '\n';
            } else {
                std::cout << "  Dibatalkan.\n";
            }
        }

        // CARI BUKU 
        else if (c == 's') {
            std::string keyword;
            if (!readLine("Kata kunci: ", keyword)) { std::cout << "  Kata kunci kosong.\n"; continue; }
            auto results = bookRepo.search(keyword);
            printHeader("Hasil Pencarian: \"" + keyword + "\"");
            printBooksTable(results);
        }

        else if (c == 'b') break;
        else std::cout << "  Pilihan tidak dikenal.\n";
    }
}


//  MENU: Kelola Member


static std::string simpleHash(const std::string& s) {
    size_t h = 0;
    for (char ch : s) h = h * 31 + ch;
    return std::to_string(h);
}

static void menuMembers(MemberRepository& memberRepo, LoanRepository& loanRepo) {
    while (true) {
        printHeader("Kelola Member");
        printMembersTable(memberRepo.listAll());

        std::cout << "\n[a] Tambah  [e] Edit  [d] Hapus  [b] Kembali\n";
        std::string choice;
        readLine("Pilihan: ", choice);
        if (choice.empty()) continue;
        char c = std::tolower(choice[0]);

        // TAMBAH MEMBER 
        if (c == 'a') {
            printHeader("Tambah Member Baru");
            std::string name, email, username, password;
            if (!readLine("Nama lengkap: ", name))  { std::cout << "  Nama tidak boleh kosong.\n"; continue; }
            if (!readLine("Email       : ", email)) { std::cout << "  Email tidak boleh kosong.\n"; continue; }
            if (!readLine("Username    : ", username)) { std::cout << "  Username tidak boleh kosong.\n"; continue; }

            // Cek username duplikat
            bool dupUser = false;
            for (const auto& m : memberRepo.listAll()) {
                if (m.getUsername() == username) { dupUser = true; break; }
            }
            if (dupUser) { std::cout << Color::RED << "  ✗ Username \"" << username << "\" sudah digunakan." << Color::RESET << '\n'; continue; }

            std::cout << "Password    : ";
            password = readPassword();
            if (password.empty()) { std::cout << "  Password tidak boleh kosong.\n"; continue; }

            int id = memberRepo.nextId();
            Member mem(id, name, email, username, simpleHash(password));
            memberRepo.save(mem);
            std::cout << "  ✓ Member #" << id << " \"" << name << "\" berhasil ditambahkan." << Color::RESET << '\n';
        }

        // EDIT MEMBER 
        else if (c == 'e') {
            int id = readInt("ID member yang akan diedit: ");
            if (id <= 0) { std::cout << "  ID tidak valid.\n"; continue; }

            auto opt = memberRepo.findById(id);
            if (!opt) { std::cout << "  Member #" << id << " tidak ditemukan.\n"; continue; }

            Member existing = *opt;
            std::cout << "  Nama saat ini   : " << existing.getName()  << '\n';
            std::cout << "  Email saat ini  : " << existing.getEmail() << '\n';
            std::cout << "  (Tekan Enter untuk tetap tidak berubah)\n";

            std::string newName, newEmail;
            std::cout << "  Nama baru   : ";
            std::getline(std::cin, newName);
            std::cout << "  Email baru  : ";
            std::getline(std::cin, newEmail);

            // Trim
            auto trim = [](std::string& s) {
                s.erase(0, s.find_first_not_of(" \t"));
                if (!s.empty()) s.erase(s.find_last_not_of(" \t") + 1);
            };
            trim(newName); trim(newEmail);

            Member updated(id,
                           newName.empty()  ? existing.getName()  : newName,
                           newEmail.empty() ? existing.getEmail() : newEmail,
                           existing.getUsername(),
                           existing.getPasswordHash());
            memberRepo.update(updated);
            std::cout << "  ✓ Member #" << id << " berhasil diperbarui." << Color::RESET << '\n';
        }

        // HAPUS MEMBER — cek dulu apakah masih punya pinjaman aktif
        else if (c == 'd') {
            int id = readInt("ID member yang akan dihapus: ");
            if (id <= 0) { std::cout << "  ID tidak valid.\n"; continue; }

            auto opt = memberRepo.findById(id);
            if (!opt) { std::cout << "  Member #" << id << " tidak ditemukan.\n"; continue; }

            // Cek apakah punya pinjaman aktif
            auto loans = loanRepo.findByMember(id);
            bool hasActive = false;
            for (const auto& l : loans) {
                if (!l.isReturned()) { hasActive = true; break; }
            }
            if (hasActive) {
                std::cout << Color::RED << "  ✗ Member #" << id << " masih punya pinjaman aktif. Tidak dapat dihapus." << Color::RESET << '\n';
                continue;
            }

            std::string confirm;
            std::cout << "  Hapus member \"" << opt->getName() << "\"? (y/N): ";
            std::getline(std::cin, confirm);
            if (!confirm.empty() && std::tolower(confirm[0]) == 'y') {
                memberRepo.remove(id);
                std::cout << "  ✓ Member #" << id << " berhasil dihapus." << Color::RESET << '\n';
            } else {
                std::cout << "  Dibatalkan.\n";
            }
        }

        else if (c == 'b') break;
        else std::cout << "  Pilihan tidak dikenal.\n";
    }
}

//  MENU: Kelola Peminjaman


static void menuLoans(BookRepository& bookRepo,
                      MemberRepository& memberRepo,
                      LoanRepository& loanRepo) {
    while (true) {
        printHeader("Kelola Peminjaman");
        std::cout << "[1] Lihat semua pinjaman aktif\n"
                  << "[2] Lihat pinjaman terlambat\n"
                  << "[3] Keluarkan pinjaman baru\n"
                  << "[4] Catat pengembalian\n"
                  << "[5] Riwayat pinjaman per member\n"
                  << "[B] Kembali\n";

        std::string choice;
        readLine("Pilihan: ", choice);
        if (choice.empty()) continue;
        char c = std::tolower(choice[0]);

        // LIHAT PINJAMAN AKTIF
        if (c == '1') {
            printHeader("Pinjaman Aktif");
            auto active = loanRepo.findActiveLoans();
            printLoansTable(active, bookRepo, memberRepo);
        }

        //LIHAT TERLAMBAT 
        else if (c == '2') {
            printHeader("Pinjaman Terlambat");
            auto overdue = loanRepo.findOverdue();
            printLoansTable(overdue, bookRepo, memberRepo);
            if (!overdue.empty())
                std::cout << "\n  ⚠  Total terlambat: " << overdue.size() << " item\n";
        }

        // PINJAMAN BARU
        else if (c == '3') {
            printHeader("Keluarkan Pinjaman Baru");

            int memberId = readInt("ID Member : ");
            if (memberId <= 0) { std::cout << "  ID tidak valid.\n"; continue; }
            auto member = memberRepo.findById(memberId);
            if (!member) { std::cout << "  Member #" << memberId << " tidak ditemukan.\n"; continue; }

            int bookId = readInt("ID Buku   : ");
            if (bookId <= 0) { std::cout << "  ID tidak valid.\n"; continue; }
            auto book = bookRepo.findById(bookId);
            if (!book) { std::cout << "  Buku #" << bookId << " tidak ditemukan.\n"; continue; }

            if (!book->isAvailable()) {
                std::cout << Color::RED << "  ✗ Buku \"" << book->getTitle() << "\" sedang dipinjam orang lain." << Color::RESET << '\n';
                continue;
            }

            int days = readInt("Durasi pinjam (hari, default 14): ");
            if (days <= 0) days = 14;

            std::string borrowDate = todayDate();
            std::string dueDate    = dueDateFromToday(days);

            int loanId = loanRepo.nextId();
            Loan loan(loanId, bookId, memberId, borrowDate, dueDate, false);
            loanRepo.save(loan);

            // Update status buku → tidak tersedia
            Book updated(bookId, book->getTitle(), book->getAuthor(), false);
            bookRepo.update(updated);

            std::cout << "  ✓ Pinjaman #" << loanId << " berhasil dicatat.\n"
                      << "    " << member->getName() << " meminjam \"" << book->getTitle() << "\"\n"
                      << "    Batas kembali: " << dueDate << '\n';
        }

        // CATAT PENGEMBALIAN
        else if (c == '4') {
            printHeader("Catat Pengembalian");

            int loanId = readInt("ID Pinjaman: ");
            if (loanId <= 0) { std::cout << "  ID tidak valid.\n"; continue; }
            auto loanOpt = loanRepo.findById(loanId);
            if (!loanOpt) { std::cout << "  Pinjaman #" << loanId << " tidak ditemukan.\n"; continue; }
            if (loanOpt->isReturned()) {
                std::cout << "  Pinjaman #" << loanId << " sudah dikembalikan sebelumnya.\n";
                continue;
            }

            Loan updated = *loanOpt;
            updated.setReturned(true);
            loanRepo.update(updated);

            // Update status buku → tersedia kembali
            auto bookOpt = bookRepo.findById(updated.getBookId());
            if (bookOpt) {
                Book bk(bookOpt->getId(), bookOpt->getTitle(), bookOpt->getAuthor(), true);
                bookRepo.update(bk);
            }

            bool late = loanOpt->isOverdue();
            std::cout << "  ✓ Pengembalian pinjaman #" << loanId << " dicatat.\n";
            if (late) std::cout << Color::RED << "  ⚠  Buku ini dikembalikan TERLAMBAT!" << Color::RESET << '\n';
        }

        // RIWAYAT MEMBER
        else if (c == '5') {
            int memberId = readInt("ID Member: ");
            if (memberId <= 0) { std::cout << "  ID tidak valid.\n"; continue; }
            auto member = memberRepo.findById(memberId);
            if (!member) { std::cout << "  Member #" << memberId << " tidak ditemukan.\n"; continue; }

            printHeader("Riwayat Pinjaman: " + member->getName());
            auto loans = loanRepo.findByMember(memberId);
            printLoansTable(loans, bookRepo, memberRepo);
        }

        else if (c == 'b') break;
        else std::cout << "  Pilihan tidak dikenal.\n";
    }
}

//  LOGIN

static bool doLogin() {
    // Verifikasi kredensial admin — max 3 percobaan
    // Admin tunggal — kredensial hardcoded (bisa diganti DB di Week 3)
    // Password: admin123  →  hash: simpleHash("admin123")
    const std::string ADMIN_USER = "admin";
    const std::string ADMIN_HASH = simpleHash("admin123");

    std::cout << Color::BOLD << Color::BLUE
              << "\n╔══════════════════════════════════════════╗\n"
              << "║     UGM Library — Admin Console v1.0    ║\n"
              << "╚══════════════════════════════════════════╝\n"
              << Color::RESET << "\n";

    for (int attempt = 1; attempt <= 3; ++attempt) {
        std::string username, password;
        readLine("Username: ", username);
        std::cout << "Password: ";
        password = readPassword();

        if (username == ADMIN_USER && simpleHash(password) == ADMIN_HASH) {
            return true;
        }

        int remaining = 3 - attempt;
        if (remaining > 0)
            std::cout << "  ✗ Username/password salah. Sisa percobaan: " << remaining << '\n';
        else
            std::cout << "  ✗ Terlalu banyak percobaan gagal. Akses ditolak.\n";
    }
    return false;
}

//  MAIN MENU

int main() {
    // Buat folder data jika belum ada
    std::filesystem::create_directories("data");

    BookRepository   bookRepo;
    MemberRepository memberRepo;
    LoanRepository   loanRepo;

    if (!doLogin()) return 1;

    // Tampilkan ringkasan setelah login
    auto books   = bookRepo.listAll();
    auto members = memberRepo.listAll();
    auto active  = loanRepo.findActiveLoans();
    auto overdue = loanRepo.findOverdue();

    std::cout << Color::GREEN << "\n  [OK] Selamat datang, admin!" << Color::RESET << "\n"
              << "  " << books.size()   << " buku  |  "
              << members.size()         << " member  |  "
              << active.size()          << " pinjaman aktif";
    if (!overdue.empty())
        std::cout << Color::RED << "  |  ⚠  " << overdue.size() << " TERLAMBAT" << Color::RESET;
    std::cout << "\n";

    //Main loop
    while (true) {
        printHeader("Menu Utama");
        std::cout << "[1] Kelola Buku\n"
                  << "[2] Kelola Member\n"
                  << "[3] Kelola Peminjaman\n"
                  << "[Q] Keluar\n";

        std::string choice;
        readLine("Pilihan: ", choice);
        if (choice.empty()) continue;
        char c = std::tolower(choice[0]);

        if      (c == '1') menuBooks  (bookRepo, loanRepo);
        else if (c == '2') menuMembers(memberRepo, loanRepo);
        else if (c == '3') menuLoans  (bookRepo, memberRepo, loanRepo);
        else if (c == 'q') {
            std::cout << "\n  Sampai jumpa!\n\n";
            break;
        }
        else std::cout << "  Pilihan tidak dikenal.\n";
    }

    return 0;
}
