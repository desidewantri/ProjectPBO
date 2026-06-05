#pragma once
#include "../models/Member.h"
#include <vector>
#include <string>
#include <optional>

class MemberRepository {
private:
    std::string fileName_;

public:
    explicit MemberRepository(const std::string& fileName = "data/members.csv");

    void save(const Member& member);
    void remove(int id);
    void update(const Member& member);
    std::optional<Member> findById(int id) const;
    std::vector<Member> listAll() const;

    int nextId() const;
};
