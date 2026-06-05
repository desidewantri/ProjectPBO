#include "MemberRepository.h"
#include "../database/DbUtils.h"
#include <fstream>
#include <iostream>

// Inisialisasi: buat file CSV dengan header jika belum ada
MemberRepository::MemberRepository(const std::string& fileName)
    : fileName_(fileName) {
    std::ifstream test(fileName_);
    if (!test.good()) {
        std::ofstream create(fileName_);
        create << "memberId,name,email,username,passwordHash\n";
    }
}

// Cari ID terbesar lalu tambah 1
int MemberRepository::nextId() const {
    auto all = listAll();
    if (all.empty()) return 1;
    int maxId = 0;
    for (const auto& m : all) {
        if (m.getMemberId() > maxId) maxId = m.getMemberId();
    }
    return maxId + 1;
}

// Tambah member baru ke akhir file CSV
void MemberRepository::save(const Member& member) {
    std::ofstream file(fileName_, std::ios::app);
    if (!file) { std::cerr << "Error: cannot open " << fileName_ << "\n"; return; }
    file << member.getMemberId() << ","
         << csvEscape(member.getName()) << ","
         << csvEscape(member.getEmail()) << ","
         << csvEscape(member.getUsername()) << ","
         << member.getPasswordHash() << "\n";
}

// Hapus member: tulis ulang file tanpa member yang dihapus
void MemberRepository::remove(int id) {
    auto all = listAll();
    std::ofstream file(fileName_);
    file << "memberId,name,email,username,passwordHash\n";
    for (const auto& m : all) {
        if (m.getMemberId() != id) {
            file << m.getMemberId() << ","
                 << csvEscape(m.getName()) << ","
                 << csvEscape(m.getEmail()) << ","
                 << csvEscape(m.getUsername()) << ","
                 << m.getPasswordHash() << "\n";
        }
    }
}

// Update member: tulis ulang file, ganti baris yang id-nya cocok
void MemberRepository::update(const Member& member) {
    auto all = listAll();
    std::ofstream file(fileName_);
    file << "memberId,name,email,username,passwordHash\n";
    for (const auto& m : all) {
        if (m.getMemberId() == member.getMemberId()) {
            file << member.getMemberId() << ","
                 << csvEscape(member.getName()) << ","
                 << csvEscape(member.getEmail()) << ","
                 << csvEscape(member.getUsername()) << ","
                 << member.getPasswordHash() << "\n";
        } else {
            file << m.getMemberId() << ","
                 << csvEscape(m.getName()) << ","
                 << csvEscape(m.getEmail()) << ","
                 << csvEscape(m.getUsername()) << ","
                 << m.getPasswordHash() << "\n";
        }
    }
}

// Cari member berdasarkan ID
std::optional<Member> MemberRepository::findById(int id) const {
    for (const auto& m : listAll()) {
        if (m.getMemberId() == id) return m;
    }
    return std::nullopt;
}

// Baca semua member dari CSV, skip header
std::vector<Member> MemberRepository::listAll() const {
    std::vector<Member> members;
    std::ifstream file(fileName_);
    if (!file) return members;

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = csvSplit(line);
        if (tokens.size() < 5) continue;
        int id             = std::stoi(tokens[0]);
        std::string name   = tokens[1];
        std::string email  = tokens[2];
        std::string uname  = tokens[3];
        std::string pwhash = tokens[4];
        members.emplace_back(id, name, email, uname, pwhash);
    }
    return members;
}
