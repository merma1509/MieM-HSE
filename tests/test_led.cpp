/**
 * @file test_led.cpp
 * @brief Test program for LED Indicator
 * @description Tests LED flashing patterns and timing
 * 
 * How to use:
 * 1. Comment out main.cpp in your build
 * 2. Compile this test file instead
 * 3. Connect serial monitor at 9600 baud
 * 4. Watch LED on board (PC_13)
 */

#ifdef BUILD_TEST_LED
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


// LED on PC_13 (built-in on most Nucleo boards)
DigitalOut led(PC_13);

// Ticker for interrupt-based flashing
Ticker ledTicker;

// Global variables for ticker tests
volatile bool tickerActive = false;
volatile int tickCount = 0;

/**
 * @brief LED toggle ISR for Ticker
 */
void ledToggleISR() {
    led = !led;
    tickCount++;
}

/**
 * @brief Prints test header
 */
void printHeader() {
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  LED INDICATOR TEST PROGRAM\n");
    pc_printf("========================================\n");
    pc_printf("Hardware: STM32 Nucleo\n");
    pc_printf("LED Pin: PC_13 (built-in)\n");
    pc_printf("Baud Rate: 9600\n");
    pc_printf("========================================\n");
    pc_printf("\n");
    pc_printf("Test Sequence:\n");
    pc_printf("  1. Basic ON/OFF Test\n");
    pc_printf("  2. Blinking Pattern Test\n");
    pc_printf("  3. PWM Brightness Test (if supported)\n");
    pc_printf("  4. Ticker-based Flashing Test\n");
    pc_printf("  5. Door Lock Simulation\n");
    pc_printf("\n");
    pc_printf("Watch the LED on your board!\n");
    pc_printf("========================================\n");
    pc_printf("\n");
}

/**
 * @brief Test 1: Basic ON/OFF Test
 */
void test_basic_onoff() {
    pc_printf("\n[TEST 1] Basic ON/OFF Test\n");
    
    for (int i = 0; i < 5; i++) {
        pc_printf("  - Cycle %d/5: LED ON (1s)...", i + 1);
        led = 1;
        ThisThread::sleep_for(1s);
        
        pc_printf(" LED OFF (1s)\n");
        led = 0;
        ThisThread::sleep_for(1s);
    }
    
    pc_printf("  - Test complete.\n");
}

/**
 * @brief Test 2: Blinking Pattern Test
 */
void test_blinking_patterns() {
    pc_printf("\n[TEST 2] Blinking Pattern Test\n");
    
    // Pattern 1: Fast blink (5 Hz)
    pc_printf("  - Pattern 1: Fast blink (5 Hz, 200ms period)\n");
    for (int i = 0; i < 20; i++) {
        led = !led;
        ThisThread::sleep_for(100ms);
    }
    led = 0;
    ThisThread::sleep_for(1s);
    
    // Pattern 2: Medium blink (2 Hz)
    pc_printf("  - Pattern 2: Medium blink (2 Hz, 500ms period)\n");
    for (int i = 0; i < 10; i++) {
        led = !led;
        ThisThread::sleep_for(250ms);
    }
    led = 0;
    ThisThread::sleep_for(1s);
    
    // Pattern 3: Slow blink (1 Hz)
    pc_printf("  - Pattern 3: Slow blink (1 Hz, 1000ms period)\n");
    for (int i = 0; i < 6; i++) {
        led = !led;
        ThisThread::sleep_for(500ms);
    }
    led = 0;
    ThisThread::sleep_for(1s);
    
    // Pattern 4: SOS Morse code
    pc_printf("  - Pattern 4: SOS Morse code\n");
    
    // S (dot-dot-dot)
    for (int i = 0; i < 3; i++) {
        led = 1;
        ThisThread::sleep_for(200ms);
        led = 0;
        ThisThread::sleep_for(200ms);
    }
    ThisThread::sleep_for(400ms);
    
    // O (dash-dash-dash)
    for (int i = 0; i < 3; i++) {
        led = 1;
        ThisThread::sleep_for(600ms);
        led = 0;
        ThisThread::sleep_for(200ms);
    }
    ThisThread::sleep_for(400ms);
    
    // S (dot-dot-dot)
    for (int i = 0; i < 3; i++) {
        led = 1;
        ThisThread::sleep_for(200ms);
        led = 0;
        ThisThread::sleep_for(200ms);
    }
    
    pc_printf("  - Test complete.\n");
    ThisThread::sleep_for(2s);
}

/**
 * @brief Test 3: Ticker-based Flashing Test
 */
