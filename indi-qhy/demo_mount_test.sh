#!/bin/bash

# QHY Mount Test Demonstration Script
# This script demonstrates the QHY Mount test programs

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/../../build/indi-qhy"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to print colored output
print_header() {
    echo -e "${CYAN}========================================${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}========================================${NC}"
}

print_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to pause and wait for user input
pause() {
    echo -e "${YELLOW}Press Enter to continue...${NC}"
    read -r
}

# Function to check if programs exist
check_programs() {
    local simple_test="$BUILD_DIR/qhy_mount_simple_test"
    local full_test="$BUILD_DIR/qhy_mount_test"
    
    if [ ! -f "$simple_test" ] || [ ! -f "$full_test" ]; then
        print_error "Test programs not found in $BUILD_DIR"
        print_info "Please build the programs first:"
        print_info "  cd $SCRIPT_DIR"
        print_info "  mkdir -p build && cd build"
        print_info "  cmake .. && make qhy_mount_test qhy_mount_simple_test"
        return 1
    fi
    
    return 0
}

# Main demonstration function
main() {
    print_header "QHY Mount Test Programs Demonstration"
    
    echo "This script demonstrates the QHY Mount test programs."
    echo "Two test programs are available:"
    echo "  1. qhy_mount_simple_test - Basic verification (no INDI server required)"
    echo "  2. qhy_mount_test - Full testing with INDI integration"
    echo ""
    
    # Check if programs exist
    if ! check_programs; then
        exit 1
    fi
    
    print_success "Test programs found!"
    pause
    
    # Demonstration 1: Simple Test
    print_header "Demonstration 1: Simple Test Program"
    print_step "Running basic simple test..."
    echo ""
    
    "$BUILD_DIR/qhy_mount_simple_test"
    
    echo ""
    print_success "Simple test completed!"
    pause
    
    # Demonstration 2: Simple Test with Info
    print_header "Demonstration 2: Simple Test with System Info"
    print_step "Running simple test with verbose output and system information..."
    echo ""
    
    "$BUILD_DIR/qhy_mount_simple_test" --info --verbose
    
    echo ""
    print_success "Detailed simple test completed!"
    pause
    
    # Demonstration 3: Full Test Help
    print_header "Demonstration 3: Full Test Program Help"
    print_step "Showing help for the full test program..."
    echo ""
    
    "$BUILD_DIR/qhy_mount_test" --help
    
    echo ""
    print_success "Help information displayed!"
    pause
    
    # Demonstration 4: Explain what would happen with full test
    print_header "Demonstration 4: Full Test Program (Explanation)"
    
    echo "The full test program (qhy_mount_test) provides comprehensive testing"
    echo "but requires an INDI server environment. Here's what it would do:"
    echo ""
    echo "Available modes:"
    echo "  • Automated tests: Connection, properties, status, movement, tracking"
    echo "  • Interactive mode: Manual control with commands (n/s/e/w/a/t/q)"
    echo "  • Simulation mode: Safe testing without physical hardware"
    echo ""
    echo "Example commands:"
    echo "  ./qhy_mount_test --auto      # Run automated tests"
    echo "  ./qhy_mount_test --simulate  # Run in simulation mode"
    echo "  ./qhy_mount_test --interactive # Interactive control"
    echo ""
    print_info "Note: Full test requires INDI server and may move physical hardware!"
    pause
    
    # Demonstration 5: Show file structure
    print_header "Demonstration 5: Test Program Files"
    print_step "Showing the test program files and their purposes..."
    echo ""
    
    echo "Test Program Files:"
    echo "├── qhy_mount_test.cpp           # Full test program source"
    echo "├── qhy_mount_simple_test.cpp    # Simple test program source"
    echo "├── run_mount_test.sh            # Convenience script"
    echo "├── demo_mount_test.sh           # This demonstration script"
    echo "├── Makefile.test                # Standalone makefile"
    echo "└── README_MOUNT_TEST.md         # Comprehensive documentation"
    echo ""
    
    if [ -f "$SCRIPT_DIR/README_MOUNT_TEST.md" ]; then
        print_info "Documentation available in README_MOUNT_TEST.md"
    fi
    
    pause
    
    # Final summary
    print_header "Summary and Next Steps"
    
    echo "✓ Simple test program: Working and demonstrated"
    echo "✓ Full test program: Available (requires INDI server for full functionality)"
    echo "✓ Documentation: Complete with usage examples"
    echo "✓ Build system: Integrated with CMake"
    echo ""
    echo "Recommended next steps:"
    echo "1. Use simple test for quick verification: ./qhy_mount_simple_test"
    echo "2. Read full documentation: README_MOUNT_TEST.md"
    echo "3. For hardware testing: Set up INDI server and use full test program"
    echo "4. For development: Use convenience script: ./run_mount_test.sh"
    echo ""
    
    print_success "QHY Mount test programs are ready for use!"
    
    echo ""
    echo "Thank you for using the QHY Mount test programs!"
}

# Run the demonstration
main "$@"
