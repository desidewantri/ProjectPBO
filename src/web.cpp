/**
 * web.cpp — User Web Server (Week 3)
 * UGM Library Borrowing System
 *
 * Routes:
 *   GET  /              — landing page, daftar buku tersedia
 *   GET  /search?q=...  — pencarian buku
 *   POST /borrow        — pinjam buku (memberId + bookId)
 *   POST /return        — kembalikan buku (loanId)
 *   GET  /me?id=...     — pinjaman aktif member
 */

#include "httplib.h"

#include "models/Book.h"
#include "models/Member.h"
#include "models/Loan.h"
#include "repository/BookRepository.h"
#include "repository/MemberRepository.h"
#include "repository/LoanRepository.h"
#include "database/DbUtils.h"

#include <string>
#include <sstream>
#include <filesystem>

// ── HTML escape ──────────────────────────────────────────────────────────────
static std::string esc(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&#39;";  break;
            default:   out += c;
        }
    }
    return out;
}

// ── CSS & layout ─────────────────────────────────────────────────────────────
static const std::string CSS = R"(
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: 'Segoe UI', Arial, sans-serif;
    background: #f0f4f8;
    color: #1a202c;
    min-height: 100vh;
  }
  header {
    background: linear-gradient(135deg, #003366 0%, #0055a5 100%);
    color: white;
    padding: 1rem 1.5rem;
    display: flex;
    align-items: center;
    gap: 1rem;
    box-shadow: 0 2px 8px rgba(0,0,0,0.2);
  }
  header h1 { font-size: 1.3rem; font-weight: 700; }
  header p  { font-size: 0.8rem; opacity: 0.85; }
  nav a {
    color: #90cdf4;
    text-decoration: none;
    margin-left: 1.5rem;
    font-size: 0.9rem;
  }
  nav a:hover { color: white; }
  .container { max-width: 900px; margin: 0 auto; padding: 1.5rem 1rem; }
  .search-bar {
    display: flex; gap: 0.5rem; margin-bottom: 1.5rem; flex-wrap: wrap;
  }
  .search-bar input {
    flex: 1; min-width: 180px;
    padding: 0.55rem 0.9rem;
    border: 1px solid #cbd5e0; border-radius: 6px;
    font-size: 0.95rem;
  }
  .search-bar input:focus { outline: 2px solid #0055a5; border-color: transparent; }
  .member-bar {
    display: flex; gap: 0.5rem; align-items: center; flex-wrap: wrap;
  }
  .member-bar label { font-size: 0.9rem; color: #4a5568; }
  .member-bar input {
    width: 100px; padding: 0.5rem 0.7rem;
    border: 1px solid #cbd5e0; border-radius: 6px;
    font-size: 0.95rem;
  }
  btn, button, .btn {
    padding: 0.5rem 1rem; border: none; border-radius: 6px;
    cursor: pointer; font-size: 0.9rem; font-weight: 600;
    transition: background 0.15s;
  }
  .btn-primary { background: #0055a5; color: white; }
  .btn-primary:hover { background: #003f7f; }
  .btn-success { background: #276749; color: white; }
  .btn-success:hover { background: #1e4d35; }
  .btn-warning { background: #b7791f; color: white; }
  .btn-warning:hover { background: #975a16; }
  .btn-sm { padding: 0.3rem 0.7rem; font-size: 0.82rem; }
  .card {
    background: white; border-radius: 10px;
    box-shadow: 0 1px 4px rgba(0,0,0,0.1);
    margin-bottom: 1rem; overflow: hidden;
  }
  .card-header {
    background: #ebf4ff; padding: 0.75rem 1rem;
    font-weight: 700; font-size: 1rem;
    border-bottom: 1px solid #bee3f8;
    color: #2c5282;
  }
  table { width: 100%; border-collapse: collapse; }
  th {
    background: #f7fafc; padding: 0.6rem 0.9rem;
    text-align: left; font-size: 0.82rem;
    color: #4a5568; text-transform: uppercase;
    border-bottom: 2px solid #e2e8f0;
  }
  td { padding: 0.65rem 0.9rem; border-bottom: 1px solid #f0f0f0; font-size: 0.9rem; }
  tr:last-child td { border-bottom: none; }
  tr:hover td { background: #f7fafc; }
  .badge {
    display: inline-block; padding: 0.2rem 0.6rem;
    border-radius: 999px; font-size: 0.75rem; font-weight: 700;
  }
  .badge-green  { background: #c6f6d5; color: #22543d; }
  .badge-gray   { background: #e2e8f0; color: #4a5568; }
  .badge-red    { background: #fed7d7; color: #742a2a; }
  .badge-yellow { background: #fefcbf; color: #744210; }
  .alert {
    padding: 0.8rem 1rem; border-radius: 8px;
    margin-bottom: 1rem; font-size: 0.9rem;
  }
  .alert-success { background: #c6f6d5; color: #22543d; border-left: 4px solid #38a169; }
  .alert-error   { background: #fed7d7; color: #742a2a; border-left: 4px solid #e53e3e; }
  .alert-info    { background: #bee3f8; color: #2c5282; border-left: 4px solid #3182ce; }
  .empty { text-align: center; color: #a0aec0; padding: 2rem; font-style: italic; }
  .overdue-row td { background: #fff5f5 !important; }
  @media (max-width: 600px) {
    header { flex-direction: column; align-items: flex-start; gap: 0.3rem; }
    nav a  { margin-left: 0; margin-right: 1rem; }
    table  { font-size: 0.82rem; }
    th, td { padding: 0.5rem 0.6rem; }
  }
</style>
)";

// ── Page wrapper ─────────────────────────────────────────────────────────────
static std::string page(const std::string& title,
                         const std::string& body,
                         const std::string& memberId = "") {
    std::ostringstream h;
    h << "<!DOCTYPE html><html lang='id'><head>"
      << "<meta charset='UTF-8'>"
      << "<meta name='viewport' content='width=device-width, initial-scale=1'>"
      << "<title>" << esc(title) << " — UGM Library</title>"
      << CSS
      << "</head><body>"
      << "<header>"
      << "  <div>"
      << "    <h1>📚 UGM Library</h1>"
      << "    <p>Sistem Peminjaman Buku DTETI UGM</p>"
      << "  </div>"
      << "  <nav>"
      << "    <a href='/'>Beranda</a>"
      << (memberId.empty() ? "" : "    <a href='/me?id=" + memberId + "'>Pinjaman Saya</a>")
      << "  </nav>"
      << "</header>"
      << "<div class='container'>" << body << "</div>"
      << "</body></html>";
    return h.str();
}

// ── Helper: form pencarian + member ID ───────────────────────────────────────
static std::string searchForm(const std::string& q, const std::string& memberId) {
    std::ostringstream f;
    f << "<form method='GET' action='/search' class='search-bar'>"
      << "  <input name='q' placeholder='Cari judul atau pengarang...' value='"
      << esc(q) << "'>"
      << "  <button type='submit' class='btn btn-primary'>🔍 Cari</button>"
      << "  <a href='/' class='btn btn-primary' style='text-decoration:none'>Semua Buku</a>"
      << "</form>"
      << "<div class='member-bar' style='margin-bottom:1rem'>"
      << "  <label>Member ID:</label>"
      << "  <input id='mid' type='number' placeholder='contoh: 1' value='" << esc(memberId) << "'>"
      << "  <button class='btn btn-primary btn-sm' "
      << "    onclick=\"location.href='/me?id='+document.getElementById('mid').value\">"
      << "    Lihat Pinjaman Saya</button>"
      << "</div>";
    return f.str();
}

// ── Helper: tabel buku ────────────────────────────────────────────────────────
static std::string booksTable(const std::vector<Book>& books,
                               const std::string& memberId) {
    if (books.empty())
        return "<div class='empty'>Tidak ada buku yang ditemukan.</div>";

    std::ostringstream t;
    t << "<table><thead><tr>"
      << "<th>ID</th><th>Judul</th><th>Pengarang</th><th>Status</th><th>Aksi</th>"
      << "</tr></thead><tbody>";

    for (const auto& b : books) {
        t << "<tr><td>" << b.getId() << "</td>"
          << "<td>" << esc(b.getTitle()) << "</td>"
          << "<td>" << esc(b.getAuthor()) << "</td>"
          << "<td>";
        if (b.isAvailable())
            t << "<span class='badge badge-green'>Tersedia</span>";
        else
            t << "<span class='badge badge-gray'>Dipinjam</span>";
        t << "</td><td>";
        if (b.isAvailable()) {
            t << "<form method='POST' action='/borrow' style='display:inline'>"
              << "<input type='hidden' name='bookId' value='" << b.getId() << "'>"
              << "<input type='hidden' name='memberId' value='" << esc(memberId) << "'>"
              << "<button type='submit' class='btn btn-success btn-sm' "
              << (memberId.empty() ? "onclick=\"alert('Masukkan Member ID terlebih dahulu!');return false\"" : "")
              << ">Pinjam</button></form>";
        } else {
            t << "<span style='color:#a0aec0;font-size:0.82rem'>—</span>";
        }
        t << "</td></tr>";
    }
    t << "</tbody></table>";
    return t.str();
}

// ════════════════════════════════════════════════════════════════════════════
//  GET /
// ════════════════════════════════════════════════════════════════════════════
static void handleIndex(const httplib::Request& req,
                         httplib::Response& res,
                         BookRepository& bookRepo) {
    std::string memberId = req.get_param_value("mid");
    std::string msg      = req.get_param_value("msg");
    std::string msgType  = req.get_param_value("type");

    auto books = bookRepo.listAll();

    std::ostringstream body;

    // Alert
    if (!msg.empty()) {
        std::string cls = (msgType == "err") ? "alert-error"
                        : (msgType == "ok")  ? "alert-success"
                                             : "alert-info";
        body << "<div class='alert " << cls << "'>" << esc(msg) << "</div>";
    }

    body << searchForm("", memberId);

    // Statistik
    int available = 0;
    for (const auto& b : books) if (b.isAvailable()) available++;
    body << "<div style='margin-bottom:1rem;color:#4a5568;font-size:0.9rem'>"
         << "Total: <strong>" << books.size() << "</strong> buku | "
         << "Tersedia: <strong>" << available << "</strong> | "
         << "Dipinjam: <strong>" << (books.size() - available) << "</strong>"
         << "</div>";

    body << "<div class='card'>"
         << "<div class='card-header'>📖 Daftar Buku</div>"
         << booksTable(books, memberId)
         << "</div>";

    res.set_content(page("Beranda", body.str(), memberId), "text/html; charset=utf-8");
}

// ════════════════════════════════════════════════════════════════════════════
//  GET /search?q=...
// ════════════════════════════════════════════════════════════════════════════
static void handleSearch(const httplib::Request& req,
                          httplib::Response& res,
                          BookRepository& bookRepo) {
    std::string q        = req.get_param_value("q");
    std::string memberId = req.get_param_value("mid");

    if (q.empty()) {
        res.set_redirect("/");
        return;
    }

    auto results = bookRepo.search(q);

    std::ostringstream body;
    body << searchForm(q, memberId);
    body << "<div class='card'>"
         << "<div class='card-header'>🔍 Hasil pencarian: \"" << esc(q) << "\" ("
         << results.size() << " ditemukan)</div>"
         << booksTable(results, memberId)
         << "</div>";

    res.set_content(page("Pencarian", body.str(), memberId), "text/html; charset=utf-8");
}

// ════════════════════════════════════════════════════════════════════════════
//  POST /borrow
// ════════════════════════════════════════════════════════════════════════════
static void handleBorrow(const httplib::Request& req,
                          httplib::Response& res,
                          BookRepository& bookRepo,
                          MemberRepository& memberRepo,
                          LoanRepository& loanRepo) {
    std::string memberIdStr = req.get_param_value("memberId");
    std::string bookIdStr   = req.get_param_value("bookId");

    // Validasi parameter
    if (memberIdStr.empty() || bookIdStr.empty()) {
        res.set_redirect("/?msg=Parameter+memberId+dan+bookId+wajib+diisi&type=err");
        return;
    }

    int memberId, bookId;
    try {
        memberId = std::stoi(memberIdStr);
        bookId   = std::stoi(bookIdStr);
    } catch (...) {
        res.set_redirect("/?msg=ID+tidak+valid&type=err");
        return;
    }

    // Cek member
    auto member = memberRepo.findById(memberId);
    if (!member) {
        res.set_redirect("/?msg=Member+ID+" + memberIdStr + "+tidak+ditemukan&type=err&mid=" + memberIdStr);
        return;
    }

    // Cek buku
    auto book = bookRepo.findById(bookId);
    if (!book) {
        res.set_redirect("/?msg=Buku+ID+" + bookIdStr + "+tidak+ditemukan&type=err&mid=" + memberIdStr);
        return;
    }

    if (!book->isAvailable()) {
        res.set_redirect("/?msg=Buku+" + esc(book->getTitle()) + "+sedang+dipinjam&type=err&mid=" + memberIdStr);
        return;
    }

    // Simpan loan
    int loanId = loanRepo.nextId();
    Loan loan(loanId, bookId, memberId, todayDate(), dueDateFromToday(14), false);
    loanRepo.save(loan);

    // Update status buku
    Book updated(bookId, book->getTitle(), book->getAuthor(), false);
    bookRepo.update(updated);

    std::string msg = "Berhasil+meminjam+%22" + book->getTitle() + "%22.+Batas+kembali:+" + dueDateFromToday(14);
    res.set_redirect("/me?id=" + memberIdStr + "&msg=" + msg + "&type=ok");
}

// ════════════════════════════════════════════════════════════════════════════
//  POST /return
// ════════════════════════════════════════════════════════════════════════════
static void handleReturn(const httplib::Request& req,
                          httplib::Response& res,
                          BookRepository& bookRepo,
                          LoanRepository& loanRepo) {
    std::string loanIdStr   = req.get_param_value("loanId");
    std::string memberIdStr = req.get_param_value("memberId");

    if (loanIdStr.empty()) {
        res.set_redirect("/?msg=Parameter+loanId+wajib+diisi&type=err");
        return;
    }

    int loanId;
    try { loanId = std::stoi(loanIdStr); }
    catch (...) {
        res.set_redirect("/?msg=loanId+tidak+valid&type=err");
        return;
    }

    auto loanOpt = loanRepo.findById(loanId);
    if (!loanOpt) {
        res.set_redirect("/?msg=Pinjaman+tidak+ditemukan&type=err");
        return;
    }
    if (loanOpt->isReturned()) {
        res.set_redirect("/me?id=" + memberIdStr + "&msg=Buku+sudah+dikembalikan+sebelumnya&type=err");
        return;
    }

    // Update loan
    Loan updated = *loanOpt;
    updated.setReturned(true);
    loanRepo.update(updated);

    // Update buku jadi tersedia
    auto bookOpt = bookRepo.findById(updated.getBookId());
    if (bookOpt) {
        Book bk(bookOpt->getId(), bookOpt->getTitle(), bookOpt->getAuthor(), true);
        bookRepo.update(bk);
    }

    std::string msg = loanOpt->isOverdue()
        ? "Buku+dikembalikan+(TERLAMBAT)"
        : "Buku+berhasil+dikembalikan";
    std::string type = loanOpt->isOverdue() ? "err" : "ok";
    res.set_redirect("/me?id=" + memberIdStr + "&msg=" + msg + "&type=" + type);
}

// ════════════════════════════════════════════════════════════════════════════
//  GET /me?id=...
// ════════════════════════════════════════════════════════════════════════════
static void handleMe(const httplib::Request& req,
                      httplib::Response& res,
                      BookRepository& bookRepo,
                      MemberRepository& memberRepo,
                      LoanRepository& loanRepo) {
    std::string idStr  = req.get_param_value("id");
    std::string msg    = req.get_param_value("msg");
    std::string msgType = req.get_param_value("type");

    if (idStr.empty()) {
        res.set_redirect("/?msg=Parameter+id+wajib+diisi&type=err");
        return;
    }

    int memberId;
    try { memberId = std::stoi(idStr); }
    catch (...) {
        res.set_redirect("/?msg=ID+tidak+valid&type=err");
        return;
    }

    auto member = memberRepo.findById(memberId);
    if (!member) {
        res.set_redirect("/?msg=Member+ID+" + idStr + "+tidak+ditemukan&type=err");
        return;
    }

    auto loans = loanRepo.findByMember(memberId);

    std::ostringstream body;

    // Alert
    if (!msg.empty()) {
        std::string cls = (msgType == "err") ? "alert-error"
                        : (msgType == "ok")  ? "alert-success"
                                             : "alert-info";
        body << "<div class='alert " << cls << "'>" << esc(msg) << "</div>";
    }

    body << "<div class='card'>"
         << "<div class='card-header'>👤 Halo, " << esc(member->getName()) << "!</div>"
         << "<table><thead><tr>"
         << "<th>Loan ID</th><th>Judul Buku</th><th>Dipinjam</th>"
         << "<th>Batas Kembali</th><th>Status</th><th>Aksi</th>"
         << "</tr></thead><tbody>";

    // Hitung due soon (≤5 hari)
    int dueSoon = 0;
    bool hasActive = false;

    for (const auto& l : loans) {
        if (l.isReturned()) continue;
        hasActive = true;

        auto bookOpt = bookRepo.findById(l.getBookId());
        std::string title = bookOpt ? bookOpt->getTitle() : "?";

        bool overdue = l.isOverdue();
        std::string rowClass = overdue ? " class='overdue-row'" : "";

        body << "<tr" << rowClass << ">"
             << "<td>" << l.getLoanId() << "</td>"
             << "<td>" << esc(title) << "</td>"
             << "<td>" << l.getBorrowDate() << "</td>"
             << "<td>" << l.getDueDate() << "</td>"
             << "<td>";
        if (overdue)
            body << "<span class='badge badge-red'>TERLAMBAT</span>";
        else
            body << "<span class='badge badge-yellow'>Aktif</span>";
        body << "</td><td>"
             << "<form method='POST' action='/return' style='display:inline'>"
             << "<input type='hidden' name='loanId' value='" << l.getLoanId() << "'>"
             << "<input type='hidden' name='memberId' value='" << memberId << "'>"
             << "<button type='submit' class='btn btn-warning btn-sm'>Kembalikan</button>"
             << "</form></td></tr>";

        if (!overdue) {
            // Hitung hari sampai due
            // Sederhana: cukup tampilkan saja, tidak hitung hari
            dueSoon++;
        }
    }

    if (!hasActive) {
        body << "<tr><td colspan='6' class='empty'>Tidak ada pinjaman aktif.</td></tr>";
    }

    body << "</tbody></table></div>";

    // Riwayat yang sudah dikembalikan
    bool hasReturned = false;
    std::ostringstream history;
    history << "<div class='card' style='margin-top:1rem'>"
            << "<div class='card-header'>📋 Riwayat Pengembalian</div>"
            << "<table><thead><tr>"
            << "<th>Loan ID</th><th>Judul Buku</th><th>Dipinjam</th><th>Dikembalikan</th>"
            << "</tr></thead><tbody>";

    for (const auto& l : loans) {
        if (!l.isReturned()) continue;
        hasReturned = true;
        auto bookOpt = bookRepo.findById(l.getBookId());
        std::string title = bookOpt ? bookOpt->getTitle() : "?";
        history << "<tr><td>" << l.getLoanId() << "</td>"
                << "<td>" << esc(title) << "</td>"
                << "<td>" << l.getBorrowDate() << "</td>"
                << "<td><span class='badge badge-green'>Kembali</span></td></tr>";
    }
    if (!hasReturned)
        history << "<tr><td colspan='4' class='empty'>Belum ada riwayat pengembalian.</td></tr>";
    history << "</tbody></table></div>";

    body << history.str();

    // Tombol kembali ke beranda dengan member ID
    body << "<div style='margin-top:1rem'>"
         << "<a href='/?mid=" << memberId << "' class='btn btn-primary' style='text-decoration:none'>"
         << "← Kembali ke Beranda</a></div>";

    res.set_content(page("Pinjaman Saya", body.str(), std::to_string(memberId)),
                    "text/html; charset=utf-8");
}

// ════════════════════════════════════════════════════════════════════════════
//  MAIN
// ════════════════════════════════════════════════════════════════════════════
int main() {
    std::filesystem::create_directories("data");

    BookRepository   bookRepo;
    MemberRepository memberRepo;
    LoanRepository   loanRepo;

    httplib::Server svr;

    // GET /
    svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
        handleIndex(req, res, bookRepo);
    });

    // GET /search
    svr.Get("/search", [&](const httplib::Request& req, httplib::Response& res) {
        handleSearch(req, res, bookRepo);
    });

    // POST /borrow
    svr.Post("/borrow", [&](const httplib::Request& req, httplib::Response& res) {
        handleBorrow(req, res, bookRepo, memberRepo, loanRepo);
    });

    // POST /return
    svr.Post("/return", [&](const httplib::Request& req, httplib::Response& res) {
        handleReturn(req, res, bookRepo, loanRepo);
    });

    // GET /me
    svr.Get("/me", [&](const httplib::Request& req, httplib::Response& res) {
        handleMe(req, res, bookRepo, memberRepo, loanRepo);
    });

    // 404 handler
    svr.set_error_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            "<!DOCTYPE html><html><body>"
            "<h2>404 — Halaman tidak ditemukan</h2>"
            "<a href='/'>Kembali ke Beranda</a>"
            "</body></html>",
            "text/html; charset=utf-8");
    });

    std::cout << "╔══════════════════════════════════════════╗\n"
              << "║   UGM Library Web Server  — Port 8080   ║\n"
              << "╚══════════════════════════════════════════╝\n"
              << "  Buka browser: http://localhost:8080\n"
              << "  Tekan Ctrl+C untuk berhenti.\n\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}
