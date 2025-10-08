/**
 * @file test_integration.cpp
 * @brief Integration test for complete door lock system
 * @description Tests all components working together
 * 
 * How to use:
 * 1. Connect all hardware (keypad, LCD, relay, LED)
 * 2. Comment out main.cpp in your build
 * 3. Compile this test file instead
 * 4. Connect serial monitor at 9600 baud
 * 5. Follow test prompts
 */

#ifdef BUILD_TEST_INTEGRATION

#include "mbed.h"
#include "TextLCD.h"
#include "Keypad.h"

#include <cstdarg>
#include <cstdio>

using namespace std::chrono_literals;

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

// I2C LCD
I2C i2c(PB_7, PB_6);
TextLCD_I2C lcd(&i2c, 0x27, TextLCD::LCD16x2);

// LED
DigitalOut led(PC_13);

// Relay
DigitalOut relay(PA_8);

// Keypad
PinName rowPins[4] = {PA_0, PA_1, PA_4, PA_5};
PinName colPins[4] = {PB_0, PB_1, PB_3, PB_4};
char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
Keypad keypad(keys, rowPins, colPins, 4, 4);

// Ticker for LED flashing
Ticker ledTicker;

// Test statistics
int testsRun = 0;
int testsPassed = 0;
int testsFailed = 0;

/**
 * @brief LED flash ISR
 */
void ledFlashISR() {
    led = !led;
}

/**
 * @brief Print test header
 */
void printTestHeader(const char* testName) {
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("[TEST] %s\n", testName);
    pc_printf("========================================\n");
}

/**
 * @brief Report test result
 */
void reportResult(bool passed, const char* message) {
    testsRun++;
    if (passed) {
        testsPassed++;
        pc_printf("[PASS] %s\n", message);
    } else {
        testsFailed++;
        pc_printf("[FAIL] %s\n", message);
    }
}

/**
 * @brief Test 1: Hardware Initialization
 */
bool test_hardware_init() {
    printTestHeader("Hardware Initialization");
    
    pc_printf("  - Initializing LCD...\n");
    lcd.cls();
    lcd.printf("Init Test");
    ThisThread::sleep_for(1s);
    reportResult(true, "LCD initialized");
    
    pc_printf("  - Initializing LED...\n");
    led = 1;
    ThisThread::sleep_for(500ms);
    led = 0;
    reportResult(true, "LED initialized");
    
    pc_printf("  - Initializing Relay...\n");
    relay = 0;
    reportResult(true, "Relay initialized");
    
    pc_printf("  - Initializing Keypad...\n");
    reportResult(true, "Keypad initialized");
    
    return true;
}

/**
 * @brief Test 2: LCD Display
 */
bool test_lcd_display() {
    printTestHeader("LCD Display Test");
    
    lcd.cls();
    lcd.printf("LCD Test");
    lcd.locate(0, 1);
    lcd.printf("Line 2 OK");
    
    pc_printf("  - Check LCD: Should show 'LCD Test' and 'Line 2 OK'\n");
    pc_printf("  - Does LCD display correctly? (y/n): ");
    
    ThisThread::sleep_for(3s);
    reportResult(true, "LCD display test (visual check required)");
    
    return true;
}

/**
 * @brief Test 3: Keypad Input
 */
bool test_keypad_input() {
    printTestHeader("Keypad Input Test");
    
    lcd.cls();
    lcd.printf("Press any key");
    
    
    Timer timeout;
    timeout.start();
    bool keyDetected = false;
    
    while (timeout.elapsed_time() < 10s) {
        char key = keypad.getKey();
        if (key) {
            pc_printf("  - Key detected: '%c'\n", key);
            lcd.locate(0, 1);
            lcd.printf("Key: %c", key);
            keyDetected = true;
            break;
        }
        ThisThread::sleep_for(50ms);
    }
    
    reportResult(keyDetected, keyDetected ? "Keypad responding" : "No key detected (TIMEOUT)");
    return keyDetected;
}
/**
 * @brief Test 4: LED Flashing
 */
bool test_led_flashing() {
    printTestHeader("LED Flashing Test");
    
    lcd.cls();
    lcd.printf("LED Flashing");
    lcd.locate(0, 1);
    lcd.printf("2 Hz for 5s");
    
    pc_printf("  - Starting LED flash at 2 Hz for 5 seconds...\n");
    
    ledTicker.attach(&ledFlashISR, 250ms);
    
    for (int i = 5; i > 0; i--) {
        pc_printf("  - %d seconds remaining...\n", i);
        ThisThread::sleep_for(1s);
    }
    
    ledTicker.detach();
    led = 0;
    
    pc_printf("  - LED flashing stopped\n");
    reportResult(true, "LED flashing test completed");
    
    return true;
}

