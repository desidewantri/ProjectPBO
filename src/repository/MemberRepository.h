#pragma once
#include "../models/Member.h"
#include <vector>
#include <string>
#include <optional>

// MemberRepository mengelola operasi CRUD member ke file members.csv
class MemberRepository {
private:
    std::string fileName_;  // path ke file CSV

public:
    explicit MemberRepository(const std::string& fileName = "data/members.csv");

    void save(const Member& member);                 // tambah member baru
    void remove(int id);                             // hapus member
    void update(const Member& member);               // perbarui data member
    std::optional<Member> findById(int id) const;    // cari member by ID
    std::vector<Member> listAll() const;             // ambil semua member

    // Generate ID berikutnya
    int nextId() const;
};
