CC = gcc
CFLAGS = -std=c11 -O3 -Wall -Wextra -Wpedantic -Wconversion
INCLUDE = -lreadline

SRC_DIR = ./src
OBJ_DIR = ./obj

TARGET = monkey
TEST_TARGET = monkey_test

SRCS := $(notdir $(wildcard $(SRC_DIR)/*.c))
SRCS := $(filter-out test_main.c, $(SRCS))
SRCS := $(filter-out main.c, $(SRCS))
OBJS = $(SRCS:.c=.o)
OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS))

RUN_SRCS := $(notdir $(wildcard $(SRC_DIR)/*.c))
RUN_SRCS := $(filter-out test_main.c, $(RUN_SRCS))
RUN_OBJS = $(RUN_SRCS:.c=.o)
RUN_OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(RUN_OBJS))

TEST_SRCS := $(notdir $(wildcard $(SRC_DIR)/*.c))
TEST_SRCS := $(filter-out main.c, $(TEST_SRCS))
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(TEST_OBJS))

DEPS = $(RUN_OBJECTS:.o=.d)
DEPS += $(TEST_OBJECTS:.o=.d)

all: main

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@ -MD $(LDFLAGS)

$(OBJ_DIR)/main.o : $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -MD $(LDFLAGS)

$(OBJ_DIR)/test_main.o : $(SRC_DIR)/test_main.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@ -MD $(LDFLAGS)

main : $(RUN_OBJECTS)
	$(CC) $(CFLAGS) $(RUN_OBJECTS) -o $(TARGET) $(LDFLAGS)

test : $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)

.PHONY: clean all
clean:
	rm -f $(RUN_OBJECTS) $(DEPS) $(TARGET) $(TEST_TARGET)
	rm -f $(TEST_OBJECTS) $(DEPS) $(TARGET) $(TEST_TARGET)

-include $(DEPS)

