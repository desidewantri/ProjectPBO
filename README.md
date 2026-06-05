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
<img width="1087" height="270" alt="SS_CLI_LoginAdmin" src="https://github.com/user-attachments/assets/13ca7767-de0f-4ce4-8b31-efde9ea55d03" />

![CLI Buku](screenshots/cli_books.png)
<img width="1031" height="933" alt="SS_CLI_KelolaBuku" src="https://github.com/user-attachments/assets/133a9155-d2db-4896-862d-7c31a88130a2" />

![CLI Pinjaman](screenshots/cli_loans.png)
<img width="1072" height="1092" alt="SS_CLI_KelolaPeminjaman" src="https://github.com/user-attachments/assets/bda1edee-2214-4776-91df-3fb0f63bb7a3" />

### User Web
![Web Beranda](screenshots/web_home.png)
<img width="1918" height="1087" alt="SS_WEB_Beranda" src="https://github.com/user-attachments/assets/0cfa4094-e7b0-4bef-a67a-83d248a7de0d" />

![Web Pencarian](screenshots/web_search.png)
<img width="1918" height="1082" alt="SS_WEB_Pencarian" src="https://github.com/user-attachments/assets/5b709114-eaef-4394-a947-d68ba8c4ffd6" />

![Web Pinjaman Saya](screenshots/web_me.png)
<img width="1917" height="1086" alt="SS_WEB_PinjamanSaya" src="https://github.com/user-attachments/assets/05fcb1d0-13e0-4e62-bc86-c28ba4939af3" />

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
