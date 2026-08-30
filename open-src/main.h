/*
====================================================================
 main.h - ESP32-TV-B-Gone Configuration Header
====================================================================
 Project: TV-B-Gone ESP32
 Author: PhuDev
 Contact: 
   - Zalo: 0816262451
   - Facebook: lmphudev
   - Telegram: @phudev
 Version: 1.0
 Date: 2026
 Description: 
   Pin definitions and configuration parameters for TV-B-Gone
====================================================================
*/

#ifndef MAIN_H
#define MAIN_H

// ====== PIN DEFINITIONS ======
#define LED           2   // Onboard status LED (GPIO2)
#define IRLED         21  // IR LED output pin (GPIO21) - requires transistor driver
#define TRIGGER       0   // BOOT button (GPIO0) - built-in pull-up resistor

// ====== LED BLINK PATTERNS ======
#define BLINK_ON   2   // Ready/Starting transmission - blink 2 times
#define BLINK_OFF  4   // Stopped/Completed transmission - blink 4 times

// ====== TRANSMISSION PARAMETERS ======
// Repeat count per code (MUST be 1 for toggle codes)
// WARNING: Most TV codes are TOGGLE type (same code for ON/OFF)
// Setting >1 will turn TV ON then OFF immediately = self-canceling
#define REPEAT_PER_CODE 1

// Gap between codes (microseconds)
// Lower value = faster transmission, but TV needs sensitive IR receiver
#define GAP_BETWEEN_CODES_US 130000UL

// Thermal cooldown settings to prevent IR LED overheating
#define COOLDOWN_EVERY_N_CODES 60    // Cooldown after every N codes
#define COOLDOWN_MS 900             // Cooldown duration (milliseconds)

// ====== BUTTON CONFIGURATION ======
#define BUTTON_DEBOUNCE_MS 200       // Button debounce time (milliseconds)

// ====== UTILITY MACROS ======
#define NUM_ELEM(x) (sizeof (x) / sizeof (*(x)))  // Calculate array size

// ====== DEBUG SETTINGS ======
#define DEBUG 1                      // Enable debug output (1=on, 0=off)
#define DEBUGP(x) if (DEBUG == 1) { x ; }

// ====== COMPATIBILITY DEFINES ======
#define NOP __asm__ __volatile__ ("nop")
#define DELAY_CNT 25
#define freq_to_timerval(x) (x / 1000)

// ====== IR CODE STRUCTURE ======
// Compressed IR code format for memory efficiency
struct IrCode {
  uint8_t timer_val;           // Frequency timer value (kHz)
  uint8_t numpairs;            // Number of on/off pulse pairs
  uint8_t bitcompression;      // Compression bits per pair
  uint16_t const *times;       // Timing table pointer
  uint8_t const *codes;        // Compressed code data pointer
};

#endif // MAIN_H