CC = gcc
CFLAGS = -g -Wall -Wextra
OBJ_DIR = obj
SRC_DIR = src
TEST_DIR = tests

SRC_FILES = $(SRC_DIR)/board.c $(SRC_DIR)/game.c $(SRC_DIR)/move.c $(SRC_DIR)/attack_data.c $(SRC_DIR)/move_gen.c
TEST_FILES = $(TEST_DIR)/attack_and_move_tests.c $(TEST_DIR)/move_gen_tests.c $(TEST_DIR)/tests.c

# notdir removes directory prefix, patsubst adds obj file directory and replaces .c with .o
SRC_OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILES)))
TEST_OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(TEST_FILES)))
GAME_OBJ_FILES = $(SRC_OBJ_FILES) obj/springer.o

# Default target
tests: springer_tests.exe
game: springer.exe

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link
springer_tests.exe: $(TEST_OBJ_FILES) $(SRC_OBJ_FILES)
	$(CC) -o $@ $^

springer.exe: $(GAME_OBJ_FILES)
	$(CC) -o $@ $^

# Clean
clean:
	rm -rf $(OBJ_DIR) springer_tests.exe springer.exe