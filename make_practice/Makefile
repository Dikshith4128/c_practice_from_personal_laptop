cc = gcc
CFLAGS = -Wall -Wextra
SRC = code/hello.c
OUT_DIR = out
OUT = $(OUT_DIR)/hello


all: $(OUT)


$(OUT): $(SRC)
	mkdir -p $(OUT_DIR)
	$(cc) $(CFLAGS) $< -o $@


clean:
	rm -f $(OUT)
