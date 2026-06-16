# FerroWave DIY Build Guide
## Required Libraries & Installation

### Hardware Requirements
- **ESP32-A1S Audio Kit v2.2** (with ES8388 codec)
- **5V Electromagnet** (controlled via MOSFET)
- **WS2812B LED Ring** (24 LEDs)
- **MOSFET** (e.g., IRLZ44N or similar logic-level N-channel)
- **330Î© Resistor** (for LED data line)
- **Ferrofluid** in display container
- Power supply (5V for LEDs and magnet)

---

## Arduino IDE Setup

### 1. ESP32 Board Support
Install ESP32 board support in Arduino IDE:

**Via Board Manager:**
1. Open Arduino IDE
2. Go to `File` â `Preferences`
3. Add to "Additional Board URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Go to `Tools` â `Board` â `Boards Manager`
5. Search for "ESP32" and install **"esp32" by Espressif Systems**
6. Select Board: `Tools` â `Board` â `ESP32 Arduino` â `ESP32 Dev Module`

---

## Required Libraries

### 1. **AudioTools** (by Phil Schatzmann)
**Primary audio processing library**

**Installation:**
- Arduino IDE: `Sketch` â `Include Library` â `Manage Libraries`
- Search: "AudioTools"
- Install: **"AudioTools" by pschatzmann**
- Version: Latest (tested with v0.9.x)

**GitHub:** https://github.com/pschatzmann/arduino-audio-tools

**What it does:**
- Handles I2S audio streams
- Provides AudioBoardStream for ES8388 codec
- Ring buffer for audio processing
- Multi-output routing (Bluetooth + processing)

---

### 2. **ESP32-A2DP** (by Phil Schatzmann)
**Bluetooth A2DP audio sink**

**Installation:**
- Arduino IDE: `Sketch` â `Include Library` â `Manage Libraries`
- Search: "ESP32-A2DP"
- Install: **"ESP32-A2DP" by pschatzmann**
- Version: Latest (tested with v1.7.x)

**GitHub:** https://github.com/pschatzmann/ESP32-A2DP

**What it does:**
- Enables Bluetooth audio receiver functionality
- A2DP profile support (wireless audio streaming)
- Integrates with AudioTools for audio routing

---

### 3. **Adafruit NeoPixel**
**WS2812B LED control**

**Installation:**
- Arduino IDE: `Sketch` â `Include Library` â `Manage Libraries`
- Search: "Adafruit NeoPixel"
- Install: **"Adafruit NeoPixel" by Adafruit**
- Version: Latest (tested with v1.11.x)

**GitHub:** https://github.com/adafruit/Adafruit_NeoPixel

**What it does:**
- Controls WS2812B addressable RGB LEDs
- Handles color calculations and LED timing
- Provides easy color and brightness control

---

## Complete Library Installation Summary

### Option 1: Arduino Library Manager (Recommended)
```
1. Sketch â Include Library â Manage Libraries
2. Search and install each:
   - "AudioTools" by pschatzmann
   - "ESP32-A2DP" by pschatzmann
   - "Adafruit NeoPixel" by Adafruit
```

### Option 2: Manual Installation (Advanced)
Clone repositories into your Arduino `libraries` folder:

```bash
cd ~/Documents/Arduino/libraries/

# AudioTools
git clone https://github.com/pschatzmann/arduino-audio-tools.git

# ESP32-A2DP
git clone https://github.com/pschatzmann/ESP32-A2DP.git

# Adafruit NeoPixel
git clone https://github.com/adafruit/Adafruit_NeoPixel.git
```

Then restart Arduino IDE.

---

## Wiring Diagram

### ESP32-A1S Connections

**Electromagnet (via MOSFET):**
- MOSFET Gate â GPIO 22
- MOSFET Drain â Electromagnet (-)
- MOSFET Source â GND
- Electromagnet (+) â 5V Power Supply

**LED Ring (WS2812B):**
- LED DIN â GPIO 21 (through 330Î© resistor)
- LED VCC â 5V
- LED GND â GND

