# Hardware Setup Guide

## Quick Start Wiring Guide

### Materials Checklist
- STM32 Nucleo board (L152RE or L476RG)
- 4x4 Matrix Keypad
- 16x2 I2C LCD Display
- 5V Relay Module
- 12V Electromagnetic Lock (optional)
- Jumper wires (20+ pieces)
- Breadboard
- 12V DC Power Supply (for lock)
- USB Cable (for Nucleo programming)

---

## Step-by-Step Wiring

### 1. Keypad Connection

The keypad has 8 pins (4 rows + 4 columns):

```
Keypad Pin Layout (from left to right):
[R1] [R2] [R3] [R4] [C1] [C2] [C3] [C4]
```

**Connections:**
```
Keypad → Nucleo
─────────────────
R1     → PA_0
R2     → PA_1
R3     → PA_4
R4     → PA_5
C1     → PB_0
C2     → PB_1
C3     → PB_3
C4     → PB_4
```

**Key Layout:**
```
┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ A │
├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ B │
├───┼───┼───┼───┤
│ 7 │ 8 │ 9 │ C │
├───┼───┼───┼───┤
│ * │ 0 │ # │ D │
└───┴───┴───┴───┘
```

---

### 2. LCD Display (I2C)

The I2C LCD has 4 pins:

```
LCD Pin → Nucleo
────────────────
VCC     → 5V
GND     → GND
SDA     → PB_7
SCL     → PB_6
```

**I2C Address:** 0x27 (default)
- If your LCD doesn't work, try address 0x3F
- To change, edit `main.cpp`: `TextLCD_I2C lcd(&i2c, 0x3F, ...)`

---

### 3. Relay Module

**For Electromagnetic Lock Control:**

```
Relay Module → Connections
───────────────────────────
VCC          → 5V (Nucleo)
GND          → GND (Nucleo)
IN           → PA_8 (Nucleo)
COM          → 12V+ (Power Supply)
NO           → Lock+ (Electromagnetic Lock)
Lock-        → 12V- (Power Supply GND)
```

**Relay States:**
- `PA_8 = HIGH (1)` → Relay ON → Lock OPEN
- `PA_8 = LOW (0)` → Relay OFF → Lock CLOSED

**Important:**
- Use a separate 12V power supply for the lock
- **DO NOT** power the lock from Nucleo 5V (insufficient current)
- Common GND between Nucleo and 12V supply

---

### 4. LED Indicator

**Built-in LED on Nucleo:**
- Pin: `PC_13`
- No external wiring needed
- LED is on the board (usually green)

**Optional External LED:**
```
LED Anode (+) → Resistor (220Ω) → PA_8
LED Cathode (-) → GND
```

---

## Complete Wiring Diagram

```
                    STM32 Nucleo L476RG
                    ┌─────────────────────┐
                    │                     │
    Keypad R1 ──────┤ PA_0                │
    Keypad R2 ──────┤ PA_1                │
    Keypad R3 ──────┤ PA_4                │
    Keypad R4 ──────┤ PA_5                │
                    │                     │
    Keypad C1 ──────┤ PB_0                │
    Keypad C2 ──────┤ PB_1                │
    Keypad C3 ──────┤ PB_3                │
    Keypad C4 ──────┤ PB_4                │
                    │                     │
    LCD SDA ────────┤ PB_7                │
    LCD SCL ────────┤ PB_6                │
                    │                     │
    Relay IN ───────┤ PA_8                │
                    │                     │
    LCD VCC ────────┤ 5V                  │
    Relay VCC ──────┤ 5V                  │
                    │                     │
    LCD GND ────────┤ GND ────────────────┼──── 12V GND
    Relay GND ──────┤ GND                 │
                    │                     │
                    └─────────────────────┘
                              │
                              │ USB Cable
                              ▼
                          Computer
                          
    12V Power Supply
    ┌──────────────┐
    │   12V DC     │
    │   Adapter    │
    └──┬────────┬──┘
       │        │
      (+)      (-)
       │        │
       │        └──────────────────────── GND (Common)
       │
       └── Relay COM
           
    Electromagnetic Lock
    ┌──────────────┐
    │   12V Lock   │
    └──┬────────┬──┘
      (+)      (-)
       │        │
       │        └──────────────────────── 12V GND
       │
       └── Relay NO (Normally Open)
```

