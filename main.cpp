/**
 * @file main.cpp
 * @brief Electronic Door Lock System with Keypad Authentication
 * @author Mugabo Martin
 * @date 07.10.2025
 * 
 * Features:
 * - 4x4 Matrix keypad for password entry
 * - 16x2 LCD display for user feedback
 * - LED indicator (ON when closed, FLASHING when open)
 * - Password masking for security
 * - Failed attempt counter with lockout
 * - 10-second auto-close timer
 */

#ifndef BUILD_TESTS

#include <mbed.h>
#include "TextLCD.h"
#include "Keypad.h"
#include "config.h"  

// ==================== HARDWARE I/O ====================
// I2C LCD Display (16x2)
I2C i2c(PB_7, PB_6);                 // SDA, SCL pins
TextLCD_I2C lcd(&i2c, 0x27, TextLCD::LCD16x2);

// LED Indicator (built-in LED on most Nucleo boards)
DigitalOut led(PC_13);

// Lock Control (Relay or Servo)
#if USE_RELAY
    DigitalOut lockControl(PA_8);    // Relay control pin
#else
    PwmOut lockControl(PA_8);        // Servo control pin (PWM)
#endif

// Keypad Configuration (4x4 Matrix)
PinName rowPins[ROWS] = {PA_0, PA_1, PA_4, PA_5};  // Row pins
PinName colPins[COLS] = {PB_0, PB_1, PB_3, PB_4};  // Column pins

// Keypad layout mapping
char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

Keypad keypad(keys, rowPins, colPins, ROWS, COLS);

// ==================== GLOBAL VARIABLES ====================
std::string inputPassword = "";      // User input buffer
int failedAttempts = 0;              // Failed login counter
bool isDoorOpen = false;             // Door state flag
bool isLockedOut = false;            // Lockout state flag

// Timers
Timer doorTimer;                     // Timer for auto-close
Timer lockoutTimer;                  // Timer for lockout period
Ticker ledTicker;                    // Ticker for LED flashing

// ==================== FUNCTION PROTOTYPES ====================
void openLock();
void closeLock();
void checkPassword();
void ledFlashISR();
void updateLCD();
void resetSystem();
void handleSpecialKeys(char key);

// ==================== LED FLASHING ISR ====================
/**
 * @brief Interrupt Service Routine for LED flashing
 * Called by Ticker every LED_FLASH_PERIOD_MS/2 milliseconds
 */
void ledFlashISR() {
    if (isDoorOpen) {
        led = !led;  // Toggle LED state
    }
}

// ==================== LOCK CONTROL FUNCTIONS ====================
/**
 * @brief Opens the door lock and starts LED flashing
 */
void openLock() {
    isDoorOpen = true;
    
    #if USE_RELAY
        lockControl = 1;             // Energize relay (lock opens)
    #else
        lockControl.period_ms(20);   // 50Hz for servo
        lockControl.pulsewidth_ms(2);// 2ms pulse = 90 degrees
    #endif
    
    // Start LED flashing at 2Hz (toggle every 250ms)
    ledTicker.attach(&ledFlashISR, 250ms);
    
    // Start door timer
    doorTimer.reset();
    doorTimer.start();
}

/**
 * @brief Closes the door lock and turns LED solid ON
 */
void closeLock() {
    isDoorOpen = false;
    
    #if USE_RELAY
        lockControl = 0;             // De-energize relay (lock closes)
    #else
        lockControl.period_ms(20);
        lockControl.pulsewidth_ms(1);// 1ms pulse = 0 degrees
    #endif
    
    // Stop LED flashing and turn it ON solid
    ledTicker.detach();
    led = 1;  // LED ON when door is closed
    
    doorTimer.stop();
}

// ==================== PASSWORD VALIDATION ====================
/**
 * @brief Validates entered password and controls access
 */
void checkPassword() {
    // Check if system is locked out
    if (isLockedOut) {
        lcd.cls();
        lcd.printf("LOCKED OUT!");
        lcd.locate(0, 1);
        int remaining = (LOCKOUT_TIME_MS - lockoutTimer.read_ms()) / 1000;
        lcd.printf("Wait %ds", remaining);
        return;
    }
    
    // Validate password
    if (inputPassword == PASSWORD) {
        // CORRECT PASSWORD
        lcd.cls();
        lcd.printf("Access Granted!");
        lcd.locate(0, 1);
        lcd.printf("Door Opening...");
        
        failedAttempts = 0;  // Reset failed attempts
        openLock();
        
    } else {
        // WRONG PASSWORD
        failedAttempts++;
        lcd.cls();
        lcd.printf("Wrong Password!");
        lcd.locate(0, 1);
        lcd.printf("Attempts: %d/%d", failedAttempts, MAX_FAILED_ATTEMPTS);
        
        // Check if max attempts reached
        if (failedAttempts >= MAX_FAILED_ATTEMPTS) {
            ThisThread::sleep_for(2000);
            lcd.cls();
            lcd.printf("TOO MANY TRIES!");
            lcd.locate(0, 1);
            lcd.printf("Locked 30s");
            
            isLockedOut = true;
            lockoutTimer.reset();
            lockoutTimer.start();
        }
    }
    
    // Clear input buffer
    inputPassword.clear();
    ThisThread::sleep_for(2000);
    updateLCD();
}

// ==================== LCD UPDATE ====================
/**
 * @brief Updates LCD display based on system state
 */
