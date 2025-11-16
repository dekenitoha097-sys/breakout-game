# ========================
# Configuration
# ========================

CXX = g++
CXXFLAGS = -I include -Wall -O2

# Liens Raylib (Windows et Linux)
LDFLAGS_WIN = -L C:/raylib/w64devkit/lib -lraylib -lopengl32 -lgdi32 -lwinmm
LDFLAGS_LINUX = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_DIR = src
OBJ_DIR = obj
BIN = prog

# ========================
# Recherche automatique des sources
# ========================
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# ========================
# Règles
# ========================

all: $(BIN)

# ----- Link -----
$(BIN): $(OBJ_FILES)
ifeq ($(OS),Windows_NT)
	$(CXX) $^ -o $@ $(LDFLAGS_WIN)
else
	$(CXX) $^ -o $@ $(LDFLAGS_LINUX)
endif

# ----- Compilation .cpp -> .o -----
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ----- Nettoyage -----
clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
