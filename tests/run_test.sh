#!/bin/bash

###############################################################################
# Test Runner Script for Door Lock System
# Usage: ./run_test.sh [test_name]
# Example: ./run_test.sh test_keypad
###############################################################################

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project directory
PROJECT_DIR="/Users/mugabo15/Mbed Programs/doorLocker"
TESTS_DIR="$PROJECT_DIR/tests"
BUILD_DIR="$PROJECT_DIR/BUILD"

# Available tests
AVAILABLE_TESTS=(
    "test_keypad"
    "test_lcd"
    "test_led"
    "test_relay"
    "test_integration"
)

###############################################################################
# Functions
###############################################################################

print_header() {
    echo -e "${BLUE}"
    echo "========================================"
    echo "  Door Lock Test Runner"
    echo "========================================"
    echo -e "${NC}"
}

print_usage() {
    echo "Usage: $0 [test_name]"
    echo ""
    echo "Available tests:"
    for test in "${AVAILABLE_TESTS[@]}"; do
        echo "  - $test"
    done
    echo ""
    echo "Examples:"
    echo "  $0 test_keypad          # Run keypad test"
    echo "  $0 all                  # Run all tests"
    echo "  $0 list                 # List available tests"
    echo ""
}

list_tests() {
    echo -e "${YELLOW}Available Tests:${NC}"
    for i in "${!AVAILABLE_TESTS[@]}"; do
        test_file="${AVAILABLE_TESTS[$i]}.cpp"
        if [ -f "$TESTS_DIR/$test_file" ]; then
            echo -e "  ${GREEN}✓${NC} $((i+1)). ${AVAILABLE_TESTS[$i]}"
        else
            echo -e "  ${RED}✗${NC} $((i+1)). ${AVAILABLE_TESTS[$i]} (file not found)"
        fi
    done
    echo ""
}

backup_main() {
    if [ -f "$PROJECT_DIR/main.cpp" ]; then
        echo -e "${YELLOW}Backing up main.cpp...${NC}"
        cp "$PROJECT_DIR/main.cpp" "$PROJECT_DIR/main.cpp.backup"
        echo -e "${GREEN}✓ Backup created${NC}"
    fi
}

restore_main() {
    if [ -f "$PROJECT_DIR/main.cpp.backup" ]; then
        echo -e "${YELLOW}Restoring main.cpp...${NC}"
        mv "$PROJECT_DIR/main.cpp.backup" "$PROJECT_DIR/main.cpp"
        echo -e "${GREEN}✓ Main restored${NC}"
    fi
}

run_test() {
    local test_name=$1
    local test_file="$TESTS_DIR/${test_name}.cpp"
    
    echo -e "${BLUE}========================================"
    echo "  Running: $test_name"
    echo -e "========================================${NC}"
    
    # Check if test file exists
    if [ ! -f "$test_file" ]; then
        echo -e "${RED}✗ Test file not found: $test_file${NC}"
        return 1
    fi
    
    # Backup main.cpp
    backup_main
    
    # Copy test file to main.cpp
    echo -e "${YELLOW}Copying test file...${NC}"
    cp "$test_file" "$PROJECT_DIR/main.cpp"
    
    # Clean previous build
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    
    # Compile
    echo -e "${YELLOW}Compiling...${NC}"
    cd "$PROJECT_DIR"
    mbed compile --target NUCLEO_L476RG --toolchain GCC_ARM
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Compilation successful!${NC}"
        
        # Find binary
        BIN_FILE=$(find "$BUILD_DIR" -name "*.bin" | head -n 1)
        
        if [ -f "$BIN_FILE" ]; then
            echo -e "${GREEN}✓ Binary created: $BIN_FILE${NC}"
            echo ""
            echo -e "${YELLOW}Next steps:${NC}"
            echo "  1. Connect your STM32 Nucleo board"
            echo "  2. Copy binary to board:"
            echo "     cp \"$BIN_FILE\" /Volumes/NODE_L476RG/"
            echo "  3. Open serial monitor at 9600 baud:"
            echo "     screen /dev/tty.usbmodem* 9600"
            echo ""
        fi
        
        # Restore main.cpp
        restore_main
        return 0
    else
        echo -e "${RED}✗ Compilation failed!${NC}"
        
        # Restore main.cpp
        restore_main
        return 1
    fi
}

run_all_tests() {
    echo -e "${BLUE}Running all tests...${NC}"
    echo ""
    
    local passed=0
    local failed=0
    
    for test in "${AVAILABLE_TESTS[@]}"; do
        if run_test "$test"; then
            ((passed++))
        else
            ((failed++))
        fi
        echo ""
        sleep 2
    done
    
    echo -e "${BLUE}========================================"
    echo "  Test Summary"
    echo -e "========================================${NC}"
    echo -e "Total tests: $((passed + failed))"
    echo -e "${GREEN}Passed: $passed${NC}"
    echo -e "${RED}Failed: $failed${NC}"
    echo ""
}

###############################################################################
# Main Script
###############################################################################

print_header

# Check if mbed CLI is available
if ! command -v mbed &> /dev/null; then
    echo -e "${RED}✗ Mbed CLI not found!${NC}"
    echo "Please install Mbed CLI:"
    echo "  pip3 install mbed-cli"
    exit 1
fi

# Parse arguments
if [ $# -eq 0 ]; then
    print_usage
    list_tests
    exit 0
fi

case "$1" in
    "list")
        list_tests
        ;;
    "all")
        run_all_tests
        ;;
    "help"|"-h"|"--help")
        print_usage
        ;;
    *)
        # Check if test exists
        if [[ " ${AVAILABLE_TESTS[@]} " =~ " $1 " ]]; then
            run_test "$1"
        else
            echo -e "${RED}✗ Unknown test: $1${NC}"
            echo ""
            print_usage
            list_tests
            exit 1
        fi
        ;;
esac

exit 0
