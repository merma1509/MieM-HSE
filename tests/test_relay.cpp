/**
 * @file test_relay.cpp
 * @brief Test program for Relay Module
 * @description Tests relay switching functionality
 * 
 * How to use:
 * 1. Comment out main.cpp in your build
 * 2. Compile this test file instead
 * 3. Connect serial monitor at 9600 baud
 * 4. Listen for relay clicking sound
 * 5. Verify lock opens/closes
 */

#ifdef BUILD_TEST_RELAY
#include "mbed.h"

#include <cstdarg>
#include <cstdio>

using namespace std::chrono_literals;

// Serial output for debugging
BufferedSerial pc(USBTX, USBRX, 9600);
void pc_printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    if (len <= 0) {
        return;
    }
    if (len > static_cast<int>(sizeof(buffer))) {
        len = sizeof(buffer);
    }
    pc.write(buffer, len);
}


// Relay control pin
DigitalOut relay(PA_8);

// LED for visual feedback
DigitalOut led(PC_13);

// Test statistics
int totalSwitches = 0;
int onCount = 0;
int offCount = 0;

/**
 * @brief Prints test header
 */
void printHeader() {
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  RELAY MODULE TEST PROGRAM\n");
    pc_printf("========================================\n");
    pc_printf("Hardware: STM32 Nucleo\n");
    pc_printf("Relay Pin: PA_8\n");
    pc_printf("Baud Rate: 9600\n");
    pc_printf("========================================\n");
    pc_printf("\n");
    pc_printf("Test Sequence:\n");
    pc_printf("  1. Basic ON/OFF Test\n");
    pc_printf("  2. Rapid Switching Test\n");
    pc_printf("  3. Timed Lock Simulation\n");
    pc_printf("  4. Continuous Operation Test\n");
    pc_printf("\n");
    pc_printf("Listen for relay clicking sound!\n");
    pc_printf("========================================\n");
    pc_printf("\n");
}

/**
 * @brief Test 1: Basic ON/OFF Test
 */
void test_basic_onoff() {
    pc_printf("\n[TEST 1] Basic ON/OFF Test\n");
    pc_printf("  - Testing relay switching...\n");
    
    for (int i = 0; i < 5; i++) {
        pc_printf("  - Cycle %d/5: ", i + 1);
        
        // Turn relay ON
        relay = 1;
        led = 1;
        onCount++;
        totalSwitches++;
        pc_printf("ON (2s) ");
        ThisThread::sleep_for(2s);
        
        // Turn relay OFF
        relay = 0;
        led = 0;
        offCount++;
        totalSwitches++;
        pc_printf("-> OFF (2s)\n");
        ThisThread::sleep_for(2s);
    }
    
    pc_printf("  - Test complete. Relay should have clicked 10 times.\n");
}

/**
 * @brief Test 2: Rapid Switching Test
 */
void test_rapid_switching() {
    pc_printf("\n[TEST 2] Rapid Switching Test\n");
    pc_printf("  - Testing rapid ON/OFF cycles...\n");
    
    for (int i = 0; i < 10; i++) {
        relay = 1;
        led = 1;
        onCount++;
        totalSwitches++;
        ThisThread::sleep_for(500ms);
        
        relay = 0;
        led = 0;
        offCount++;
        totalSwitches++;
        ThisThread::sleep_for(500ms);
        
        pc_printf("  - Rapid cycle %d/10 complete\n", i + 1);
    }
    
    pc_printf("  - Test complete. 20 rapid switches executed.\n");
}

/**
 * @brief Test 3: Timed Lock Simulation
 */
void test_timed_lock_simulation() {
    pc_printf("\n[TEST 3] Timed Lock Simulation (10 seconds)\n");
    pc_printf("  - Simulating door lock opening for 10 seconds...\n");
    
    // Open lock
    pc_printf("  - [0s] Lock OPENING...\n");
    relay = 1;
    led = 1;
    onCount++;
    totalSwitches++;
    
    // Countdown
    for (int i = 10; i > 0; i--) {
        pc_printf("  - [%ds] Lock OPEN - Closing in %d seconds...\n", 10 - i, i);
        ThisThread::sleep_for(1s);
    }
    
    // Close lock
    pc_printf("  - [10s] Lock CLOSING...\n");
    relay = 0;
    led = 0;
    offCount++;
    totalSwitches++;
    
    pc_printf("  - Lock CLOSED. Test complete.\n");
    ThisThread::sleep_for(2s);
}

/**
 * @brief Test 4: Pulse Width Test
 */
