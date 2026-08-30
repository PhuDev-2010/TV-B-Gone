/*
====================================================================
 ESP32-TV-B-Gone - Open Source Universal TV Killer
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
   Dual-region TV power-off code transmitter (NA + EU)
   Interrupt-driven control with instant stop capability
====================================================================
*/

// ====== LIBRARIES ======
#include "WORLD_IR_CODES.h"          // IR code database for TVs worldwide
#include <IRremoteESP8266.h>         // IR transmission library (ESP32 compatible)
#include <IRsend.h>                  // IRsend class for IR signal transmission

// ====== FUNCTION DECLARATIONS ======
void blinkLED(uint8_t times, uint16_t onMs, uint16_t offMs);  // Status LED blink function
uint8_t read_bits(uint8_t count);                            // Read compressed data bits
bool sendOneRegion(const IrCode* const* codeList, uint8_t count, const char* regionName,
                    uint16_t startIndex, uint16_t totalCodesOverall, uint16_t &codesSinceCooldown);
void IRAM_ATTR triggerISR();                                 // Button interrupt handler

// ====== GLOBAL VARIABLES ======
uint16_t rawData[300];                // Raw IR data buffer for transmission
IRsend irsend(IRLED);                // IRsend object on GPIO21 pin

// IR code arrays from WORLD_IR_CODES.h
extern const IrCode* const NApowerCodes[];   // North America region codes
extern const IrCode* const EUpowerCodes[];   // Europe region codes
extern uint8_t num_NAcodes, num_EUcodes;     // Code count for each region

// Compressed data reading variables
uint8_t bitsleft_r = 0;               // Remaining bits in current byte
uint8_t bits_r = 0;                  // Current byte being read
uint8_t code_ptr;                    // Pointer position in code array
volatile const IrCode * powerCode;   // Pointer to current code being processed

uint16_t ontime, offtime;            // On/off timing for each pulse

// ====== CONTROL VARIABLES ======
volatile bool buttonEvent = false;               // Button press event flag
volatile unsigned long lastInterruptMs = 0;      // Last interrupt timestamp (debounce)
volatile bool isSending = false;                 // Transmission status flag
volatile bool stopRequested = false;             // Stop request flag

// ====== INTERRUPT SERVICE ROUTINE ======
// Handles button press with debounce
// If sending: request stop
// If idle: trigger transmission start
void IRAM_ATTR triggerISR() {
  unsigned long now = millis();
  if (now - lastInterruptMs > BUTTON_DEBOUNCE_MS) {
    if (isSending) {
      stopRequested = true;          // Stop ongoing transmission
    } else {
      buttonEvent = true;            // Start new transmission
    }
    lastInterruptMs = now;
  }
}

// ====== COMPRESSED DATA READER ======
// Reads 'count' bits from the compressed IrCode structure
uint8_t read_bits(uint8_t count) {
  uint8_t tmp = 0;
  for (uint8_t k = 0; k < count; k++) {
    if (bitsleft_r == 0) {
      bits_r = powerCode->codes[code_ptr++];
      bitsleft_r = 8;
    }
    bitsleft_r--;
    tmp |= (((bits_r >> (bitsleft_r)) & 1) << (count - 1 - k));
  }
  return tmp;
}

// ====== SETUP ======
void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("========================================");
  Serial.println("[INIT] ESP32-TV-B-Gone (NA+EU dual-region, interrupt control)");
  Serial.printf("[INIT] Author: PhuDev\n");
  Serial.printf("[INIT] Contact: Zalo 0816262451 | FB: lmphudev | Telegram: @phudev\n");
  Serial.printf("[INIT] IR LED pin   = GPIO %d\n", IRLED);
  Serial.printf("[INIT] LED pin      = GPIO %d\n", LED);
  Serial.printf("[INIT] Button pin   = GPIO %d (BOOT button)\n", TRIGGER);

  irsend.begin();

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(TRIGGER, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(TRIGGER), triggerISR, FALLING);

  delayMicroseconds(50000);

  Serial.printf("[INIT] NA codes loaded: %d\n", num_NAcodes);
  Serial.printf("[INIT] EU codes loaded: %d\n", num_EUcodes);
  Serial.printf("[INIT] Total codes per press: %d\n", num_NAcodes + num_EUcodes);
  Serial.println("[INIT] Ready. Press BOOT button to start transmission.");
  Serial.println("[INIT] Press again during transmission to STOP instantly.");
  Serial.println("========================================");

  buttonEvent = false;
  isSending = false;
  stopRequested = false;

  blinkLED(BLINK_ON, 100, 150);
}

