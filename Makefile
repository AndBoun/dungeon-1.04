CC = gcc
CFLAGS = -g -Wall -Werror
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj

# Create directory structure for object files
DIRS = $(OBJ_DIR) $(OBJ_DIR)/core $(OBJ_DIR)/generation $(OBJ_DIR)/io $(OBJ_DIR)/path_finding

# Source files
CORE_SRC = $(wildcard $(SRC_DIR)/core/*.c)
GEN_SRC = $(wildcard $(SRC_DIR)/generation/*.c)
IO_SRC = $(wildcard $(SRC_DIR)/io/*.c)
PATH_FINDING_SRC = $(wildcard $(SRC_DIR)/path_finding/*.c)

MAIN_SRC = $(SRC_DIR)/main.c

# Object files
CORE_OBJ = $(patsubst $(SRC_DIR)/core/%.c, $(OBJ_DIR)/core/%.o, $(CORE_SRC))
GEN_OBJ = $(patsubst $(SRC_DIR)/generation/%.c, $(OBJ_DIR)/generation/%.o, $(GEN_SRC))
IO_OBJ = $(patsubst $(SRC_DIR)/io/%.c, $(OBJ_DIR)/io/%.o, $(IO_SRC))
PATH_FINDING_OBJ = $(patsubst $(SRC_DIR)/path_finding/%.c, $(OBJ_DIR)/path_finding/%.o, $(PATH_FINDING_SRC))

MAIN_OBJ = $(OBJ_DIR)/main.o

# All objects
ALL_OBJ = $(CORE_OBJ) $(GEN_OBJ) $(IO_OBJ) $(PATH_FINDING_OBJ) $(MAIN_OBJ)

# Target executable
TARGET = dungeon

all: dirs $(TARGET)

dirs:
	@mkdir -p $(DIRS)

$(TARGET): $(ALL_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compile main.c
$(OBJ_DIR)/main.o: $(MAIN_SRC)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile core files
$(OBJ_DIR)/core/%.o: $(SRC_DIR)/core/%.c $(INCLUDE_DIR)/dungeon.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile generation files
$(OBJ_DIR)/generation/%.o: $(SRC_DIR)/generation/%.c $(INCLUDE_DIR)/dungeon.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile IO files
$(OBJ_DIR)/io/%.o: $(SRC_DIR)/io/%.c $(INCLUDE_DIR)/dungeon.h $(INCLUDE_DIR)/load_save.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Compile path_finding files
$(OBJ_DIR)/path_finding/%.o: $(SRC_DIR)/path_finding/%.c $(INCLUDE_DIR)/dijkstra.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all dirs clean