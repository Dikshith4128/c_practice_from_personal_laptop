# Compiler
CC = gcc

# Source and Output Directories
SRC_DIR = .
OUT_DIR = ../out

# Get all .c files in code/ folder
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)

# Convert .c filenames to corresponding .out filenames in the out/ directory
OUT_FILES := $(patsubst %.c, $(OUT_DIR)/%.out, $(notdir $(SRC_FILES)))

# Default rule to compile all .c files
all: $(OUT_FILES)

# Compile each .c file into ../out/filename.out
$(OUT_DIR)/%.out: %.c
	$(CC) -o $@ $<

# Clean rule to remove all compiled files
clean:
	rm -f $(OUT_DIR)/*.out

# Rebuild rule (clean and make all)
rebuild: clean all

