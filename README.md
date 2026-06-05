# UGM Library — Borrowing System in C++

**Nama  :** Desi D Simamora  
**NIM   :** 23/514990/TK/56564  

---

## Deskripsi

Sistem peminjaman buku berbasis C++ dengan dua antarmuka:
- **Admin CLI** (`./admin`) — kelola buku, member, dan peminjaman via terminal
- **User Web** (`./web`) — pengguna dapat melihat dan meminjam buku via browser

Keduanya menggunakan satu **core OOP engine** dan satu **database CSV** yang sama.

---

## Struktur Proyek

```
ProjectPBO/
├── Makefile
├── README.md
├── data/                        ← database CSV (auto-generated)
│   ├── books.csv
│   ├── members.csv
│   └── loans.csv
└── src/
    ├── seed.cpp                 ← isi data awal
    ├── admin.cpp                ← Admin CLI (Week 2)
    ├── web.cpp                  ← User Web Server (Week 3)
    ├── httplib.h                ← library HTTP (cpp-httplib)
    ├── database/
    │   └── DbUtils.h            ← helper CSV & tanggal
    ├── models/
    │   ├── User.h / User.cpp    ← base class
    │   ├── Admin.h / Admin.cpp  ← extends User
    │   ├── Member.h / Member.cpp← extends User
    │   ├── Book.h / Book.cpp    ← entitas buku
    │   └── Loan.h / Loan.cpp    ← entitas peminjaman
    └── repository/
        ├── BookRepository.h/cpp
        ├── MemberRepository.h/cpp
        └── LoanRepository.h/cpp
```

---

## UML Class Diagram

![UML Diagram](uml.png)

---

## Cara Build

> Pastikan sudah install **MSYS2 UCRT64** dan **g++** tersedia.

```bash
# Install g++ (sekali saja)
pacman -S mingw-w64-ucrt-x86_64-gcc

# Build semua
make all

# Atau build satu per satu
make seed    # build seeder
make admin   # build CLI
make web     # build web server
```

---

## Cara Menjalankan

### 1. Isi data awal (wajib dijalankan sekali)
```bash
./seed
```

### 2. Admin CLI
```bash
./admin
```
- Login: username `admin`, password `admin123`
- Fitur: kelola buku, kelola member, kelola peminjaman

### 3. Web Server
```bash
./web
```
- Buka browser: `http://localhost:8080`
- Fitur: lihat buku, cari buku, pinjam, kembalikan, lihat pinjaman saya

> **Catatan:** `./admin` dan `./web` dapat dijalankan bersamaan. Data yang diubah di CLI langsung terlihat di web, dan sebaliknya.

---

## Routes Web

| Method | Route | Fungsi |
|--------|-------|--------|
| GET | `/` | Landing page, daftar semua buku |
| GET | `/search?q=...` | Pencarian buku by judul/pengarang |
| POST | `/borrow` | Pinjam buku (memberId + bookId) |
| POST | `/return` | Kembalikan buku (loanId) |
| GET | `/me?id=...` | Lihat pinjaman aktif member |

---

## Schema Database (CSV)

### books.csv
| Field | Tipe | Keterangan |
|-------|------|-----------|
| id | int | Primary key |
| title | string | Judul buku |
| author | string | Pengarang |
| available | bool | 1=tersedia, 0=dipinjam |

### members.csv
| Field | Tipe | Keterangan |
|-------|------|-----------|
| id | int | Primary key |
| name | string | Nama lengkap |
| email | string | Email |
| username | string | Username login |
| passwordHash | string | Hash password |

### loans.csv
| Field | Tipe | Keterangan |
|-------|------|-----------|
| loanId | int | Primary key |
| bookId | int | Foreign key → books |
| memberId | int | Foreign key → members |
| borrowDate | string | Tanggal pinjam (YYYY-MM-DD) |
| dueDate | string | Batas kembali (YYYY-MM-DD) |
| returned | bool | 1=sudah kembali, 0=belum |

---

## Konsep OOP yang Diimplementasikan

| Konsep | Implementasi |
|--------|-------------|
| **Encapsulation** | Semua field private, akses via getter/setter |
| **Inheritance** | `Admin` dan `Member` extends `User` |
| **Polymorphism** | `role()` virtual di `User`, di-override di `Admin` dan `Member` |
| **Composition** | `Loan` memiliki referensi ke `Book` dan `Member` |
| **Operator Overloading** | `operator==` dan `operator<<` di `Book`, `Loan`, `User` |
| **STL Containers** | `std::vector`, `std::optional` di Repository |
| **File I/O** | `<fstream>` untuk baca/tulis CSV |
| **Separation of Concerns** | Model / Repository / CLI / Web terpisah |

---

## Screenshots

### Admin CLI
![CLI Login](screenshots/cli_login.png)
![CLI Buku](screenshots/cli_books.png)
![CLI Pinjaman](screenshots/cli_loans.png)

### User Web
![Web Beranda](screenshots/web_home.png)
![Web Pencarian](screenshots/web_search.png)
![Web Pinjaman Saya](screenshots/web_me.png)

---

## Known Limitations

- Password admin hardcoded (`admin123`), belum disimpan di database
- Tidak ada fitur registrasi member via web (hanya via CLI admin)
- CSV tidak thread-safe jika diakses bersamaan oleh banyak user
- Tidak ada pagination pada daftar buku yang panjang
- Belum ada fitur perpanjangan (renew) pinjaman

---

## Tools & Libraries

| Layer | Tool |
|-------|------|
| Language | C++17 (g++) |
| Build | Makefile |
| HTTP Server | [cpp-httplib](https://github.com/yhirose/cpp-httplib) |
| Database | CSV (pure `<fstream>`) |
| Version Control | Git + GitHub |

---

## Academic Integrity

Proyek ini dikerjakan secara individu. Referensi yang digunakan:
- [cppreference.com](https://cppreference.com) — dokumentasi C++ STL
- [cpp-httplib documentation](https://github.com/yhirose/cpp-httplib)
- AI assistant (Claude) — untuk sintaks dan debugging
