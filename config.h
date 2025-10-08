#ifndef CONFIG_H
#define CONFIG_H

// ==================== PASSWORD SETTINGS ====================
#define PASSWORD "1234"              // Default password (4 digits)
#define MAX_PASSWORD_LENGTH 8        // Maximum password length

// ================ KEYPAD SETTINGS =======================
#define ROWS 4                      // Number of rows in keypad
#define COLS 4                      // Number of columns in keypad

// ==================== TIMING SETTINGS ====================
#define OPEN_TIME_MS 10000           // Door open duration (milliseconds)
#define LED_FLASH_PERIOD_MS 500      // LED flash period (500ms = 2Hz)
#define LOCKOUT_TIME_MS 30000        // Lockout duration (30 seconds)
#define DEBOUNCE_TIME_MS 50          // Keypad debounce delay

// ==================== SECURITY SETTINGS ====================
#define MAX_FAILED_ATTEMPTS 3        // Max wrong attempts before lockout

// ==================== HARDWARE SETTINGS ====================
#define USE_RELAY true               // true = relay, false = servo

#endif