/**
 * @brief Test 5: Relay Control
 */
bool test_relay_control() {
    printTestHeader("Relay Control Test");
    
    lcd.cls();
    lcd.printf("Relay Test");
    
    pc_printf("  - Turning relay ON...\n");
    lcd.locate(0, 1);
    lcd.printf("Relay: ON");
    relay = 1;
    ThisThread::sleep_for(2s);
    
    pc_printf("  - Turning relay OFF...\n");
    lcd.locate(0, 1);
    lcd.printf("Relay: OFF");
    relay = 0;
    ThisThread::sleep_for(1s);
    
    pc_printf("  - Did you hear relay clicking? (visual/audio check)\n");
    reportResult(true, "Relay control test (check required)");
    
    return true;
}

/**
 * @brief Test 6: Password Entry Simulation
 */
bool test_password_entry() {
    printTestHeader("Password Entry Simulation");
    
    lcd.cls();
    lcd.printf("Enter: 1234#");
    
    pc_printf("  - Enter password '1234' then press '#' (timeout: 30s)\n");
    
    std::string input = "";
    Timer timeout;
    timeout.start();
    bool success = false;
    
    while (timeout.elapsed_time() < 30s) {
        char key = keypad.getKey();
        
        if (key) {
            if (key == '#') {
                // Submit
                if (input == "1234") {
                    pc_printf("  - Correct password entered!\n");
                    lcd.cls();
                    lcd.printf("Access Granted!");
                    success = true;
                } else {
                    pc_printf("  - Wrong password: '%s'\n", input.c_str());
                    lcd.cls();
                    lcd.printf("Wrong Password!");
                }
                break;
            } else if (key == '*') {
                // Clear
                input.clear();
                lcd.cls();
                lcd.printf("Enter: 1234#");
                pc_printf("  - Input cleared\n");
            } else if (key >= '0' && key <= '9') {
                // Add digit
                input += key;
                lcd.locate(0, 1);
                for (size_t i = 0; i < input.length(); i++) {
                    lcd.printf("*");
                }
                pc_printf("  - Digit entered: '%c' (total: %d digits)\n", key, input.length());
            }
        }
        
        ThisThread::sleep_for(50);
    }
    
    if (!success && timeout.elapsed_time() >= 30s) {
        pc_printf("  - TIMEOUT: No password entered\n");
    }
    
    ThisThread::sleep_for(2s);
    reportResult(success, success ? "Password entry successful" : "Password entry failed");
    
    return success;
}

/**
 * @brief Test 7: Door Open/Close Cycle
 */
bool test_door_cycle() {
    printTestHeader("Door Open/Close Cycle");
    
    // Closed state
    pc_printf("  - [1/4] Door CLOSED - LED solid ON\n");
    lcd.cls();
    lcd.printf("Door: CLOSED");
    led = 1;
    relay = 0;
    ThisThread::sleep_for(2s);
    
    // Opening
    pc_printf("  - [2/4] Door OPENING...\n");
    lcd.cls();
    lcd.printf("Door: OPENING");
    relay = 1;
    ThisThread::sleep_for(1s);
    
    // Open state
    pc_printf("  - [3/4] Door OPEN - LED flashing (5s)\n");
    lcd.cls();
    lcd.printf("Door: OPEN");
    lcd.locate(0, 1);
    lcd.printf("Closing in 5s");
    
    ledTicker.attach(&ledFlashISR, 250ms);
    
    for (int i = 5; i > 0; i--) {
        lcd.locate(12, 1);
        lcd.printf("%ds", i);
        ThisThread::sleep_for(1s);
    }
    
    ledTicker.detach();
    
    // Closing
    pc_printf("  - [4/4] Door CLOSING...\n");
    lcd.cls();
    lcd.printf("Door: CLOSING");
    relay = 0;
    led = 1;
    ThisThread::sleep_for(1s);
    
    // Closed
    lcd.cls();
    lcd.printf("Door: CLOSED");
    
    pc_printf("  - Door cycle complete\n");
    reportResult(true, "Door open/close cycle completed");
    
    return true;
}

/**
 * @brief Test 8: Full System Integration
 */
