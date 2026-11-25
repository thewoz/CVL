INCLUDE_INSTALL_PATH=/usr/local/include
LIBRARY_INSTALL_PATH=/usr/local/lib

LIBRARY_NAME=cvl

# Test build configuration
CXX      ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
# -isystem: gli header OpenCV non devono generare warning nelle build -Wpedantic
OPENCV_CFLAGS = $(shell pkg-config --cflags opencv4 | sed 's/-I/-isystem /g')
OPENCV_LIBS   = $(shell pkg-config --libs opencv4)
OPENCV_LIBDIR = $(shell pkg-config --variable=libdir opencv4)
TEST_SRC = tests/test_cvl.cpp
TEST_BIN = tests/test_cvl
TEST_BIN_ASAN = tests/test_cvl_asan
EXAMPLE_SRC = src/main.cpp

# Static analysis tools (make lint). Homebrew's LLVM keeps clang-tidy out of the
# PATH, so it is looked for there too; override with make CLANG_TIDY=... lint
CPPCHECK   ?= cppcheck
CLANG_TIDY ?= $(firstword $(shell command -v clang-tidy) $(wildcard /usr/local/opt/llvm/bin/clang-tidy /opt/homebrew/opt/llvm/bin/clang-tidy) clang-tidy)

# Homebrew's clang-tidy does not know where the macOS SDK is
ifeq ($(shell uname), Darwin)
    TIDY_SYSROOT = -isysroot $(shell xcrun --show-sdk-path)
endif

all: install

install:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)
	@ln -s $(shell pwd)/include $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)

uninstall:
	@rm -rf $(INCLUDE_INSTALL_PATH)/$(LIBRARY_NAME)

# Compile the example program (needs the headers installed: it includes <cvl/cvl.hpp>)
example:
	@mkdir -p ~/bin
	$(CXX) $(CXXFLAGS) $(OPENCV_CFLAGS) -I$(INCLUDE_INSTALL_PATH) $(EXAMPLE_SRC) -o ~/bin/cvl $(OPENCV_LIBS) -Wl,-rpath,$(OPENCV_LIBDIR)
	@echo "Example built at ~/bin/cvl"

# Build and run the smoke-test suite against the working-tree headers
# (_DEBUG_CVL_LIBS_ makes the umbrella header use local "..." includes).
test:
	$(CXX) $(CXXFLAGS) -D_DEBUG_CVL_LIBS_ $(OPENCV_CFLAGS) -Iinclude $(TEST_SRC) -o $(TEST_BIN) $(OPENCV_LIBS) -Wl,-rpath,$(OPENCV_LIBDIR)
	./$(TEST_BIN)

# Same suite under AddressSanitizer + UndefinedBehaviorSanitizer.
# detect_leaks=0: LeakSanitizer is not supported on macOS, and OpenCV keeps
# one-time allocations alive until exit anyway. OpenCL is disabled because the
# Apple Intel GPU driver crashes on its own (see tests/README.md).
test_asan:
	$(CXX) -std=c++17 -Wall -Wextra -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -D_DEBUG_CVL_LIBS_ $(OPENCV_CFLAGS) -Iinclude $(TEST_SRC) -o $(TEST_BIN_ASAN) $(OPENCV_LIBS) -Wl,-rpath,$(OPENCV_LIBDIR)
	OPENCV_OPENCL_RUNTIME=disabled ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=print_stacktrace=1 ./$(TEST_BIN_ASAN)

# Static analysis of the library, the example and the test: cppcheck on every
# header, then clang-tidy with the checks of .clang-tidy (through the two
# programs, since the library is header-only). Any finding is an error.
# Needs the headers installed, cppcheck and clang-tidy (brew install cppcheck llvm).
# -D_H_CVL_H_ lets cppcheck read each header on its own; useInitializationList
# is suppressed because assigning the members in the constructor body is the
# style of the library.
lint:
	$(CPPCHECK) --std=c++17 --language=c++ --enable=warning,performance,portability --quiet --error-exitcode=1 --inline-suppr \
		-D_H_CVL_H_ -D_DEBUG_CVL_LIBS_ --suppress=missingIncludeSystem --suppress=missingInclude --suppress=useInitializationList \
		$(wildcard include/*.hpp) $(EXAMPLE_SRC) $(TEST_SRC)
	$(CLANG_TIDY) --quiet --warnings-as-errors='*' $(TEST_SRC) $(EXAMPLE_SRC) -- -std=c++17 $(TIDY_SYSROOT) -D_DEBUG_CVL_LIBS_ $(OPENCV_CFLAGS) -Iinclude -I$(INCLUDE_INSTALL_PATH)
	@echo "LINT OK"

clean:
	@rm -f $(TEST_BIN) $(TEST_BIN_ASAN)
	@rm -rf $(TEST_BIN_ASAN).dSYM

.PHONY: all install uninstall example test test_asan lint clean
