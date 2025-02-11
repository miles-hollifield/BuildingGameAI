# -----------------------------------------------------------------------------
# Makefile for Compiling and Building Homework 2 (Parts 1 & 2)
#
# This Makefile builds two separate executables:
# - `hw2pt1` for Variable Matching Steering Behaviors
# - `hw2pt2` for Arrive and Align with Breadcrumbs
#
# Adapted from the sample Makefile provided by Dr. David L. Roberts.
#
# Author: Miles Hollifield
# Date: 1/28/2025
# -----------------------------------------------------------------------------

# Source Files
HW2PT1_SRC = hw2pt1.cpp \
             VariableMatchingSteeringBehaviors/PositionMatching.cpp \
             VariableMatchingSteeringBehaviors/OrientationMatching.cpp \
             VariableMatchingSteeringBehaviors/VelocityMatching.cpp \
             VariableMatchingSteeringBehaviors/RotationMatching.cpp

HW2PT2_SRC = hw2pt2.cpp

# Object Files
HW2PT1_OBJ = $(HW2PT1_SRC:.cpp=.o)
HW2PT2_OBJ = $(HW2PT2_SRC:.cpp=.o)

# Flags and Libraries
CXXFLAGS = -std=c++17 -I./VariableMatchingSteeringBehaviors
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
all: hw2pt1 hw2pt2

uname_s := $(shell uname -s)

# Build hw2pt1
hw2pt1: $(HW2PT1_OBJ)
ifeq ($(uname_s),Darwin)
	$(MACOS_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(MACOS_LIB)
else ifeq ($(uname_s),Linux)
	$(UBUNTU_COMPILER) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(UBUNTU_LIB)
endif

# Build hw2pt2
hw2pt2: $(HW2PT2_OBJ)
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
	rm -f $(HW2PT1_OBJ) $(HW2PT2_OBJ) hw2pt1 hw2pt2
