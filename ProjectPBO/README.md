# UGM Library Borrowing System
**OOP Mini Project — DTETI UGM**  
**Week 1: Core Engine + Database Layer**

---

## Identitas
| Field | Value |
|-------|-------|
| Nama | *(Desi  D Simamora)* |
| NIM | *(23/514990/TK/56564)* |

---

## Cara Build & Menjalankan

### Requirement
- `g++` dengan dukungan C++17
- Linux / WSL / macOS

### Build
```bash
make          # atau: make seed
```

### Jalankan Seed (insert data awal)
```bash
./seed
```
Data akan tersimpan di folder `data/` dan **persists** antar run.

### Reset Data
```bash
make clean-data
./seed        # seed ulang dari awal
```

### Build dari awal
```bash
make clean && make
```

---

## Struktur Proyek
```
borrowing-system/
├── Makefile
├── README.md
├── data/                      ← CSV database (auto-created)
│   ├── books.csv
│   ├── members.csv
│   └── loans.csv
└── src/
    ├── seed.cpp               ← Entry point Week 1
    ├── models/
    │   ├── User.h / .cpp      ← Base class (abstract)
    │   ├── Admin.h / .cpp     ← extends User
    │   ├── Member.h / .cpp    ← extends User
    │   ├── Book.h / .cpp
    │   └── Loan.h / .cpp
    ├── repository/
    │   ├── BookRepository.h / .cpp
    │   ├── MemberRepository.h / .cpp
    │   └── LoanRepository.h / .cpp
    └── database/
        └── DbUtils.h          ← CSV helpers, date utilities
```

---

## UML Class Diagram

*(Lihat file `uml.png` atau diagram draw.io)*

Ringkasan relasi:
- `User` ← `Admin`, `Member` (Inheritance)
- `Admin` uses `BookRepository`, `MemberRepository`
- `Member` uses `LoanRepository`
- `Loan` has-a `Book` (via bookId) dan `Member` (via memberId)
- Setiap Repository manage satu tipe entitas ke CSV

---

## Database Schema

**Path B — CSV Files** (dipilih untuk simplisitas dan kemudahan debug)

### `data/books.csv`
| Kolom | Tipe | Keterangan |
|-------|------|-----------|
| id | int | Primary key, auto-increment |
| title | string | Judul buku |
| author | string | Nama pengarang |
| available | 0/1 | 1 = tersedia, 0 = dipinjam |

### `data/members.csv`
| Kolom | Tipe | Keterangan |
|-------|------|-----------|
| memberId | int | Primary key |
| name | string | Nama lengkap |
| email | string | Email UGM |
| username | string | Login username |
| passwordHash | string | Hash dari password |

### `data/loans.csv`
| Kolom | Tipe | Keterangan |
|-------|------|-----------|
| loanId | int | Primary key |
| bookId | int | FK → books.id |
| memberId | int | FK → members.memberId |
| borrowDate | YYYY-MM-DD | Tanggal pinjam |
| dueDate | YYYY-MM-DD | Batas kembali |
| returned | 0/1 | 1 = sudah dikembalikan |

---

## Konsep OOP yang Digunakan

| Konsep | Lokasi | Keterangan |
|--------|--------|-----------|
| **Encapsulation** | Semua model | Semua field `private`, akses via getter/setter |
| **Inheritance** | `Admin`, `Member` extends `User` | `role()` sebagai virtual method |
| **Polymorphism** | `User::role()` pure virtual | Override di Admin dan Member |
| **Operator Overloading** | `Book::operator==`, `Loan::operator==`, `User::operator==`, `operator<<` | Perbandingan dan stream output |
| **Composition** | `Loan` has bookId + memberId | Loan menghubungkan Book dan Member |
| **STL Containers** | `std::vector<Book>`, dll | Semua listAll() mengembalikan vector |
| **RAII** | Repository constructors | File dibuat otomatis bila belum ada |
| **std::optional** | `findById()` | Return type aman, tidak nullptr |

---

## Keterbatasan (Known Limitations)

- Password menggunakan `std::hash` sederhana, bukan SHA-256 (Week 3 bonus)
- CSV tidak mendukung field yang mengandung newline
- Tidak ada concurrent access protection
- `isOverdue()` di `Loan` bergantung pada sistem waktu lokal

---

## Rencana Week 2 & 3

- **Week 2:** Admin CLI — login, menu CRUD buku & member, alur loan
- **Week 3:** Web server dengan `cpp-httplib` pada port 8080
