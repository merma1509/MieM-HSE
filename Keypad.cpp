/**
 * @file Keypad.cpp
 * @brief Implementation of 4x4 Matrix Keypad Library
 */

#include "Keypad.h"

Keypad::Keypad(char keys[][4], PinName* rowPins, PinName* colPins, int rows, int cols) 
    : _keys(keys), _rows(rows), _cols(cols), _lastKey('\0') {
    
    // Allocate memory for row pins (outputs)
    _rowPins = new DigitalOut*[_rows];
    for (int i = 0; i < _rows; i++) {
        _rowPins[i] = new DigitalOut(rowPins[i]);
        _rowPins[i]->write(1);  // Set all rows HIGH initially
    }
    
    // Allocate memory for column pins (inputs with pull-up)
    _colPins = new DigitalIn*[_cols];
    for (int i = 0; i < _cols; i++) {
        _colPins[i] = new DigitalIn(colPins[i]);
        _colPins[i]->mode(PullUp);  // Enable internal pull-up resistors
    }
    
    // Start debounce timer
    _debounceTimer.start();
}

Keypad::~Keypad() {
    // Clean up dynamically allocated memory
    for (int i = 0; i < _rows; i++) {
        delete _rowPins[i];
    }
    delete[] _rowPins;
    
    for (int i = 0; i < _cols; i++) {
        delete _colPins[i];
    }
    delete[] _colPins;
}

char Keypad::scanKeys() {
    // Scan each row
    for (int row = 0; row < _rows; row++) {
        // Set current row LOW, others HIGH
        for (int i = 0; i < _rows; i++) {
            _rowPins[i]->write(i == row ? 0 : 1);
        }
        
        // Small delay for signal stabilization
        wait_us(10);
        
        // Check each column
        for (int col = 0; col < _cols; col++) {
            // If column is LOW, key is pressed (pull-up makes it HIGH when not pressed)
            if (_colPins[col]->read() == 0) {
                // Set all rows back to HIGH
                for (int i = 0; i < _rows; i++) {
                    _rowPins[i]->write(1);
                }
                return _keys[row][col];
            }
        }
    }
    
    // Set all rows back to HIGH
    for (int i = 0; i < _rows; i++) {
        _rowPins[i]->write(1);
    }
    
    return '\0';  // No key pressed
}

char Keypad::getKey() {
    char key = scanKeys();
    
    // Debouncing logic
    if (key != '\0') {
        // Key is pressed
        if (key != _lastKey) {
            // New key detected
            _lastKey = key;
            _debounceTimer.reset();
            return key;
        } else {
            // Same key still pressed - check debounce time
            if (_debounceTimer.read_ms() > DEBOUNCE_TIME_MS) {
                return '\0';  // Ignore held key after debounce period
            }
        }
    } else {
        // No key pressed - reset last key
        _lastKey = '\0';
    }
    
    return '\0';
}