// ====== TRANSMIT ONE REGION ======
// Sends all codes from a single region (NA or EU)
// Returns true if stopped early, false if completed all codes
bool sendOneRegion(const IrCode* const* codeList, uint8_t count, const char* regionName,
                    uint16_t startIndex, uint16_t totalCodesOverall, uint16_t &codesSinceCooldown) {
  for (uint8_t i = 0; i < count; i++) {
    // Check for stop request before each code
    if (stopRequested) {
      stopRequested = false;
      return true; // Stopped early
    }
    
    powerCode = codeList[i];

    const uint8_t freq = powerCode->timer_val;
    const uint8_t numpairs = powerCode->numpairs;
    const uint8_t bitcompression = powerCode->bitcompression;

    // Calculate progress percentage
    uint16_t overallIndex = startIndex + i + 1;
    uint8_t percent = (uint8_t)((uint32_t)overallIndex * 100 / totalCodesOverall);

    Serial.printf("[TX][%s] Code %3d/%d | %3d%% | freq=%2d kHz | pairs=%2d\n",
                  regionName, i + 1, count, percent, freq, numpairs);

    // Reconstruct raw IR data from compressed format
    code_ptr = 0;
    for (uint8_t k = 0; k < numpairs; k++) {
      uint16_t ti = read_bits(bitcompression) * 2;
      ontime  = powerCode->times[ti];
      offtime = powerCode->times[ti + 1];
      rawData[k * 2]       = ontime * 10;
      rawData[(k * 2) + 1] = offtime * 10;
    }

    // Send the IR code (repeat per REPEAT_PER_CODE times)
    for (uint8_t r = 0; r < REPEAT_PER_CODE; r++) {
      irsend.sendRaw(rawData, (numpairs * 2), freq);
    }

    bitsleft_r = 0;

    blinkLED(1, 25, 0);  // Quick blink for visual feedback

    delayMicroseconds(GAP_BETWEEN_CODES_US);

    // Thermal cooldown to prevent overheating
    codesSinceCooldown++;
    if (codesSinceCooldown >= COOLDOWN_EVERY_N_CODES) {
      codesSinceCooldown = 0;
      Serial.printf("[TX] Cooldown %dms...\n", COOLDOWN_MS);
      delay(COOLDOWN_MS);
      
      // Check for stop request during cooldown
      if (stopRequested) {
        stopRequested = false;
        return true;
      }
    }
  }
  return false; // All codes sent
}

// ====== SEND ALL CODES ======
// Master function to send all NA + EU codes
void sendAllCodes() {
  uint16_t totalCodes = num_NAcodes + num_EUcodes;
  unsigned long tStart = millis();
  uint16_t codesSinceCooldown = 0;

  // Start transmission
  isSending = true;
  stopRequested = false;

  Serial.println();
  Serial.printf("[TX] STARTING: %d NA + %d EU = %d codes\n",
                num_NAcodes, num_EUcodes, totalCodes);

  // Send NA region first
  bool stoppedEarly = sendOneRegion(NApowerCodes, num_NAcodes, "NA", 0, totalCodes, codesSinceCooldown);

  // Send EU region if not stopped
  if (!stoppedEarly && !stopRequested) {
    stoppedEarly = sendOneRegion(EUpowerCodes, num_EUcodes, "EU", num_NAcodes, totalCodes, codesSinceCooldown);
  }

  // Final check for stop request
  if (stopRequested) {
    stoppedEarly = true;
    stopRequested = false;
  }

  unsigned long elapsedMs = millis() - tStart;

  // End transmission
  isSending = false;

  if (stoppedEarly) {
    Serial.printf("[TX] STOPPED EARLY after %lu ms. Ready for next press.\n", elapsedMs);
  } else {
    Serial.printf("[TX] COMPLETED %d codes in %lu ms (~%.1f seconds).\n",
                  totalCodes, elapsedMs, elapsedMs / 1000.0);
  }

  delay(400);
  blinkLED(BLINK_OFF, 100, 150);

  Serial.println("[TX] DONE - Ready for next button press.");
  Serial.println("========================================");

  // Clear button event to ignore any events during LED blinking
  buttonEvent = false;
}

// ====== MAIN LOOP ======
void loop() {
  // If idle and button pressed -> start transmission
  if (!isSending && buttonEvent) {
    buttonEvent = false;
    Serial.println("[BTN] Button pressed -> STARTING from beginning.");
    sendAllCodes();
  }
  
  // If sending and stop requested (handled in sendOneRegion)
  // Redundant safety check
  if (isSending && stopRequested) {
    // Stop handling is already done in sendOneRegion()
    // This is just a safety net
  }
  
  yield();  // Allow background tasks
}

// ====== LED FUNCTIONS ======
// Blinks the status LED with specified pattern
void blinkLED(uint8_t times, uint16_t onMs, uint16_t offMs) {
  for (uint8_t n = 0; n < times; n++) {
    digitalWrite(LED, LOW);
    delay(onMs);
    digitalWrite(LED, HIGH);
    if (n < times - 1 && offMs > 0) {
      delay(offMs);
    }
  }
}