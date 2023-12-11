CXX=c++
CXXFLAGS=-Wall -Wextra -Wpedantic -std=c++23 -O3
INCLUDE=-I./src -I./solutions
LIBS=$(shell pkg-config --libs fmt)

BUILDPATH=./build
OBJ_DIR=$(BUILDPATH)/obj

CONFIG_BUILD_PATH=$(BUILDPATH)/config.in
INPUT_DEFINES_HPP=input_defines.in.hpp
INPUT_DEFINES_CONFIG_PATH=$(CONFIG_BUILD_PATH)/$(INPUT_DEFINES_HPP)

SRC_AOC=./src/solution.cpp ./src/aocprogram.cpp ./src/parsing.cpp ./src/aoc.cpp
SRC_SOLUTIONS=$(wildcard ./solutions/2023/*.cpp)
OBJ_FILES=$(patsubst ./solutions/2023/%.cpp,$(OBJ_DIR)/%.o,$(SRC_SOLUTIONS))

all: configure aoc

configure: buildpath configure_stubs configure_inputs

stubs: configure_stubs

configure_inputs:
	./scripts/define_inputs.py $(INPUT_DEFINES_CONFIG_PATH)

configure_stubs:
	mkdir -p ./stubs
	./scripts/make_stubs.py ./stubs

buildpath:
	mkdir -p $(BUILDPATH)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(CONFIG_BUILD_PATH)

compileflags:
	echo $(CXXFLAGS) $(INCLUDE) -I./stubs $(LIBS) | sed -e 's/c++23/c++2b/' -e 's/ /\n/g' > ./compile_flags.txt

aoc: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -I$(CONFIG_BUILD_PATH) $(LIBS) -DINPUTS_CONFIG_IN=\"$(INPUT_DEFINES_HPP)\" -o "${BUILDPATH}/aoc" $(SRC_AOC) $^

$(OBJ_DIR)/%.o: ./solutions/2023/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

.PHONY: clean stubs

clean:
	rm -r ${BUILDPATH}
