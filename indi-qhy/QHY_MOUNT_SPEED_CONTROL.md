# QHY Mount Speed Control Modifications

## Overview

This document describes the modifications made to the Skywatcher protocol implementation to support QHY Mount's direct frequency control system.

## Key Changes

### 1. Mount Code Assignment
- Added QHY Mount code: `0xF1` (defined as `QHY_MOUNT_CODE`)
- Added to mount type detection in `InquireBoardVersion()`

### 2. Speed Control Logic

Unlike standard Skywatcher mounts that use complex calculations:
```cpp
period = static_cast<uint32_t>(((SKYWATCHER_STELLAR_DAY * RAStepsWorm) / static_cast<double>(RASteps360)) / absrate);
```

QHY Mount uses direct frequency control:
```cpp
double frequency_khz = absrate; // Use rate directly as frequency in kHz
period = static_cast<uint32_t>(frequency_khz * 1000); // Convert to Hz for internal use
```

### 3. Motor Specifications

QHY Mount motor configuration:
- **Subdivision**: 256 microsteps
- **Reduction gear**: 100x
- **Motor pulses per revolution**: 200
- **Actual rotation speed formula**: `steps_frequency / 256 / 100`

### 4. Frequency Limits

- **Maximum frequency**: 40 kHz (firmware limit)
- **Minimum frequency**: 0.01 kHz (10 Hz)
- Frequencies outside this range are automatically clamped

## Modified Functions

### Core Speed Functions
1. `SlewRA(double rate)` - RA axis slewing
2. `SlewDE(double rate)` - DE axis slewing  
3. `SetRARate(double rate)` - RA axis rate setting
4. `SetDERate(double rate)` - DE axis rate setting
5. `SetSpeed(SkywatcherAxis axis, uint32_t period)` - Low-level speed setting

### Speed Mapping Examples

| Input Rate (kHz) | Motor Frequency | Actual Speed (rev/sec) |
|------------------|-----------------|------------------------|
| 0.01             | 10 Hz           | 0.000390625           |
| 0.1              | 100 Hz          | 0.00390625            |
| 1.0              | 1 kHz           | 0.0390625             |
| 10.0             | 10 kHz          | 0.390625              |
| 40.0             | 40 kHz          | 1.5625                |

## Implementation Details

### Constants Added
```cpp
#define QHY_MOUNT_CODE 0xF1
#define QHY_MAX_FREQUENCY_KHZ 40.0
#define QHY_MIN_FREQUENCY_KHZ 0.01
```

### Detection Logic
```cpp
if (MountCode == QHY_MOUNT_CODE) // QHY Mount
{
    // Use direct frequency control
    double frequency_khz = absrate;
    if (frequency_khz > QHY_MAX_FREQUENCY_KHZ)
        frequency_khz = QHY_MAX_FREQUENCY_KHZ;
    if (frequency_khz < QHY_MIN_FREQUENCY_KHZ)
        frequency_khz = QHY_MIN_FREQUENCY_KHZ;
    
    period = static_cast<uint32_t>(frequency_khz * 1000);
    useHighspeed = (frequency_khz > 1.0);
}
else
{
    // Standard Skywatcher calculation
    // ... existing code ...
}
```

## Files Modified

1. **indi-3rdparty/indi-qhy/eqmod/skywatcher.h**
   - Added QHY Mount constants
   - Added mount code definition

2. **indi-3rdparty/indi-qhy/eqmod/skywatcher.cpp**
   - Modified speed calculation functions
   - Added QHY Mount detection
   - Updated `SetSpeed()` function for direct frequency control

## Testing

A test file `test_qhy_mount_speed.cpp` is provided to verify the speed calculations and demonstrate the frequency mapping.

To compile and run the test:
```bash
g++ -o test_qhy_mount_speed test_qhy_mount_speed.cpp
./test_qhy_mount_speed
```

## Usage Notes

1. **Rate Input**: For QHY Mount, the rate parameter directly represents the desired motor frequency in kHz
2. **Frequency Limits**: The firmware automatically clamps frequencies to the 10Hz-40kHz range
3. **High/Low Speed**: Frequencies > 1kHz are considered "high speed" for compatibility
4. **Precision**: The system supports frequencies down to 0.01kHz (10Hz) for very slow movements

## Backward Compatibility

These changes are fully backward compatible:
- Standard Skywatcher mounts continue to use the original calculation method
- Only mounts with code `0xF1` use the new direct frequency control
- All existing functionality remains unchanged for other mount types
