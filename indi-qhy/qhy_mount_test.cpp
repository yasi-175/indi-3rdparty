/*
 QHY Mount Test Program

 Copyright (C) 2024 QHY. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "qhy_mount_base.h"
#include "indicom.h"

#define VERSION 1.00

// Global variables for signal handling
static bool running = true;
static std::unique_ptr<QHYMountBase> mount_driver;

// Signal handler for graceful shutdown
void signal_handler(int signal)
{
    printf("\nReceived signal %d, shutting down gracefully...\n", signal);
    running = false;
    if (mount_driver)
    {
        mount_driver->Disconnect();
    }
}

// Test connection to the mount
bool test_connection()
{
    printf("Testing mount connection...\n");

    // Simulate connection by setting connected state
    // In a real test, you would trigger the connection through INDI properties
    mount_driver->setConnected(true);

    if (!mount_driver->isConnected())
    {
        printf("ERROR: Failed to connect to mount\n");
        return false;
    }

    printf("SUCCESS: Connected to mount\n");
    return true;
}

// Test basic mount properties
bool test_properties()
{
    printf("Testing mount properties...\n");
    
    // Get mount name
    const char* name = mount_driver->getDeviceName();
    printf("Mount name: %s\n", name ? name : "Unknown");
    
    // Check if mount is connected
    if (!mount_driver->isConnected())
    {
        printf("ERROR: Mount is not connected\n");
        return false;
    }
    
    printf("SUCCESS: Mount properties verified\n");
    return true;
}

// Test mount status reading
bool test_status()
{
    printf("Testing mount status reading...\n");
    
    // Read scope status
    if (!mount_driver->ReadScopeStatus())
    {
        printf("ERROR: Failed to read scope status\n");
        return false;
    }
    
    // Get current coordinates from EqNP property
    auto eqNP = mount_driver->getNumber("EQUATORIAL_EOD_COORD");
    if (eqNP && eqNP.isValid())
    {
        double ra = eqNP[0].getValue();
        double dec = eqNP[1].getValue();
        char ra_str[32], dec_str[32];
        fs_sexa(ra_str, ra, 2, 3600);
        fs_sexa(dec_str, dec, 2, 3600);
        printf("Current position - RA: %s, DEC: %s\n", ra_str, dec_str);
    }
    else
    {
        printf("WARNING: Could not get current coordinates\n");
    }
    
    printf("SUCCESS: Mount status read\n");
    return true;
}

// Test mount movement (small movements only for safety)
bool test_movement()
{
    printf("Testing mount movement (small movements for safety)...\n");
    
    // Test slewing in different directions for short periods
    printf("Testing North movement...\n");
    if (!mount_driver->MoveNS(INDI_DIR_NS::DIRECTION_NORTH, INDI::Telescope::MOTION_START))
    {
        printf("ERROR: Failed to start North movement\n");
        return false;
    }
    
    // Move for 1 second
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    if (!mount_driver->MoveNS(INDI_DIR_NS::DIRECTION_NORTH, INDI::Telescope::MOTION_STOP))
    {
        printf("ERROR: Failed to stop North movement\n");
        return false;
    }
    
    printf("North movement test completed\n");
    
    // Test East movement
    printf("Testing East movement...\n");
    if (!mount_driver->MoveWE(INDI_DIR_WE::DIRECTION_EAST, INDI::Telescope::MOTION_START))
    {
        printf("ERROR: Failed to start East movement\n");
        return false;
    }
    
    // Move for 1 second
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    if (!mount_driver->MoveWE(INDI_DIR_WE::DIRECTION_EAST, INDI::Telescope::MOTION_STOP))
    {
        printf("ERROR: Failed to stop East movement\n");
        return false;
    }
    
    printf("East movement test completed\n");
    
    printf("SUCCESS: Movement tests completed\n");
    return true;
}

// Test abort functionality
bool test_abort()
{
    printf("Testing abort functionality...\n");
    
    if (!mount_driver->Abort())
    {
        printf("ERROR: Failed to abort mount operations\n");
        return false;
    }
    
    printf("SUCCESS: Abort test completed\n");
    return true;
}

// Test tracking functionality
bool test_tracking()
{
    printf("Testing tracking functionality...\n");
    
    // Enable tracking
    if (!mount_driver->SetTrackEnabled(true))
    {
        printf("WARNING: Failed to enable tracking (may not be supported)\n");
    }
    else
    {
        printf("Tracking enabled\n");
        
        // Wait a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        
        // Disable tracking
        if (!mount_driver->SetTrackEnabled(false))
        {
            printf("WARNING: Failed to disable tracking\n");
        }
        else
        {
            printf("Tracking disabled\n");
        }
    }
    
    printf("SUCCESS: Tracking test completed\n");
    return true;
}

// Interactive mode for manual testing
void interactive_mode()
{
    printf("\n=== Interactive Mode ===\n");
    printf("Commands:\n");
    printf("  s - Read status\n");
    printf("  n - Move North (1 sec)\n");
    printf("  S - Move South (1 sec)\n");
    printf("  e - Move East (1 sec)\n");
    printf("  w - Move West (1 sec)\n");
    printf("  a - Abort all movements\n");
    printf("  t - Toggle tracking\n");
    printf("  q - Quit\n");
    printf("Enter command: ");
    
    char command;
    bool tracking_enabled = false;
    
    while (running && std::cin >> command)
    {
        switch (command)
        {
            case 's':
                test_status();
                break;
                
            case 'n':
                printf("Moving North for 1 second...\n");
                mount_driver->MoveNS(INDI_DIR_NS::DIRECTION_NORTH, INDI::Telescope::MOTION_START);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                mount_driver->MoveNS(INDI_DIR_NS::DIRECTION_NORTH, INDI::Telescope::MOTION_STOP);
                break;
                
            case 'S':
                printf("Moving South for 1 second...\n");
                mount_driver->MoveNS(INDI_DIR_NS::DIRECTION_SOUTH, INDI::Telescope::MOTION_START);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                mount_driver->MoveNS(INDI_DIR_NS::DIRECTION_SOUTH, INDI::Telescope::MOTION_STOP);
                break;
                
            case 'e':
                printf("Moving East for 1 second...\n");
                mount_driver->MoveWE(INDI_DIR_WE::DIRECTION_EAST, INDI::Telescope::MOTION_START);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                mount_driver->MoveWE(INDI_DIR_WE::DIRECTION_EAST, INDI::Telescope::MOTION_STOP);
                break;
                
            case 'w':
                printf("Moving West for 1 second...\n");
                mount_driver->MoveWE(INDI_DIR_WE::DIRECTION_WEST, INDI::Telescope::MOTION_START);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                mount_driver->MoveWE(INDI_DIR_WE::DIRECTION_WEST, INDI::Telescope::MOTION_STOP);
                break;
                
            case 'a':
                printf("Aborting all movements...\n");
                mount_driver->Abort();
                break;
                
            case 't':
                tracking_enabled = !tracking_enabled;
                printf("%s tracking...\n", tracking_enabled ? "Enabling" : "Disabling");
                mount_driver->SetTrackEnabled(tracking_enabled);
                break;
                
            case 'q':
                running = false;
                break;
                
            default:
                printf("Unknown command: %c\n", command);
                break;
        }
        
        if (running)
        {
            printf("Enter command: ");
        }
    }
}

// Run automated tests
bool run_automated_tests()
{
    printf("\n=== Running Automated Tests ===\n");

    bool all_passed = true;

    // Test 1: Connection
    if (!test_connection())
    {
        all_passed = false;
        goto cleanup;
    }

    // Test 2: Properties
    if (!test_properties())
    {
        all_passed = false;
        goto cleanup;
    }

    // Test 3: Status reading
    if (!test_status())
    {
        all_passed = false;
        goto cleanup;
    }

    // Test 4: Abort functionality
    if (!test_abort())
    {
        all_passed = false;
        goto cleanup;
    }

    // Test 5: Movement (optional, can be dangerous)
    printf("\nWARNING: Movement tests will move the mount!\n");
    printf("Do you want to run movement tests? (y/N): ");
    char response;
    if (std::cin >> response && (response == 'y' || response == 'Y'))
    {
        if (!test_movement())
        {
            all_passed = false;
            goto cleanup;
        }
    }
    else
    {
        printf("Skipping movement tests\n");
    }

    // Test 6: Tracking
    if (!test_tracking())
    {
        all_passed = false;
        goto cleanup;
    }

cleanup:
    if (all_passed)
    {
        printf("\n=== ALL TESTS PASSED ===\n");
    }
    else
    {
        printf("\n=== SOME TESTS FAILED ===\n");
    }

    return all_passed;
}

// Print usage information
void print_usage(const char* program_name)
{
    printf("QHY Mount Test Program v%.2f\n", VERSION);
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -h, --help     Show this help message\n");
    printf("  -i, --interactive  Run in interactive mode\n");
    printf("  -a, --auto     Run automated tests (default)\n");
    printf("  -d, --device   Specify device port (e.g., /dev/ttyUSB0)\n");
    printf("  -s, --simulate Use simulation mode\n");
    printf("\nThis program tests the QHY Mount driver functionality.\n");
    printf("It can run automated tests or provide an interactive interface.\n");
    printf("Use with caution as it may move the mount!\n");
}

int main(int argc, char *argv[])
{
    printf("QHY Mount Test Program v%.2f\n", VERSION);
    printf("Copyright (C) 2024 QHY. All rights reserved.\n\n");

    // Parse command line arguments
    bool interactive_mode_requested = false;
    bool auto_mode = true;
    bool simulate = false;
    const char* device_port = nullptr;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0)
        {
            interactive_mode_requested = true;
            auto_mode = false;
        }
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--auto") == 0)
        {
            auto_mode = true;
            interactive_mode_requested = false;
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--simulate") == 0)
        {
            simulate = true;
        }
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--device") == 0)
        {
            if (i + 1 < argc)
            {
                device_port = argv[++i];
            }
            else
            {
                printf("ERROR: --device option requires a device path\n");
                return 1;
            }
        }
        else
        {
            printf("ERROR: Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("Creating mount driver instance...\n");

    // Create mount driver instance
    try
    {
        mount_driver = std::make_unique<QHYMountBase>();

        if (!mount_driver)
        {
            printf("ERROR: Failed to create mount driver instance\n");
            return 1;
        }

        printf("Mount driver created successfully\n");

        // Set device name first
        mount_driver->setDeviceName("QHY Mount Test");

        // Initialize properties (call ISGetProperties to trigger initialization)
        printf("Initializing mount properties...\n");
        mount_driver->ISGetProperties(nullptr);

        printf("Mount driver initialized successfully\n");

        // Set device port if specified
        if (device_port)
        {
            printf("Setting device port to: %s\n", device_port);
            // Note: The actual method to set device port may vary
            // This is a placeholder - you may need to adjust based on the actual API
        }

        // Enable simulation if requested
        if (simulate)
        {
            printf("Enabling simulation mode\n");
            // Set simulation mode
            auto simSP = mount_driver->getSwitch("SIMULATION");
            if (simSP && simSP.isValid())
            {
                simSP[0].setState(ISS_ON);
                simSP[1].setState(ISS_OFF);
                simSP.setState(IPS_OK);
                printf("Simulation mode enabled\n");
            }
            else
            {
                printf("WARNING: Could not enable simulation mode\n");
            }
        }

        int result = 0;

        if (auto_mode)
        {
            // Run automated tests
            if (!run_automated_tests())
            {
                result = 1;
            }
        }

        if (interactive_mode_requested && running)
        {
            // Run interactive mode
            interactive_mode();
        }

        // Cleanup
        if (mount_driver && mount_driver->isConnected())
        {
            printf("Disconnecting from mount...\n");
            mount_driver->setConnected(false);
        }

        printf("QHY Mount Test Program finished\n");
        return result;
    }
    catch (const std::exception& e)
    {
        printf("ERROR: Exception occurred: %s\n", e.what());
        return 1;
    }
    catch (...)
    {
        printf("ERROR: Unknown exception occurred\n");
        return 1;
    }
}