void updateLCD() {
    lcd.cls();
    
    if (isLockedOut) {
        lcd.printf("LOCKED OUT!");
        lcd.locate(0, 1);
        int remaining = (LOCKOUT_TIME_MS - lockoutTimer.read_ms()) / 1000;
        lcd.printf("Wait %ds", remaining);
    } else if (isDoorOpen) {
        lcd.printf("Door Open");
        lcd.locate(0, 1);
        int remaining = (OPEN_TIME_MS - doorTimer.read_ms()) / 1000;
        lcd.printf("Closing in %ds", remaining);
    } else {
        lcd.printf("Enter Password:");
        lcd.locate(0, 1);
        // Display masked password (asterisks)
        for (size_t i = 0; i < inputPassword.length(); i++) {
            lcd.printf("*");
        }
    }
}

// ==================== SYSTEM RESET ====================
/**
 * @brief Resets system after lockout period
 */
void resetSystem() {
    isLockedOut = false;
    failedAttempts = 0;
    inputPassword.clear();
    lockoutTimer.stop();
    updateLCD();
}

// ==================== SPECIAL KEY HANDLER ====================
/**
 * @brief Handles special keys A, B, C, D with custom functionality
 * @param key The special key pressed ('A', 'B', 'C', 'D')
 */
void handleSpecialKeys(char key) {
    switch (key) {
        case 'A':
            // A: Display system information
            lcd.cls();
            lcd.printf("Door Lock v1.0");
            lcd.locate(0, 1);
            lcd.printf("Attempts: %d", failedAttempts);
            break;
            
        case 'B':
            // B: Toggle LED brightness (simulate with flash rate)
            static bool fastFlash = false;
            fastFlash = !fastFlash;
            if (fastFlash) {
                ledTicker.attach(&ledFlashISR, 100ms);  // Faster flash
                lcd.cls();
                lcd.printf("Fast Flash ON");
            } else {
                ledTicker.attach(&ledFlashISR, 250ms);  // Normal flash
                lcd.cls();
                lcd.printf("Normal Flash");
            }
            ThisThread::sleep_for(2000);
            break;
            
        case 'C':
            // C: Clear failed attempts (admin function)
            if (failedAttempts > 0) {
                failedAttempts = 0;
                lcd.cls();
                lcd.printf("Attempts Reset");
                ThisThread::sleep_for(2000);
            } else {
                lcd.cls();
                lcd.printf("No Attempts");
                ThisThread::sleep_for(2000);
            }
            break;
            
        case 'D':
            // D: Display lock status and system state
            lcd.cls();
            if (isDoorOpen) {
                lcd.printf("Door: OPEN");
                int remaining = (OPEN_TIME_MS - doorTimer.read_ms()) / 1000;
                lcd.locate(0, 1);
                lcd.printf("Closes in %ds", remaining);
            } else if (isLockedOut) {
                lcd.printf("Door: LOCKED");
                int remaining = (LOCKOUT_TIME_MS - lockoutTimer.read_ms()) / 1000;
                lcd.locate(0, 1);
                lcd.printf("Unlock in %ds", remaining);
            } else {
                lcd.printf("Door: CLOSED");
                lcd.locate(0, 1);
                lcd.printf("Ready");
            }
            ThisThread::sleep_for(3000);
            break;
    }
    updateLCD();  // Return to normal display
}

// ==================== MAIN PROGRAM ====================
int main() {
    // Initialize system
    lcd.cls();
    lcd.printf("Door Lock v1.0");
    lcd.locate(0, 1);
    lcd.printf("Initializing...");
    ThisThread::sleep_for(2000);
    
    // Close lock and turn LED ON
    closeLock();
    
    // Display ready message
    lcd.cls();
    lcd.printf("System Ready!");
    ThisThread::sleep_for(1000);
    updateLCD();
    
    // Start timers
    doorTimer.start();
    lockoutTimer.start();
    
    // ==================== MAIN LOOP ====================
    while (true) {
        // Check if lockout period expired
        if (isLockedOut && lockoutTimer.read_ms() >= LOCKOUT_TIME_MS) {
            resetSystem();
        }
        
        // Check if door should auto-close
        if (isDoorOpen && doorTimer.read_ms() >= OPEN_TIME_MS) {
            closeLock();
            updateLCD();
        }
        
        // Update LCD if door is open (countdown timer)
        if (isDoorOpen) {
            updateLCD();
        }
        
        // Read keypad input
        char key = keypad.getKey();
        if (key && !isDoorOpen) {  // Ignore input when door is open
            if (key == '#') {
                // Submit password
                if (inputPassword.length() > 0) {
                    checkPassword();
                } else {
                    // No password entered - show message
                    lcd.cls();
                    lcd.printf("Enter Password");
                    lcd.locate(0, 1);
                    lcd.printf("First!");
                    ThisThread::sleep_for(2000);
                    updateLCD();
                }
            } else if (key == '*') {
                // Clear input with confirmation
                if (inputPassword.length() > 0) {
                    inputPassword.clear();
                    updateLCD();
                } else {
                    // No input to clear - show message
                    lcd.cls();
                    lcd.printf("Nothing to Clear");
                    ThisThread::sleep_for(1500);
                    updateLCD();
                }
            } else if (key >= '0' && key <= '9') {
                // Add digit to password (max MAX_PASSWORD_LENGTH digits)
                if (inputPassword.length() < MAX_PASSWORD_LENGTH) {
                    inputPassword += key;
                    updateLCD();
                }
            } else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') {
                // Handle special keys
                handleSpecialKeys(key);
            }
        }
        
        ThisThread::sleep_for(100);  // 100ms loop delay
    }
}