**Onboard Buttons (ESP32-A1S):**
- Button 1: GPIO 36 (Magnet Mode â)
- Button 2: GPIO 39 (Magnet Mode â)
- Button 3: GPIO 34 (LED Mode â)
- Button 4: GPIO 35 (LED Mode â)
- Button 5: GPIO 32 (EQ Preset â)
- Button 6: GPIO 33 (EQ Preset â)

**I2S Audio (Pre-wired on ESP32-A1S):**
- BCK: GPIO 27
- WS: GPIO 25
- DATA OUT: GPIO 26
- DATA IN (AUX): GPIO 35
- MCLK: GPIO 0

---

## Upload Settings

**Board Settings in Arduino IDE:**
```
Board: "ESP32 Dev Module"
Upload Speed: 921600
CPU Frequency: 240MHz
Flash Frequency: 80MHz
Flash Mode: QIO
Flash Size: 4MB (32Mb)
Partition Scheme: "Default 4MB with spiffs"
Core Debug Level: "None"
PSRAM: "Disabled"
```

**Port:** Select your ESP32's COM/USB port

---

## Compilation Notes

### Expected Memory Usage
- Sketch uses approximately **~65-75%** of program storage
- Global variables use approximately **~25-30%** of dynamic memory
- Uses ESP32 dual-core capabilities for audio processing

### Common Compilation Issues

**1. "AudioSource is ambiguous"**
- Fixed in code (renamed enum to `InputSource`)
- Caused by naming conflict with AudioTools library

**2. Missing library errors**
- Ensure all three libraries are installed
- Restart Arduino IDE after installation

**3. "Bluetooth: Cannot allocate memory"**
- Set partition scheme to "Default 4MB with spiffs"
- Ensure no other large libraries are included

---

## First Boot Checklist

1. **Serial Monitor** (115200 baud)
   - Should show startup banner
   - Lists all button controls
   - Shows current settings

2. **LED Ring Test**
   - Blue startup animation on power-up
   - Should respond to audio input

3. **Bluetooth Pairing**
   - Device name: "FerroWave_AUX"
   - Should appear in Bluetooth device list
   - No pairing code required

4. **Test Commands**
   - Type `?` in Serial Monitor to see settings
   - Type `t` to test electromagnet pulse
   - Type `m` to list magnet modes

---

## Troubleshooting

### No Audio Output
- Check I2S wiring (BCK, WS, DATA pins)
- Verify AudioBoardStream configuration
- Ensure ES8388 codec is properly initialized

### LEDs Not Working
- Verify LED power (5V, GND)
- Check data pin (GPIO 21) connection
- Ensure 330Î© resistor is in place
- Try reducing LED brightness: `l50` in serial

### Electromagnet Not Responding
- Check MOSFET wiring
- Verify GPIO 22 connection
- Test with `t` command in serial monitor
- Check power supply capacity

### Bluetooth Won't Connect
- Restart ESP32
- Ensure no other device is connected
- Check if Bluetooth is enabled on source device
- Try forgetting device and re-pairing

---

## Power Requirements

- **ESP32-A1S Board:** ~500mA (via USB or 5V)
- **LED Ring (24 LEDs):** Up to 1.4A at full brightness white
- **Electromagnet:** 500mA - 2A depending on coil

**Recommended:** 5V 3A+ power supply for stable operation

---

## Additional Resources

- **AudioTools Documentation:** https://github.com/pschatzmann/arduino-audio-tools/wiki
- **ESP32-A2DP Examples:** https://github.com/pschatzmann/ESP32-A2DP/tree/main/examples
- **NeoPixel Guide:** https://learn.adafruit.com/adafruit-neopixel-uberguide

---

## License & Credits

- **FerroWave:** DIY ferrofluid visualizer project
- **Libraries:** See individual library licenses
  - AudioTools: GPL-3.0
  - ESP32-A2DP: Apache-2.0
  - Adafruit NeoPixel: LGPL-3.0

---

**Version:** 1.0 (AUX + Button Control Edition)  
**Last Updated:** December 2024
