/**
 * @file test_keypad.cpp
 * @brief Test program for 4x4 Matrix Keypad
 * @description Tests keypad functionality by reading and displaying pressed keys
 * 
 * How to use:
 * 1. Comment out main.cpp in your build
 * 2. Compile this test file instead
 * 3. Connect serial monitor at 9600 baud
 * 4. Press keys and verify output
 */

#ifdef BUILD_TEST_KEYPAD
#include "mbed.h"
#include "Keypad.h"

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

// Keypad Configuration
PinName rowPins[4] = {PA_0, PA_1, PA_4, PA_5};
PinName colPins[4] = {PB_0, PB_1, PB_3, PB_4};

char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

Keypad keypad(keys, rowPins, colPins, 4, 4);

// LED for visual feedback
DigitalOut led(PC_13);

// Test statistics
int keyPressCount = 0;
int lastKeyCount[16] = {0}; // Counter for each key

/**
 * @brief Maps key character to index for statistics
 */
int getKeyIndex(char key) {
    const char* keyMap = "123A456B789C*0#D";
    for (int i = 0; i < 16; i++) {
        if (keyMap[i] == key) return i;
    }
    return -1;
}

/**
 * @brief Prints test header
 */
void printHeader() {
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  KEYPAD TEST PROGRAM\n");
    pc_printf("========================================\n");
    pc_printf("Hardware: STM32 Nucleo\n");
    pc_printf("Keypad: 4x4 Matrix\n");
    pc_printf("Baud Rate: 9600\n");
    pc_printf("========================================\n");
    pc_printf("\n");
    pc_printf("Keypad Layout:\n");
    pc_printf("  +---+---+---+---+\n");
    pc_printf("  | 1 | 2 | 3 | A |\n");
    pc_printf("  +---+---+---+---+\n");
    pc_printf("  | 4 | 5 | 6 | B |\n");
    pc_printf("  +---+---+---+---+\n");
    pc_printf("  | 7 | 8 | 9 | C |\n");
    pc_printf("  +---+---+---+---+\n");
    pc_printf("  | * | 0 | # | D |\n");
    pc_printf("  +---+---+---+---+\n");
    pc_printf("\n");
    pc_printf("Press keys to test...\n");
    pc_printf("Press 'D' to show statistics\n");
    pc_printf("\n");
}

/**
 * @brief Prints test statistics
 */
void printStatistics() {
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  KEYPAD STATISTICS\n");
    pc_printf("========================================\n");
    pc_printf("Total key presses: %d\n", keyPressCount);
    pc_printf("\n");
    pc_printf("Individual key counts:\n");
    
    const char* keyMap = "123A456B789C*0#D";
    for (int i = 0; i < 16; i++) {
        if (lastKeyCount[i] > 0) {
            pc_printf("  Key '%c': %d times\n", keyMap[i], lastKeyCount[i]);
        }
    }
    pc_printf("========================================\n");
    pc_printf("\n");
}

/**
 * @brief Main test function
 */
int main() {
    // Initialize LED (OFF = 0)
    led = 0;
    
    // Wait for serial connection
    ThisThread::sleep_for(2s);

    // Print header
    printHeader();
    
    pc_printf("[READY] Waiting for key presses...\n\n");
    
    // Main test loop
    while (true) {
        char key = keypad.getKey();
        
        if (key) {
            // Increment counters
            keyPressCount++;
            int idx = getKeyIndex(key);
            if (idx >= 0) {
                lastKeyCount[idx]++;
            }
            
            // Flash LED for visual feedback
            led = 1;
            
            // Print key information
            pc_printf("[%d] Key Pressed: '%c' (ASCII: %d)\n", 
                      keyPressCount, key, (int)key);

            // Special commands
            if (key == 'D') {
                ThisThread::sleep_for(500ms);
                printStatistics();
            } else if (key == '*') {
                pc_printf("     --> Clear command detected\n");
            } else if (key == '#') {
                pc_printf("     --> Submit command detected\n");
            } else if (key == 'C') {
                pc_printf("     --> Cancel command detected\n");
            }
            
            // Turn off LED after short delay
            ThisThread::sleep_for(100ms);
            led = 0;
        }
        
        ThisThread::sleep_for(50ms);  // 50ms scan rate
    }
}

#endif // BUILD_TEST_KEYPAD
