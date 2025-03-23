# -----------------------------------------------------------------------------
# Makefile for Compiling and Building Homework 3
#
# This Makefile builds executables for the pathfinding and path following assignment.
#
# Adapted from the sample Makefile provided by Dr. David L. Roberts.
#
# Author: Miles Hollifield
# Date: 3/20/2025
# -----------------------------------------------------------------------------

# Source Files by Component
MAIN_SRC = hw3.cpp
SMALL_GRAPH_SRC = source/SmallGraph.cpp
LARGE_GRAPH_SRC = source/LargeGraph.cpp

# Source files for path following behaviors
BEHAVIOR_SRC = source/Arrive.cpp source/Align.cpp

# Object files
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
SMALL_GRAPH_OBJ = $(SMALL_GRAPH_SRC:.cpp=.o)
LARGE_GRAPH_OBJ = $(LARGE_GRAPH_SRC:.cpp=.o)
BEHAVIOR_OBJ = $(BEHAVIOR_SRC:.cpp=.o)

# Flags and Libraries
CXXFLAGS = -std=c++17 -I. -Iheaders
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
all: hw3 small_graph large_graph

uname_s := $(shell uname -s)

# Build main application
hw3: $(MAIN_OBJ) $(BEHAVIOR_OBJ)
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

# Build small graph test
small_graph: $(SMALL_GRAPH_OBJ)
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

# Build large graph test
large_graph: $(LARGE_GRAPH_OBJ)
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

# Compile sources
%.o: %.cpp
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) $(CXXFLAGS) -c $< -o $@ $(MACOS_INCLUDE)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) $(CXXFLAGS) -c $< -o $@ $(UBUNTU_INCLUDE)
endif

# Clean Build Files
.PHONY: clean
clean:
	rm -f $(MAIN_OBJ) $(SMALL_GRAPH_OBJ) $(LARGE_GRAPH_OBJ) $(BEHAVIOR_OBJ) hw3 small_graph large_graph
	rm -f *.dat