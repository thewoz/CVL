INCLUDE_INSTALL_PATH=/usr/local/include
LIBRARY_INSTALL_PATH=/usr/local/lib

LIBRARY_NAME=cvl

# Test build configuration
CXX      ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4)
OPENCV_LIBS   = $(shell pkg-config --libs opencv4)
OPENCV_LIBDIR = $(shell pkg-config --variable=libdir opencv4)
TEST_SRC = tests/test_cvl.cpp
TEST_BIN = tests/test_cvl

all: install

install:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
	@ln -s $(shell pwd)/include $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)

uninstall:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)

# Build and run the smoke-test suite against the working-tree headers
# (_DEBUG_CVL_LIBS_ makes the umbrella header use local "..." includes).
test:
	$(CXX) $(CXXFLAGS) -D_DEBUG_CVL_LIBS_ $(OPENCV_CFLAGS) -Iinclude $(TEST_SRC) -o $(TEST_BIN) $(OPENCV_LIBS) -Wl,-rpath,$(OPENCV_LIBDIR)
	./$(TEST_BIN)

clean:
	@rm -f $(TEST_BIN)

.PHONY: all install uninstall test clean
