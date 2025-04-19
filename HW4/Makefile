# -----------------------------------------------------------------------------
# Makefile for Compiling and Building Homework 4
#
# This Makefile builds executables for decision trees, behavior trees, and learning.
#
# Author: Miles Hollifield
# Date: 4/7/2025
# -----------------------------------------------------------------------------

# Source Files by Component
MAIN_SRC = hw4.cpp
BEHAVIOR_SRC = source/Arrive.cpp source/Align.cpp
DT_SRC = source/DecisionTree.cpp source/BehaviorTree.cpp source/Monster.cpp source/DTLearning.cpp

# Object files
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
BEHAVIOR_OBJ = $(BEHAVIOR_SRC:.cpp=.o)
DT_OBJ = $(DT_SRC:.cpp=.o)

# All object files
ALL_OBJ = $(MAIN_OBJ) $(BEHAVIOR_OBJ) $(DT_OBJ)

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
all: hw4

uname_s := $(shell uname -s)

# Build main application
hw4: $(ALL_OBJ)
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
	rm -f $(ALL_OBJ) hw4
	rm -f *.dat
	rm -f behavior_data.csv