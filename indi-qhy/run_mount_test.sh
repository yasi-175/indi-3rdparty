#!/bin/bash

# QHY Mount Test Runner Script
# This script helps users run the QHY mount test program with various options

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEST_PROGRAM="$SCRIPT_DIR/qhy_mount_test"
SIMPLE_TEST_PROGRAM="$SCRIPT_DIR/qhy_mount_simple_test"
BUILD_DIR="$SCRIPT_DIR/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if program exists
check_program() {
    if [ -f "$TEST_PROGRAM" ]; then
        return 0
    elif [ -f "$BUILD_DIR/qhy_mount_test" ]; then
        TEST_PROGRAM="$BUILD_DIR/qhy_mount_test"
        return 0
    else
        return 1
    fi
}

# Function to check if simple test program exists
check_simple_program() {
    if [ -f "$SIMPLE_TEST_PROGRAM" ]; then
        return 0
    elif [ -f "$BUILD_DIR/qhy_mount_simple_test" ]; then
        SIMPLE_TEST_PROGRAM="$BUILD_DIR/qhy_mount_simple_test"
        return 0
    else
        return 1
    fi
}

# Function to build the program
build_program() {
    print_info "Building QHY Mount Test Program..."
    
    # Try CMake build first
    if [ -f "$SCRIPT_DIR/CMakeLists.txt" ]; then
        print_info "Using CMake build..."
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        
        if cmake .. && make qhy_mount_test && make qhy_mount_simple_test; then
            print_success "CMake build completed successfully"
            TEST_PROGRAM="$BUILD_DIR/qhy_mount_test"
            SIMPLE_TEST_PROGRAM="$BUILD_DIR/qhy_mount_simple_test"
            cd "$SCRIPT_DIR"
            return 0
        else
            print_warning "CMake build failed, trying standalone Makefile..."
            cd "$SCRIPT_DIR"
        fi
    fi
    
    # Try standalone Makefile
    if [ -f "$SCRIPT_DIR/Makefile.test" ]; then
        print_info "Using standalone Makefile..."
        if make -f Makefile.test; then
            print_success "Standalone build completed successfully"
            return 0
        else
            print_error "Standalone build failed"
            return 1
        fi
    fi
    
    print_error "No build system found"
    return 1
}

# Function to show usage
show_usage() {
    echo "QHY Mount Test Runner"
    echo "Usage: $0 [option]"
    echo ""
    echo "Options:"
    echo "  auto          Run automated tests (default)"
    echo "  interactive   Run in interactive mode"
    echo "  simulate      Run in simulation mode"
    echo "  simple        Run simple tests (no INDI server required)"
    echo "  build         Build the test program"
    echo "  clean         Clean build files"
    echo "  help          Show this help message"
    echo ""
    echo "Advanced options:"
    echo "  device <port> Specify device port (e.g., /dev/ttyUSB0)"
    echo ""
    echo "Examples:"
    echo "  $0                    # Run automated tests"
    echo "  $0 interactive        # Run interactive mode"
    echo "  $0 simulate           # Run in simulation mode"
    echo "  $0 simple             # Run simple tests"
    echo "  $0 device /dev/ttyUSB0 # Use specific device"
}

# Function to check dependencies
check_dependencies() {
    print_info "Checking dependencies..."
    
    # Check for required libraries
    missing_deps=()
    
    if ! pkg-config --exists indi; then
        missing_deps+=("libindi-dev")
    fi
    
    if ! pkg-config --exists libnova; then
        missing_deps+=("libnova-dev")
    fi
    
    if ! pkg-config --exists gsl; then
        missing_deps+=("libgsl-dev")
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        print_info "Install with: sudo apt-get install ${missing_deps[*]}"
        return 1
    fi
    
    print_success "All dependencies found"
    return 0
}

# Function to check permissions
check_permissions() {
    print_info "Checking device permissions..."
    
    # Check if user is in dialout group
    if ! groups | grep -q dialout; then
        print_warning "User not in dialout group"
        print_info "Add with: sudo usermod -a -G dialout \$USER"
        print_info "Then log out and back in"
    fi
    
    # Check for common device files
    for device in /dev/ttyUSB* /dev/ttyACM*; do
        if [ -e "$device" ]; then
            if [ -r "$device" ] && [ -w "$device" ]; then
                print_success "Device $device is accessible"
            else
                print_warning "Device $device exists but may not be accessible"
                print_info "Try: sudo chmod 666 $device"
            fi
        fi
    done
}

# Main script logic
main() {
    echo "QHY Mount Test Runner v1.0"
    echo "=========================="
    
    # Parse command line arguments
    case "${1:-auto}" in
        "help"|"-h"|"--help")
            show_usage
            exit 0
            ;;
        "build")
            check_dependencies || exit 1
            build_program || exit 1
            exit 0
            ;;
        "clean")
            print_info "Cleaning build files..."
            rm -rf "$BUILD_DIR"
            make -f Makefile.test clean 2>/dev/null || true
            print_success "Clean completed"
            exit 0
            ;;
    esac
    
    # Check if program exists, build if necessary
    if ! check_program; then
        print_warning "Test program not found, attempting to build..."
        check_dependencies || exit 1
        build_program || exit 1
    fi
    
    print_success "Test program found: $TEST_PROGRAM"
    
    # Check system setup
    check_permissions
    
    # Run the test program with appropriate arguments
    case "${1:-auto}" in
        "auto")
            print_info "Running automated tests..."
            "$TEST_PROGRAM" --auto
            ;;
        "interactive")
            print_info "Starting interactive mode..."
            "$TEST_PROGRAM" --interactive
            ;;
        "simulate")
            print_info "Running in simulation mode..."
            "$TEST_PROGRAM" --simulate
            ;;
        "simple")
            if ! check_simple_program; then
                print_warning "Simple test program not found, attempting to build..."
                check_dependencies || exit 1
                build_program || exit 1
            fi
            print_info "Running simple tests..."
            "$SIMPLE_TEST_PROGRAM" --info --verbose
            ;;
        "device")
            if [ -z "$2" ]; then
                print_error "Device port not specified"
                show_usage
                exit 1
            fi
            print_info "Using device: $2"
            "$TEST_PROGRAM" --device "$2"
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
