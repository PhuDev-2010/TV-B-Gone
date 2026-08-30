📡 TV-B-Gone ESP32

Open Source Universal TV Killer

---

🚀 Overview

TV-B-Gone is a pocket-sized device powered by ESP32 that can turn off almost any TV within a range of 15–20 meters with a single button press.

---

🔥 Key Features

Feature Description
✅ Dual-region code support Sends both North America (NA) and Europe (EU) power-off codes simultaneously
✅ Massive code database Covers thousands of TV brands and models worldwide
✅ Interrupt-driven control Press button to start, press again to stop instantly
✅ Auto-reset Returns to ready state after each transmission cycle
✅ Cooldown management Prevents IR LED overheating during long code bursts
✅ Fully open source Easy to customize and expand

---

🧩 Hardware Wiring

Component ESP32 Pin Description
IR LED GPIO21 IR signal output
IR LED (Cathode -) GND Connect to ESP32 GND
Transistor Emitter (E) GND Connect directly to ESP32 GND
Transistor Collector (C) IR LED (Anode +) Connect to IR LED positive leg
Transistor Base (B) 1kΩ → GPIO21 Current limiting resistor
Power 5V Supply voltage for IR LED (via transistor)

---

🔌 Schematic Diagram

ESP32 (Standard)

```
                    +5V
                     │
                     ▼
                ┌─────────┐
                │  IR LED │
                │  (940nm)│
                └─────────┘
                     │
                     ▼
                Collector (C)
                ┌─────────┐
                │ 2N2222  │
                └─────────┘
             Base│     │Emitter (E)
                │     └────────── GND
               1kΩ              │
                │               │
         ┌──────┴──────┐        │
         │  GPIO21     │        │
         └─── ESP32 ───┴────────┘
                 GND
```

---

ESP32-C3 Super Mini

```
                    +5V
                     │
                     ▼
                ┌─────────┐
                │  IR LED │
                │  (940nm)│
                └─────────┘
                     │
                     ▼
                Collector (C)
                ┌─────────┐
                │ 2N2222  │
                └─────────┘
             Base│     │Emitter (E)
                │     └────────── GND
               1kΩ              │
                │               │
         ┌──────┴──────┐        │
         │  GPIO21     │        │
         └─── ESP32 ───┴────────┘
            C3 Super Mini
                 GND
```

---

📷 Wiring Diagram

![anh](https://drive.google.com/uc?export=view&id=1TZkpaQdGpWFP6re85XvqutlQdzK-9ihB)

Sơ đồ đấu nối ESP32 với IR LED và transistor 2N2222

---

📦 Component List

Component Quantity Notes
ESP32 Dev Board 1 Standard ESP32 or ESP32-C3 Super Mini
IR LED (5mm, 940nm) 1 High-power recommended
NPN Transistor 1 2N2222, BC547, S8050, or equivalent
Resistor 1kΩ 1 Base current limiting
Power Supply 1 5V DC (USB or battery)

---

📊 How It Works

Step Action Description
1 Idle mode ESP32 waits for button press
2 Press once Begins transmitting TV power-off codes
3 NA codes Sends all North America region codes first
4 EU codes Then sends all Europe region codes
5 Progress feedback LED blinks, serial monitor shows progress
6 Press again Immediately stops and resets to start
7 Auto-complete Returns to idle after all codes sent

---

⚙️ Key Parameters

Parameter Value Description
IR LED Pin GPIO21 Signal output
Transmission Range 15–20m Depends on IR LED power
Code Regions 2 NA + EU
Cooldown Interval Every 60 codes 900ms pause to prevent overheating
Repeat Per Code 1 Prevents toggle state cancellation
Debounce Time 200ms Button press debounce

---

🔧 Customization

Modify main.h to adjust behavior:

```cpp
#define COOLDOWN_EVERY_N_CODES  60     // Cooldown frequency
#define COOLDOWN_MS             900    // Cooldown duration (ms)
#define GAP_BETWEEN_CODES_US    130000 // Gap between codes (μs)
#define BUTTON_DEBOUNCE_MS      200    // Button debounce (ms)
```

---

📱 Serial Monitor Output

```
[INIT] ESP32-TV-B-Gone (NA+EU dual-region, interrupt control)
[INIT] IR LED pin = GPIO21
[INIT] Total NA codes: XXX
[INIT] Total EU codes: XXX
[INIT] Ready. Press BOOT button to start.
[TX] START sending XX NA + XX EU codes
[TX][NA] Code 10/XX | 15% | freq=38kHz
[TX] Cooldown 900ms...
[TX] COMPLETE. Ready for next press.
```

---

🛠️ Troubleshooting

Issue Solution
TV doesn't turn off Ensure IR LED is pointed directly at TV
Short range Check transistor wiring, use 5V supply
No serial output Verify USB cable, baud rate (115200)
IR LED not glowing Check GND connections (Emitter → GND, Cathode → GND)

---

📜 License

Open Source – Free to use, modify, and distribute.
Original concept inspired by the classic TV-B-Gone project by Mitch Altman.

---

📞 Contact & Support

PhuDev
📱 Zalo: 0816262451
📘 Facebook: lmphudev
✈️ Telegram: @phudev

---

⭐ Acknowledgments

· Mitch Altman – original TV-B-Gone concept
· IRremoteESP8266 library team
· Open source community for code contributions
