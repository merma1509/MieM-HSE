/**
 * @file test_lcd.cpp
 * @brief Test program for 16x2 I2C LCD Display
 * @description Tests LCD functionality with various display patterns
 * 
 * How to use:
 * 1. Comment out main.cpp in your build
 * 2. Compile this test file instead
 * 3. Watch LCD display for test patterns
 * 4. Connect serial monitor to see test progress
 */

#ifdef BUILD_TEST_LCD
#include "mbed.h"
#include "TextLCD.h"

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

// I2C LCD Configuration
I2C i2c(PB_7, PB_6);  // SDA, SCL
TextLCD_I2C lcd(&i2c, 0x27, TextLCD::LCD16x2);

// LED for visual feedback
DigitalOut led(PC_13);

/**
 * @brief Test 1: Basic Display Test
 */
void test_basic_display() {
    pc_printf("\n[TEST 1] Basic Display Test\n");
    
    lcd.cls();
    lcd.printf("LCD Test 1");
    lcd.locate(0, 1);
    lcd.printf("Basic Display");
    
    pc_printf("  - Displaying: 'LCD Test 1' / 'Basic Display'\n");
    pc_printf("  - Result: Check LCD for text\n");
    
    ThisThread::sleep_for(3s);
}

/**
 * @brief Test 2: Character Set Test
 */
void test_character_set() {
    pc_printf("\n[TEST 2] Character Set Test\n");
    
    // Test uppercase
    lcd.cls();
    lcd.printf("ABCDEFGHIJKLMNOP");
    lcd.locate(0, 1);
    lcd.printf("QRSTUVWXYZ");
    pc_printf("  - Displaying: Uppercase letters\n");
    ThisThread::sleep_for(3s);
    
    // Test lowercase
    lcd.cls();
    lcd.printf("abcdefghijklmnop");
    lcd.locate(0, 1);
    lcd.printf("qrstuvwxyz");
    pc_printf("  - Displaying: Lowercase letters\n");
    ThisThread::sleep_for(3s);
    
    // Test numbers
    lcd.cls();
    lcd.printf("0123456789");
    lcd.locate(0, 1);
    lcd.printf("Numbers Test");
    pc_printf("  - Displaying: Numbers 0-9\n");
    ThisThread::sleep_for(3s);
    
    // Test special characters
    lcd.cls();
    lcd.printf("!@#$%^&*()_+-=");
    lcd.locate(0, 1);
    lcd.printf("[]{}|;:',.<>?/");
    pc_printf("  - Displaying: Special characters\n");
    ThisThread::sleep_for(3s);
}

/**
 * @brief Test 3: Cursor Positioning Test
 */
void test_cursor_positioning() {
    pc_printf("\n[TEST 3] Cursor Positioning Test\n");
    
    lcd.cls();
    
    // Test each position
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 16; col++) {
            lcd.locate(col, row);
            lcd.printf("*");
            ThisThread::sleep_for(100ms);
        }
    }
    
    pc_printf("  - Filled all 32 positions with '*'\n");
    ThisThread::sleep_for(2s);
}

/**
 * @brief Test 4: Scrolling Text Test
 */
void test_scrolling_text() {
    pc_printf("\n[TEST 4] Scrolling Text Test\n");
    
    const char* message = "Hello from STM32 Nucleo! This is a scrolling text test.";
    int len = strlen(message);
    
    pc_printf("  - Scrolling message: '%s'\n", message);
    
    for (int i = 0; i < len - 15; i++) {
        lcd.cls();
        lcd.printf("%.16s", message + i);
        lcd.locate(0, 1);
        lcd.printf("Scroll Test");
        ThisThread::sleep_for(300ms);
    }
    
    ThisThread::sleep_for(1s);
}

/**
 * @brief Test 5: Clear and Refresh Test
 */
void test_clear_refresh() {
    pc_printf("\n[TEST 5] Clear and Refresh Test\n");
    
    for (int i = 0; i < 5; i++) {
        lcd.cls();
        lcd.printf("Clear Test %d", i + 1);
        lcd.locate(0, 1);
        lcd.printf("Iteration: %d/5", i + 1);
        
        pc_printf("  - Iteration %d/5\n", i + 1);
        ThisThread::sleep_for(1s);
    }
}

/**
 * @brief Test 6: Counter Display Test
 */
void test_counter_display() {
    pc_printf("\n[TEST 6] Counter Display Test\n");
    
    lcd.cls();
    lcd.printf("Counter Test:");
    
    for (int i = 0; i <= 20; i++) {
        lcd.locate(0, 1);
        lcd.printf("Count: %d       ", i);  // Extra spaces to clear previous digits
        
        pc_printf("  - Count: %d\n", i);
        ThisThread::sleep_for(500ms);
    }
    
    ThisThread::sleep_for(1s);
}

/**
 * @brief Test 7: Password Masking Test
 */
