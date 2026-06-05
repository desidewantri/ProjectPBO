CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -I src

SRC_DIR  := src
OBJ_DIR  := build

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

ifeq ($(OS),Windows_NT)
    LDFLAGS_WEB := -lws2_32
else
    LDFLAGS_WEB :=
endif

.PHONY: all seed admin web clean dirs

all: seed admin web

dirs:
	@mkdir -p $(OBJ_DIR)/models $(OBJ_DIR)/repository data

seed: dirs $(CORE_OBJS) $(OBJ_DIR)/seed.o
	$(CXX) $(CXXFLAGS) -o seed $(CORE_OBJS) $(OBJ_DIR)/seed.o
	@echo "\n✓ Built: ./seed"

admin: dirs $(CORE_OBJS) $(OBJ_DIR)/admin.o
	$(CXX) $(CXXFLAGS) -o admin $(CORE_OBJS) $(OBJ_DIR)/admin.o
	@echo "\n✓ Built: ./admin"

web: dirs $(CORE_OBJS) $(OBJ_DIR)/web.o
	$(CXX) $(CXXFLAGS) -o web $(CORE_OBJS) $(OBJ_DIR)/web.o $(LDFLAGS_WEB)
	@echo "\n✓ Built: ./web"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/seed.o: $(SRC_DIR)/seed.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/admin.o: $(SRC_DIR)/admin.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/web.o: $(SRC_DIR)/web.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) seed admin web seed.exe admin.exe web.exe
	@echo "Cleaned build artifacts."

clean-data:
	rm -rf data/
	@echo "Cleaned data files."
