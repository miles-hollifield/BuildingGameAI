# -----------------------------------------------------------------------------
# Makefile for Compiling and Building Part 2 and Part 3 of Homework 1
#
# This Makefile builds two separate executables: `part2` and `part3`, corresponding
# to the source files `part2.cpp` and `part3.cpp`. It supports cross-platform 
# compatibility for macOS (Intel and Apple Silicon) and Linux (Ubuntu).
#
# Adapted from the sample Makefile provided by Dr. David L. Roberts.
#
# Author: Miles Hollifield
# Date: 1/26/2025
# -----------------------------------------------------------------------------

# Source Files
PART2_SRC = part2.cpp
PART3_SRC = part3.cpp

# Object Files
PART2_OBJ = $(PART2_SRC:.cpp=.o)
PART3_OBJ = $(PART3_SRC:.cpp=.o)

# Flags and Libraries
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system 

# Platform-Specific Include and Library Paths
INTELMAC_INCLUDE=-I/usr/local/include							# Intel mac
APPLESILICON_INCLUDE=-I/opt/homebrew/include					# Apple Silicon
UBUNTU_APPLESILICON_INCLUDE=-I/usr/include 						# Apple Silicon Ubuntu VM
UBUNTU_INTEL_INCLUDE=-I/usr/include 							# Intel Ubuntu VM

INTELMAC_LIB=-L/usr/local/lib									# Intel mac
APPLESILICON_LIB=-L/opt/homebrew/lib							# Apple Silicon
UBUNTU_APPLESILICON_LIB=-L/usr/lib/aarch64-linux-gnu			# Apple Silicon Ubuntu VM
UBUNTU_INTEL_LIB=-L/usr/lib/x86_64-linux-gnu					# Intel Ubuntu VM

MACOS_INCLUDE=$(APPLESILICON_INCLUDE)
MACOS_LIB=$(APPLESILICON_LIB)
UBUNTU_INCLUDE=$(UBUNTU_APPLESILICON_INCLUDE)
UBUNTU_LIB=$(UBUNTU_APPLESILICON_LIB)

MACOS_COMPILER=/usr/bin/clang++
UBUNTU_COMPILER=/usr/bin/g++

# Targets
all: part2 part3

uname_s := $(shell uname -s)

# Build part2
part2: $(PART2_OBJ)
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

# Build part3
part3: $(PART3_OBJ)
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -std=c++17 -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

# Compile sources
%.o: %.cpp
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) -std=c++17 -c $< -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) -std=c++17 -c $< -o $@ $(UBUNTU_INCLUDE)
endif

# Clean Build Files
.PHONY: clean
clean:
	rm -f $(PART2_OBJ) $(PART3_OBJ) part2 part3