---

## Alternative: Servo Motor Setup

If using a **servo motor** instead of relay:

```cpp
// In main.cpp, change:
#define USE_RELAY false
```

**Servo Wiring:**
```
Servo  → Nucleo
──────────────
Signal → PA_8
VCC    → 5V
GND    → GND
```

**Servo Angles:**
- 0° (1ms pulse) = Door CLOSED
- 90° (2ms pulse) = Door OPEN

---

## Testing Individual Components

### Test 1: LCD Display
```cpp
#include "mbed.h"
#include "TextLCD.h"

I2C i2c(PB_7, PB_6);
TextLCD_I2C lcd(&i2c, 0x27, TextLCD::LCD16x2);

int main() {
    lcd.cls();
    lcd.printf("LCD Test OK!");
    while(1);
}
```

### Test 2: Keypad
```cpp
#include "mbed.h"
#include "Keypad.h"

PinName rowPins[4] = {PA_0, PA_1, PA_4, PA_5};
PinName colPins[4] = {PB_0, PB_1, PB_3, PB_4};
char keys[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
Keypad keypad(keys, rowPins, colPins, 4, 4);

int main() {
    printf("Press keys...\n");
    while(1) {
        char key = keypad.getKey();
        if (key) printf("Key: %c\n", key);
        ThisThread::sleep_for(50);
    }
}
```

### Test 3: Relay
```cpp
#include "mbed.h"

DigitalOut relay(PA_8);

int main() {
    while(1) {
        relay = 1;  // ON
        ThisThread::sleep_for(2000);
        relay = 0;  // OFF
        ThisThread::sleep_for(2000);
    }
}
```

### Test 4: LED
```cpp
#include "mbed.h"

DigitalOut led(PC_13);

int main() {
    while(1) {
        led = !led;  // Toggle
        ThisThread::sleep_for(500);
    }
}
```

---

## Common Issues

### LCD Shows Garbage Characters
- **Cause:** Wrong I2C address
- **Fix:** Try 0x3F instead of 0x27
- **Test:** Run I2C scanner to find address

### Keypad Not Responding
- **Cause:** Wrong pin connections
- **Fix:** Double-check row/column pins
- **Test:** Use multimeter to verify continuity

### Relay Clicks But Lock Doesn't Open
- **Cause:** Insufficient power
- **Fix:** Use separate 12V power supply
- **Check:** Lock voltage rating (usually 12V DC)

### LED Not Flashing
- **Cause:** Wrong pin or code issue
- **Fix:** Verify PC_13 is correct for your board
- **Test:** Use external LED on different pin

---

## Safety Warnings

**IMPORTANT:**
1. **Never** connect 12V directly to Nucleo pins
2. **Always** use a relay for high-voltage loads
3. **Common GND** between all power supplies
4. **Disconnect power** before wiring changes
5. **Check polarity** on electromagnetic locks

---

## Power Consumption

| Component            | Voltage | Current | Power  |
|----------------------|---------|---------|--------|
| Nucleo Board         | 5V      | ~100mA  | 0.5W   |
| LCD Display          | 5V      | ~20mA   | 0.1W   |
| Keypad               | 5V      | ~1mA    | 0.005W |
| Relay Module         | 5V      | ~70mA   | 0.35W  |
| Electromagnetic Lock | 12V     | ~500mA  | 6W     |
| **Total**            | -       | -       | **~7W**|

**Recommended Power Supply:**
- Nucleo: USB (5V, 500mA)
- Lock: 12V DC adapter (1A minimum)

---

## Final Checklist

Before powering on:
- All connections double-checked
- No short circuits
- Correct polarity on all components
- Separate power for lock
- Common GND connected
- Code uploaded to Nucleo
- USB cable connected

**Ready to test!**
