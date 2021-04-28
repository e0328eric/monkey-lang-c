CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wconversion
INCLUDE = -lreadline

SRC_DIR = ./src
OBJ_DIR = ./obj
TEST_DIR = ./tests
TEST_OBJ_DIR = ./obj_tests

TARGET = monkey
TEST_TARGET = monkey_test

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS = $(SRCS:.c=.o)

OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS))
DEPS = $(OBJECTS:.o=.d)

TEST_SRC = $(TEST_DIR)/main.c
TEST_OBJ = $(TEST_OBJ_DIR)/main.o
DEPS += $(TEST_OBJ_DIR)/main.d

all: main

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -MD $(LDFLAGS)

main : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(TEST_OBJ) : $(TEST_SRC)
	$(CC) $(CFLAGS) -c $< -o $@ -MD $(LDFLAGS)

test : $(TEST_OBJ)
	$(CC) $(CFLAGS) $(TEST_OBJ) -o $(TEST_TARGET) $(LDFLAGS)

.PHONY: clean all
clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)
	rm -f $(TEST_OBJ) $(TEST_TARGET)

-include $(DEPS)

