# QHY Mount Test Programs

This directory contains comprehensive test programs for the QHY Mount driver (indi_qhy_mount). Two test programs are provided:

1. **qhy_mount_test** - Full-featured test with INDI integration
2. **qhy_mount_simple_test** - Simplified test for basic verification

## Test Programs Overview

### qhy_mount_test (Full Test Program)

**Features:**
- **Automated Testing**: Runs a series of tests to verify mount functionality
- **Interactive Mode**: Provides manual control for testing specific functions
- **Safety Features**: Includes abort functionality and limited movement tests
- **Simulation Support**: Can run in simulation mode for safe testing
- **Comprehensive Logging**: Detailed output for debugging and verification

**Requirements:**
- Full INDI development environment
- INDI server running (for full functionality)
- Physical mount connection (for hardware tests)

### qhy_mount_simple_test (Basic Test Program)

**Features:**
- **Basic Verification**: Tests compilation, linking, and basic functionality
- **No INDI Server Required**: Runs independently of INDI server
- **System Information**: Displays build and system details
- **Quick Validation**: Fast verification that the build environment is correct

**Requirements:**
- Basic C++ compiler and libraries
- No INDI server required
- No hardware required

## Building

### Using CMake (Recommended)

Both test programs are integrated into the main CMake build system:

```bash
cd /path/to/indi-3rdparty/indi-qhy
mkdir build
cd build
cmake ..
make qhy_mount_test          # Build full test program
make qhy_mount_simple_test   # Build simple test program
```

### Using the Standalone Makefile

For quick testing and development:

```bash
cd /path/to/indi-3rdparty/indi-qhy
make -f Makefile.test
```

## Usage

### Quick Start

For a quick verification that everything is working:

```bash
# Run simple test (no INDI server required)
./qhy_mount_simple_test

# Or use the convenience script
./run_mount_test.sh simple
```

### Full Test Program (qhy_mount_test)

**Command Line Options:**

```bash
./qhy_mount_test [options]

Options:
  -h, --help         Show help message
  -i, --interactive  Run in interactive mode
  -a, --auto         Run automated tests (default)
  -d, --device       Specify device port (e.g., /dev/ttyUSB0)
  -s, --simulate     Use simulation mode
```

### Simple Test Program (qhy_mount_simple_test)

**Command Line Options:**

```bash
./qhy_mount_simple_test [options]

Options:
  -h, --help     Show help message
  -v, --verbose  Verbose output
  -i, --info     Show system information
```

### Examples

#### Simple Test Examples

1. **Basic test**:
   ```bash
   ./qhy_mount_simple_test
   ```

2. **Verbose output with system info**:
   ```bash
   ./qhy_mount_simple_test --info --verbose
   ```

#### Full Test Examples

1. **Run automated tests**:
   ```bash
   ./qhy_mount_test
   ```

2. **Run in interactive mode**:
   ```bash
   ./qhy_mount_test --interactive
   ```

3. **Run with simulation**:
   ```bash
   ./qhy_mount_test --simulate
   ```

4. **Specify device port**:
   ```bash
   ./qhy_mount_test --device /dev/ttyUSB0
   ```

#### Using the Convenience Script

1. **Run simple tests**:
   ```bash
   ./run_mount_test.sh simple
   ```

2. **Run full automated tests**:
   ```bash
   ./run_mount_test.sh auto
   ```

3. **Run in interactive mode**:
   ```bash
   ./run_mount_test.sh interactive
   ```

## Test Categories

### Automated Tests

1. **Connection Test**: Verifies the mount can be connected
2. **Properties Test**: Checks basic mount properties and status
3. **Status Reading Test**: Tests ability to read mount position and status
4. **Abort Test**: Verifies emergency stop functionality
5. **Movement Test**: Tests directional movement (optional, requires confirmation)
6. **Tracking Test**: Tests tracking enable/disable functionality

### Interactive Mode Commands

- `s` - Read and display current status
- `n` - Move North for 1 second
- `S` - Move South for 1 second  
- `e` - Move East for 1 second
- `w` - Move West for 1 second
- `a` - Abort all movements
- `t` - Toggle tracking on/off
- `q` - Quit interactive mode

## Safety Considerations

⚠️ **IMPORTANT SAFETY NOTES**:

1. **Movement Tests**: The program includes movement tests that will physically move your mount. Always ensure:
   - The mount is properly set up and balanced
   - There are no obstructions in the movement path
   - You are ready to manually stop the mount if needed

2. **Emergency Stop**: The program includes abort functionality, but you should also know how to manually stop your mount

3. **Simulation Mode**: When possible, use simulation mode (`--simulate`) for initial testing

4. **Supervision**: Never leave the mount unattended during testing

## Troubleshooting

### Common Issues

1. **Connection Failed**:
   - Check USB/serial connection
   - Verify device permissions (`sudo chmod 666 /dev/ttyUSB0`)
   - Ensure no other software is using the mount

2. **Permission Denied**:
   - Add user to dialout group: `sudo usermod -a -G dialout $USER`
   - Log out and back in for changes to take effect

3. **Build Errors**:
   - Ensure INDI development packages are installed
   - Check that all dependencies are available

### Dependencies

Required packages (Ubuntu/Debian):
```bash
sudo apt-get install libindi-dev libnova-dev libgsl-dev
```

Required packages (CentOS/RHEL):
```bash
sudo yum install indi-devel libnova-devel gsl-devel
```

## Output Interpretation

### Success Messages
- `SUCCESS: Connected to mount` - Mount connection established
- `SUCCESS: Mount properties verified` - Basic functionality confirmed
- `SUCCESS: Mount status read` - Position reading working
- `ALL TESTS PASSED` - All automated tests completed successfully

### Error Messages
- `ERROR: Failed to connect to mount` - Connection problem
- `ERROR: Failed to read scope status` - Communication issue
- `SOME TESTS FAILED` - One or more tests did not pass

### Warning Messages
- `WARNING: Could not get current coordinates` - Non-critical issue
- `WARNING: Failed to enable tracking` - Feature may not be supported

## Contributing

If you find issues or want to add features:

1. Test thoroughly in simulation mode first
2. Document any new test cases
3. Follow the existing code style
4. Add appropriate error handling

## License

This program is released under the same license as the INDI QHY driver (LGPL v2.1).
