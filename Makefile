CC     := gcc
CFLAGS := -std=c99 -Wall -Wextra -O2
SRC    := src/main.c src/hexview.c src/utils.c
TARGET := hexview

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
endif

.PHONY: all clean
all: $(TARGET)

.PHONY: test
test: $(TARGET)
	@bash tests/smoke.sh

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	del /Q $(TARGET) 2>nul || rm -f $(TARGET)