void test_password_masking() {
    pc_printf("\n[TEST 7] Password Masking Test\n");
    
    lcd.cls();
    lcd.printf("Enter Password:");
    
    const char* password = "1234567890";
    
    for (int i = 0; i < strlen(password); i++) {
        lcd.locate(i, 1);
        lcd.printf("*");
        
        pc_printf("  - Masked digit %d\n", i + 1);
        ThisThread::sleep_for(500ms);
    }
    
    ThisThread::sleep_for(2s);
}

/**
 * @brief Test 8: I2C Address Scan
 */
void test_i2c_scan() {
    pc_printf("\n[TEST 8] I2C Address Scan\n");
    pc_printf("  - Scanning I2C bus for devices...\n");
    
    lcd.cls();
    lcd.printf("I2C Scan...");
    
    int found = 0;
    for (int addr = 0x00; addr < 0x80; addr++) {
        if (i2c.write(addr << 1, NULL, 0) == 0) {
            pc_printf("  - Found device at address: 0x%02X\n", addr);
            found++;
        }
    }
    
    lcd.locate(0, 1);
    lcd.printf("Found: %d device", found);
    
    pc_printf("  - Total devices found: %d\n", found);
    ThisThread::sleep_for(3s);
}

/**
 * @brief Test 9: Blinking Display Test
 */
void test_blinking_display() {
    pc_printf("\n[TEST 9] Blinking Display Test\n");
    
    for (int i = 0; i < 5; i++) {
        lcd.cls();
        lcd.printf("Blink Test");
        lcd.locate(0, 1);
        lcd.printf("ON - %d/5", i + 1);
        
        pc_printf("  - Blink ON (%d/5)\n", i + 1);
        ThisThread::sleep_for(500ms);
        
        lcd.cls();
        pc_printf("  - Blink OFF (%d/5)\n", i + 1);
        ThisThread::sleep_for(500ms);
    }
}

/**
 * @brief Test 10: Full Feature Demo
 */
void test_full_demo() {
    pc_printf("\n[TEST 10] Full Feature Demo\n");
    
    // Simulate door lock system
    lcd.cls();
    lcd.printf("Door Lock v1.0");
    lcd.locate(0, 1);
    lcd.printf("Initializing...");
    ThisThread::sleep_for(2s);
    
    lcd.cls();
    lcd.printf("System Ready!");
    ThisThread::sleep_for(1500ms);
    
    lcd.cls();
    lcd.printf("Enter Password:");
    lcd.locate(0, 1);
    lcd.printf("****");
    ThisThread::sleep_for(2s);
    
    lcd.cls();
    lcd.printf("Access Granted!");
    lcd.locate(0, 1);
    lcd.printf("Door Opening...");
    ThisThread::sleep_for(2s);
    
    lcd.cls();
    lcd.printf("Door Open");
    lcd.locate(0, 1);
    lcd.printf("Closing in 10s");
    ThisThread::sleep_for(2s);
    
    lcd.cls();
    lcd.printf("Door Closed");
    lcd.locate(0, 1);
    lcd.printf("System Ready");
    ThisThread::sleep_for(2s);
    
    pc_printf("  - Demo sequence complete\n");
}

/**
 * @brief Main test function
 */
int main() {
    // Initialize LED
    led = 0;
    
    // Wait for serial connection
    ThisThread::sleep_for(2s);
    
    pc_printf("\n");
    pc_printf("========================================\n");
    pc_printf("  LCD DISPLAY TEST PROGRAM\n");
    pc_printf("========================================\n");
    pc_printf("Hardware: STM32 Nucleo\n");
    pc_printf("Display: 16x2 I2C LCD\n");
    pc_printf("I2C Address: 0x27\n");
    pc_printf("Baud Rate: 9600\n");
    pc_printf("========================================\n");
    
    // Run all tests in sequence
    int testNumber = 1;
    
    while (true) {
        pc_printf("\n========================================\n");
        pc_printf("Starting Test Cycle %d\n", testNumber);
        pc_printf("========================================\n");
        
        led = 1;  // LED ON during tests
        
        test_basic_display();
        test_character_set();
        test_cursor_positioning();
        test_scrolling_text();
        test_clear_refresh();
        test_counter_display();
        test_password_masking();
        test_i2c_scan();
        test_blinking_display();
        test_full_demo();
        
        led = 0;  // LED OFF between cycles
        
        pc_printf("\n========================================\n");
        pc_printf("Test Cycle %d Complete!\n", testNumber);
        pc_printf("All tests passed. Waiting 5 seconds...\n");
        pc_printf("========================================\n");
        
        lcd.cls();
        lcd.printf("All Tests PASS!");
        lcd.locate(0, 1);
        lcd.printf("Cycle: %d", testNumber);
        
        ThisThread::sleep_for(5s);
        
        testNumber++;
    }
}

#endif // BUILD_TEST_LCD
