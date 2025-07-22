/*
    Test file for QHY Mount speed control modifications
    
    This file demonstrates the new QHY Mount speed control logic:
    - Direct frequency control instead of complex calculations
    - period = frequency in kHz (10 = 10kHz, 1 = 1kHz, 0.1 = 100Hz, 0.01 = 10Hz)
    - Maximum frequency: 40kHz, Minimum frequency: 10Hz (0.01kHz)
    - Motor specs: 256 subdivision, 100x reduction gear, 200 pulses per revolution
    - Actual rotation speed = steps_frequency / 256 / 100
*/

#include <iostream>
#include <iomanip>

// QHY Mount constants (from skywatcher.h)
#define QHY_MOUNT_CODE 0xF1
#define QHY_MAX_FREQUENCY_KHZ 40.0
#define QHY_MIN_FREQUENCY_KHZ 0.01

// Motor specifications for QHY Mount
#define QHY_MOTOR_SUBDIVISION 256
#define QHY_REDUCTION_RATIO 100
#define QHY_PULSES_PER_REV 200

void test_qhy_speed_calculation()
{
    std::cout << "QHY Mount Speed Control Test\n";
    std::cout << "============================\n\n";
    
    std::cout << "Motor Specifications:\n";
    std::cout << "- Subdivision: " << QHY_MOTOR_SUBDIVISION << "\n";
    std::cout << "- Reduction ratio: " << QHY_REDUCTION_RATIO << "x\n";
    std::cout << "- Pulses per revolution: " << QHY_PULSES_PER_REV << "\n";
    std::cout << "- Frequency range: " << QHY_MIN_FREQUENCY_KHZ << " kHz to " << QHY_MAX_FREQUENCY_KHZ << " kHz\n\n";
    
    std::cout << "Speed Calculation Examples:\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Frequency (kHz) | Steps/sec | Actual rotation speed (rev/sec)\n";
    std::cout << "----------------|-----------|--------------------------------\n";
    
    double test_frequencies[] = {0.01, 0.1, 1.0, 10.0, 40.0};
    int num_tests = sizeof(test_frequencies) / sizeof(test_frequencies[0]);
    
    for (int i = 0; i < num_tests; i++)
    {
        double freq_khz = test_frequencies[i];
        double steps_per_sec = freq_khz * 1000; // Convert kHz to Hz
        double rotation_speed = steps_per_sec / QHY_MOTOR_SUBDIVISION / QHY_REDUCTION_RATIO;
        
        std::cout << std::setw(15) << freq_khz << " | ";
        std::cout << std::setw(9) << steps_per_sec << " | ";
        std::cout << std::setw(30) << rotation_speed << "\n";
    }
    
    std::cout << "\nNote: Actual rotation speed = steps_frequency / " << QHY_MOTOR_SUBDIVISION 
              << " / " << QHY_REDUCTION_RATIO << "\n";
}

void test_period_conversion()
{
    std::cout << "\nPeriod Conversion Test\n";
    std::cout << "======================\n\n";

    std::cout << "Input Rate (kHz) | Internal Period (Hz) | Firmware Period\n";
    std::cout << "-----------------|----------------------|----------------\n";

    double test_rates[] = {0.01, 0.1, 1.0, 10.0, 40.0, 50.0}; // Last one tests clamping
    int num_tests = sizeof(test_rates) / sizeof(test_rates[0]);

    for (int i = 0; i < num_tests; i++)
    {
        double rate = test_rates[i];

        // Apply QHY Mount logic (similar to what's in the modified code)
        double frequency_khz = rate;
        if (frequency_khz > QHY_MAX_FREQUENCY_KHZ)
            frequency_khz = QHY_MAX_FREQUENCY_KHZ;
        if (frequency_khz < QHY_MIN_FREQUENCY_KHZ)
            frequency_khz = QHY_MIN_FREQUENCY_KHZ;

        uint32_t internal_period = static_cast<uint32_t>(frequency_khz * 1000); // Hz
        uint32_t firmware_period = static_cast<uint32_t>(frequency_khz * 100);  // Scaled for firmware

        std::cout << std::setw(16) << rate << " | ";
        std::cout << std::setw(20) << internal_period << " | ";
        std::cout << std::setw(15) << firmware_period;

        if (rate != frequency_khz)
            std::cout << " (clamped)";

        std::cout << "\n";
    }
}

void test_eqmod_slew_speeds()
{
    std::cout << "\nEQMod Slew Speeds to QHY Period Mapping\n";
    std::cout << "========================================\n\n";

    // EQMod slew speeds (from eqmodbase.cpp)
    int slewspeeds[] = { 1, 2, 4, 8, 32, 64, 128, 600, 700, 800 };
    int num_speeds = sizeof(slewspeeds) / sizeof(slewspeeds[0]);

    std::cout << "Slew Speed | Input Rate | QHY Frequency | Internal Period | RAPeriod Value\n";
    std::cout << "-----------|------------|---------------|-----------------|---------------\n";

    for (int i = 0; i < num_speeds; i++)
    {
        double rate = slewspeeds[i]; // This is the rate passed to SlewRA()

        // Apply QHY Mount logic
        double frequency_khz = rate; // For QHY: rate directly becomes frequency
        if (frequency_khz > QHY_MAX_FREQUENCY_KHZ)
            frequency_khz = QHY_MAX_FREQUENCY_KHZ;
        if (frequency_khz < QHY_MIN_FREQUENCY_KHZ)
            frequency_khz = QHY_MIN_FREQUENCY_KHZ;

        uint32_t period = static_cast<uint32_t>(frequency_khz * 1000); // This becomes RAPeriod

        std::cout << std::setw(10) << slewspeeds[i] << "x | ";
        std::cout << std::setw(10) << rate << " | ";
        std::cout << std::setw(13) << frequency_khz << " kHz | ";
        std::cout << std::setw(15) << period << " Hz | ";
        std::cout << std::setw(14) << period;

        if (rate != frequency_khz)
            std::cout << " (clamped)";

        std::cout << "\n";
    }

    std::cout << "\nNote: RAPeriod = period value stored and used in SetSpeed()\n";
}

int main()
{
    test_qhy_speed_calculation();
    test_period_conversion();
    test_eqmod_slew_speeds();

    std::cout << "\nQHY Mount Speed Control Summary:\n";
    std::cout << "- Uses direct frequency control (no complex calculations)\n";
    std::cout << "- Input rate is treated as frequency in kHz\n";
    std::cout << "- Firmware limits: 10Hz to 40kHz\n";
    std::cout << "- Motor: 256 subdivision, 100x reduction, 200 pulses/rev\n";
    std::cout << "- RAPeriod stores the final period value used by SetSpeed()\n";

    return 0;
}
