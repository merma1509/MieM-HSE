/**
 * @file Keypad.h
 * @brief 4x4 Matrix Keypad Library for Mbed OS
 * @author Door Locker Project
 * @date 2025
 * 
 * This library provides a simple interface for reading keys from a 4x4 matrix keypad.
 * It uses row-column scanning to detect key presses with debouncing.
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include "mbed.h"
#include "config.h"  // Include configuration for DEBOUNCE_TIME_MS

/**
 * @class Keypad
 * @brief Matrix keypad scanner with debouncing
 */
class Keypad {
public:
    /**
     * @brief Constructor for Keypad
     * @param keys 2D array of key characters (rows x cols)
     * @param rowPins Array of row pin names
     * @param colPins Array of column pin names
     * @param rows Number of rows
     * @param cols Number of columns
     */
    Keypad(char keys[][4], PinName* rowPins, PinName* colPins, int rows, int cols);
    
    /**
     * @brief Destructor
     */
    ~Keypad();
    
    /**
     * @brief Get the currently pressed key
     * @return Character of pressed key, or '\0' if no key pressed
     */
    char getKey();
    
private:
    char (*_keys)[4];           // Pointer to key mapping array
    DigitalOut** _rowPins;      // Array of row output pins
    DigitalIn** _colPins;       // Array of column input pins
    int _rows;                  // Number of rows
    int _cols;                  // Number of columns
    char _lastKey;              // Last detected key
    Timer _debounceTimer;       // Timer for debouncing
    
    // Use DEBOUNCE_TIME_MS from config.h
    static const int DEBOUNCE_TIME_MS = ::DEBOUNCE_TIME_MS;
    
    /**
     * @brief Scan the keypad matrix
     * @return Character of pressed key, or '\0' if none
     */
    char scanKeys();
};

#endif // KEYPAD_H