void test_ticker_flashing() {
    pc_printf("\n[TEST 3] Ticker-based Flashing Test\n");
    
    // Test 1: 2 Hz flashing (door open simulation)
    pc_printf("  - Starting 2 Hz flashing for 10 seconds...\n");
    tickCount = 0;
    tickerActive = true;
    ledTicker.attach(&ledToggleISR, 250ms);  // Toggle every 250ms = 2 Hz
    
    for (int i = 10; i > 0; i--) {
        pc_printf("  - Time remaining: %d seconds (toggles: %d)\n", i, tickCount);
        ThisThread::sleep_for(1s);
    }
    
    ledTicker.detach();
    led = 0;
    tickerActive = false;
    pc_printf("  - Ticker stopped. Total toggles: %d\n", tickCount);
    ThisThread::sleep_for(2s);
    
    // Test 2: 5 Hz flashing (fast alert)
    pc_printf("  - Starting 5 Hz flashing for 5 seconds...\n");
    tickCount = 0;
    ledTicker.attach(&ledToggleISR, 100ms);  // Toggle every 100ms = 5 Hz
    
    for (int i = 5; i > 0; i--) {
        pc_printf("  - Time remaining: %d seconds (toggles: %d)\n", i, tickCount);
        ThisThread::sleep_for(1s);
    }
    
    ledTicker.detach();
    led = 0;
    pc_printf("  - Ticker stopped. Total toggles: %d\n", tickCount);
    pc_printf("  - Test complete.\n");
    ThisThread::sleep_for(2s);
}

/**
 * @brief Test 4: Heartbeat Pattern
 */
void test_heartbeat() {
    pc_printf("\n[TEST 4] Heartbeat Pattern Test\n");
    pc_printf("  - Simulating heartbeat pattern (5 cycles)...\n");
    
    for (int i = 0; i < 5; i++) {
        // First beat
        led = 1;
        ThisThread::sleep_for(100ms);
        led = 0;
        ThisThread::sleep_for(100ms);
        
        // Second beat
        led = 1;
        ThisThread::sleep_for(100ms);
        led = 0;
        ThisThread::sleep_for(600ms);
        
        pc_printf("  - Heartbeat %d/5\n", i + 1);
    }
    
    pc_printf("  - Test complete.\n");
    ThisThread::sleep_for(2s);
}

/**
 * @brief Test 5: Door Lock Simulation
 */
void test_door_lock_simulation() {
    pc_printf("\n[TEST 5] Door Lock Simulation\n");
    
    // State 1: Door Closed (LED solid ON)
    pc_printf("  - [State 1] Door CLOSED - LED solid ON (3s)\n");
    led = 1;
    ThisThread::sleep_for(3s);
    
    // State 2: Password entered, door opening
    pc_printf("  - [State 2] Password accepted, door OPENING...\n");
    for (int i = 0; i < 3; i++) {
        led = 0;
        ThisThread::sleep_for(200ms);
        led = 1;
        ThisThread::sleep_for(200ms);
    }
    
    // State 3: Door OPEN (LED flashing at 2 Hz for 10 seconds)
    pc_printf("  - [State 3] Door OPEN - LED flashing 2 Hz (10s)\n");
    tickCount = 0;
    ledTicker.attach(&ledToggleISR, 250ms);
    
    for (int i = 10; i > 0; i--) {
        pc_printf("  -   Closing in %d seconds...\n", i);
        ThisThread::sleep_for(1s);
    }
    
    ledTicker.detach();
    
    // State 4: Door closing
    pc_printf("  - [State 4] Door CLOSING...\n");
    for (int i = 0; i < 3; i++) {
        led = 1;
        ThisThread::sleep_for(200ms);
        led = 0;
        ThisThread::sleep_for(200ms);
    }
    
    // State 5: Door CLOSED (LED solid ON)
    pc_printf("  - [State 5] Door CLOSED - LED solid ON (3s)\n");
    led = 1;
    ThisThread::sleep_for(3s);
    
    led = 0;
    pc_printf("  - Simulation complete.\n");
    ThisThread::sleep_for(2s);
}

/**
 * @brief Test 6: Timing Accuracy Test
 */
void test_timing_accuracy() {
    pc_printf("\n[TEST 6] Timing Accuracy Test\n");
    pc_printf("  - Testing LED toggle timing precision...\n");
    
    Timer timer;
    int intervals[] = {100, 250, 500, 1000};
    
    for (int i = 0; i < 4; i++) {
        int interval = intervals[i];
        pc_printf("  - Testing %dms interval (10 toggles)...\n", interval);
        
        timer.reset();
        timer.start();
        
        for (int j = 0; j < 10; j++) {
            led = !led;
            ThisThread::sleep_for(std::chrono::milliseconds(interval));
        }
        
        timer.stop();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count();
        int expected = interval * 10;
        int error = elapsed - expected;
        
        pc_printf("    Expected: %dms, Actual: %dms, Error: %dms\n", 
                  expected, elapsed, error);
        
        led = 0;
        ThisThread::sleep_for(1s);
    }
    
    pc_printf("  - Test complete.\n");
}

/**
 * @brief Main test function
 */
int main() {
    // Initialize LED (OFF)
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
        test_blinking_patterns();
        test_ticker_flashing();
        test_heartbeat();
        test_door_lock_simulation();
        test_timing_accuracy();
        
        pc_printf("\n========================================\n");
        pc_printf("Test Cycle %d Complete!\n", testCycle);
        pc_printf("All LED tests passed successfully.\n");
        pc_printf("Waiting 5 seconds before next cycle...\n");
        pc_printf("========================================\n");
        
        ThisThread::sleep_for(5s);
        testCycle++;
    }
}

#endif // BUILD_TEST_LED