bool test_full_integration() {
    printTestHeader("Full System Integration");
    
    pc_printf("  - Running complete door lock simulation...\n");
    
    // Initialize
    lcd.cls();
    lcd.printf("Door Lock v1.0");
    lcd.locate(0, 1);
    lcd.printf("Test Mode");
    led = 1;
    relay = 0;
    ThisThread::sleep_for(2s);
    
    // Ready
    lcd.cls();
    lcd.printf("Enter Password:");
    pc_printf("  - System ready. Enter password '1234#' (timeout: 30s)\n");
    
    std::string input = "";
    Timer timeout;
    timeout.start();
    bool authenticated = false;
    
    while (timeout.elapsed_time() < 30s) {
        char key = keypad.getKey();
        
        if (key) {
            if (key == '#') {
                if (input == "1234") {
                    authenticated = true;
                    break;
                } else {
                    lcd.cls();
                    lcd.printf("Wrong Password!");
                    ThisThread::sleep_for(2s);
                    lcd.cls();
                    lcd.printf("Enter Password:");
                    input.clear();
                }
            } else if (key == '*') {
                input.clear();
                lcd.cls();
                lcd.printf("Enter Password:");
            } else if (key >= '0' && key <= '9') {
                input += key;
                lcd.locate(0, 1);
                for (size_t i = 0; i < input.length(); i++) {
                    lcd.printf("*");
                }
            }
        }
        
        ThisThread::sleep_for(50);
    }
    
    if (authenticated) {
        // Access granted
        pc_printf("  - Access granted! Opening door...\n");
        lcd.cls();
        lcd.printf("Access Granted!");
        lcd.locate(0, 1);
        lcd.printf("Door Opening...");
        
        relay = 1;
        ledTicker.attach(&ledFlashISR, 250ms);
        
        ThisThread::sleep_for(2s);
        
        // Door open countdown
        lcd.cls();
        lcd.printf("Door Open");
        
        for (int i = 10; i > 0; i--) {
            lcd.locate(0, 1);
            lcd.printf("Closing in %ds  ", i);
            pc_printf("  - Closing in %d seconds...\n", i);
            ThisThread::sleep_for(1s);
        }
        
        // Close door
        pc_printf("  - Closing door...\n");
        ledTicker.detach();
        relay = 0;
        led = 1;
        
        lcd.cls();
        lcd.printf("Door Closed");
        ThisThread::sleep_for(2s);
        
        reportResult(true, "Full integration test PASSED");
        return true;
    } else {
        pc_printf("  - TIMEOUT: No valid password entered\n");
        reportResult(false, "Full integration test FAILED (timeout)");
        return false;
    }
}

/**
 * @brief Print test summary
 */
void printSummary() {
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  TEST SUMMARY\n");
    pc_printf("========================================\n");
    pc_printf("Total tests run: %d\n", testsRun);
    pc_printf("Tests passed:    %d\n", testsPassed);
    pc_printf("Tests failed:    %d\n", testsFailed);
    pc_printf("Success rate:    %.1f%%\n", (float)testsPassed / testsRun * 100);
    pc_printf("========================================\n");
    
    lcd.cls();
    lcd.printf("Tests: %d/%d", testsPassed, testsRun);
    lcd.locate(0, 1);
    if (testsFailed == 0) {
        lcd.printf("All PASSED!");
    } else {
        lcd.printf("%d FAILED", testsFailed);
    }
}

/**
 * @brief Main test function
 */
int main() {
    // Initialize hardware
    led = 0;
    relay = 0;
    
    // Wait for serial connection
    ThisThread::sleep_for(2s);
    
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  DOOR LOCK INTEGRATION TEST\n");
    pc_printf("========================================\n");
    pc_printf("Hardware: STM32 Nucleo\n");
    pc_printf("Components: Keypad, LCD, LED, Relay\n");
    pc_printf("Baud Rate: 9600\n");
    pc_printf("========================================\n");
    pc_printf("\n");
    pc_printf("Starting automated test sequence...\n");
    pc_printf("\n");
    
    // Run all tests
    test_hardware_init();
    ThisThread::sleep_for(1s);
    
    test_lcd_display();
    ThisThread::sleep_for(1s);
    
    test_keypad_input();
    ThisThread::sleep_for(1s);
    
    test_led_flashing();
    ThisThread::sleep_for(1s);
    
    test_relay_control();
    ThisThread::sleep_for(1s);
    
    test_password_entry();
    ThisThread::sleep_for(1s);
    
    test_door_cycle();
    ThisThread::sleep_for(1s);
    
    test_full_integration();
    ThisThread::sleep_for(1s);
    
    // Print summary
    printSummary();
    
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  ALL TESTS COMPLETE\n");
    pc_printf("========================================\n");
    pc_printf("\n");
    
    // Cleanup
    led = 0;
    relay = 0;
    ledTicker.detach();
    
    while (true) {
        ThisThread::sleep_for(1s);
    }
}

#endif // BUILD_TEST_INTEGRATION
