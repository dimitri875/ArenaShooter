CXX = g++
CXXFLAGS = -Wall
LDFLAGS = -lraylib -lgdi32 -lwinmm

SRC_DIR := src
INCLUDE_DIRS := -Iraylib -Iinclude
LIB_DIRS := -Lraylib

CONFIG ?= Debug

ifeq ($(CONFIG),Debug)
	BUILD_DIR := build/debug
	CXXFLAGS += -g -DDEBUG
else ifeq ($(CONFIG),Release)
	BUILD_DIR := build/release
	CXXFLAGS += -O2 -DNDEBUG
else
	$(error Invalid CONFIG. Use CONFIG=Debug or CONFIG=Release)
endif

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

TARGET := $(BUILD_DIR)/game.exe

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	@echo Linking $@
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIB_DIRS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $<
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -rf build

clean-debug:
	rm -rf build/debug

clean-release:
	rm -rf build/release

.PHONY: all clean clean-debug clean-release
