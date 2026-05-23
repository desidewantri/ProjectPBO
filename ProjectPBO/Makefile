CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -I src

SRC_DIR  := src
OBJ_DIR  := build

# All .cpp sources (exclude seed.cpp and future main files)
CORE_SRCS := \
    $(SRC_DIR)/models/User.cpp \
    $(SRC_DIR)/models/Admin.cpp \
    $(SRC_DIR)/models/Member.cpp \
    $(SRC_DIR)/models/Book.cpp \
    $(SRC_DIR)/models/Loan.cpp \
    $(SRC_DIR)/repository/BookRepository.cpp \
    $(SRC_DIR)/repository/MemberRepository.cpp \
    $(SRC_DIR)/repository/LoanRepository.cpp

CORE_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CORE_SRCS))

# ─── Targets ──────────────────────────────────────────────────────
.PHONY: all seed clean dirs

all: seed

dirs:
	@mkdir -p $(OBJ_DIR)/models $(OBJ_DIR)/repository data

# Build seed binary
seed: dirs $(CORE_OBJS) $(OBJ_DIR)/seed.o
	$(CXX) $(CXXFLAGS) -o seed $(CORE_OBJS) $(OBJ_DIR)/seed.o
	@echo "\n✓ Built: ./seed"

# Pattern rule for core objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/seed.o: $(SRC_DIR)/seed.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) seed admin
	@echo "Cleaned build artifacts."

clean-data:
	rm -rf data/
	@echo "Cleaned data files."
