/*
    Debug version of QHY Mount to isolate crash issues
*/

#include "qhy_mount_base.h"
#include <iostream>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

void crash_handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char *argv[])
{
    // Install crash handler
    signal(SIGSEGV, crash_handler);
    signal(SIGABRT, crash_handler);
    
    std::cout << "Debug QHY Mount starting..." << std::endl;
    
    try {
        std::cout << "Creating QHY Mount instance..." << std::endl;
        QHYMountBase mount;
        
        std::cout << "Setting device name..." << std::endl;
        mount.setDeviceName("QHY Mount Debug");

        std::cout << "Calling ISGetProperties..." << std::endl;
        mount.ISGetProperties(nullptr);
        
        std::cout << "All tests passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception caught!" << std::endl;
        return 1;
    }
    
    return 0;
}
