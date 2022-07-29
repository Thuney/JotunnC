# C++ Compiler
CXX      := -gcc
# Compiler Flags
#-pedantic-errors
CXXFLAGS := -Wall -Wextra -std=c++17

RPATH_LINKAGE := -Wl,-rpath,$(CURDIR)/../../../.build/lib/

LDFLAGS  := -L/usr/lib -lstdc++ -lboost_unit_test_framework -lm -lpthread -lfmt -L$(CURDIR)/../../../.build/lib/ -ljotunn $(RPATH_LINKAGE)

BUILD    	:= ./.build
TEST_DIR  	:= $(BUILD)/test
TARGET_TEST := vector_tests

# Includes
INCLUDE  	:= -I../../../include/

# Unit Test Files
TEST_FILE := ./vectortests.cpp

all: build

.PHONY: all build clean debug release

build:
	@mkdir -p $(TEST_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) -o $(TEST_DIR)/$(TARGET_TEST) $(TEST_FILE)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(TEST_DIR)/*