void test_pulse_width() {
    pc_printf("\n[TEST 4] Pulse Width Test\n");
    pc_printf("  - Testing different pulse durations...\n");
    
    int durations[] = {100, 250, 500, 1000, 2000, 5000};
    int numDurations = sizeof(durations) / sizeof(durations[0]);
    
    for (int i = 0; i < numDurations; i++) {
        pc_printf("  - Pulse %d/%d: %dms ON...", i + 1, numDurations, durations[i]);
        
        relay = 1;
        led = 1;
        onCount++;
        totalSwitches++;
        ThisThread::sleep_for(std::chrono::milliseconds(durations[i]));
        
        relay = 0;
        led = 0;
        offCount++;
        totalSwitches++;
        pc_printf(" OFF\n");
        
        ThisThread::sleep_for(1s);  // Wait between pulses
    }
    
    pc_printf("  - Test complete. All pulse widths tested.\n");
}

/**
 * @brief Test 5: Stress Test
 */
void test_stress() {
    pc_printf("\n[TEST 5] Stress Test (100 cycles)\n");
    pc_printf("  - Testing relay endurance...\n");
    
    for (int i = 0; i < 100; i++) {
        relay = 1;
        led = 1;
        onCount++;
        totalSwitches++;
        ThisThread::sleep_for(100ms);
        
        relay = 0;
        led = 0;
        offCount++;
        totalSwitches++;
        ThisThread::sleep_for(100ms);
        
        if ((i + 1) % 10 == 0) {
            pc_printf("  - Progress: %d/100 cycles\n", i + 1);
        }
    }
    
    pc_printf("  - Stress test complete. 200 switches executed.\n");
}

/**
 * @brief Prints test statistics
 */
void printStatistics() {
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  RELAY TEST STATISTICS\n");
    pc_printf("========================================\n");
    pc_printf("Total switches: %d\n", totalSwitches);
    pc_printf("ON commands: %d\n", onCount);
    pc_printf("OFF commands: %d\n", offCount);
    pc_printf("========================================\n");
    pc_printf("\n");
}

/**
 * @brief Manual control mode
 */
void manual_control_mode() {
    pc_printf("\n[MANUAL MODE] Interactive Relay Control\n");
    pc_printf("  - Press '1' to turn relay ON\n");
    pc_printf("  - Press '0' to turn relay OFF\n");
    pc_printf("  - Press 'q' to quit manual mode\n");
    pc_printf("\n");
    
    while (true) {
        if (pc.readable()) {
            char cmd = 0; pc.read(&cmd, 1);
            
            if (cmd == '1') {
                relay = 1;
                led = 1;
                onCount++;
                totalSwitches++;
                pc_printf("  - Relay ON\n");
            } else if (cmd == '0') {
                relay = 0;
                led = 0;
                offCount++;
                totalSwitches++;
                pc_printf("  - Relay OFF\n");
            } else if (cmd == 'q' || cmd == 'Q') {
                relay = 0;
                led = 0;
                pc_printf("  - Exiting manual mode...\n");
                break;
            } else {
                pc_printf("  - Invalid command: '%c'\n", cmd);
            }
        }
        
        ThisThread::sleep_for(100ms);
    }
}

/**
 * @brief Main test function
 */
int main() {
    // Initialize relay and LED (both OFF)
    relay = 0;
    led = 0;
    
    // Wait for serial connection
    ThisThread::sleep_for(2s);
    
    // Print header
    printHeader();
    
    int testCycle = 1;
    
    while (true) {
        pc_printf("\n========================================\n");
        pc_printf("Starting Test Cycle %d\n", testCycle);
        pc_printf("========================================\n");
        
        // Run all tests
        test_basic_onoff();
        ThisThread::sleep_for(2s);
        
        test_rapid_switching();
        ThisThread::sleep_for(2s);
        
        test_timed_lock_simulation();
        ThisThread::sleep_for(2s);
        
        test_pulse_width();
        ThisThread::sleep_for(2s);
        
        test_stress();
        ThisThread::sleep_for(2s);
        
        // Print statistics
        printStatistics();
        
        pc_printf("\n========================================\n");
        pc_printf("Test Cycle %d Complete!\n", testCycle);
        pc_printf("========================================\n");
        pc_printf("\n");
        pc_printf("Options:\n");
        pc_printf("  - Press 'm' for manual control mode\n");
        pc_printf("  - Press any other key to repeat tests\n");
        pc_printf("\n");
        
        // Wait for user input (with timeout)
        Timer inputTimer;
        inputTimer.start();
        bool gotInput = false;
        
        while (inputTimer.elapsed_time() < 10s) {  // 10 second timeout
            if (pc.readable()) {
                char cmd = 0; pc.read(&cmd, 1);
                gotInput = true;
                
                if (cmd == 'm' || cmd == 'M') {
                    manual_control_mode();
                }
                break;
            }
            ThisThread::sleep_for(100ms);
        }
        
        if (!gotInput) {
            pc_printf("No input received. Repeating tests in 5 seconds...\n");
            ThisThread::sleep_for(5s);
        }
        
        testCycle++;
    }
}

#endif // BUILD_TEST_RELAY
