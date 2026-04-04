# FerroWave Troubleshooting Guide

Complete troubleshooting guide for resolving common issues with FerroWave.

---

## Quick Diagnostic Checklist

Before diving into specific issues, run through this quick checklist:

- [ ] Battery pack wake button pressed
- [ ] Breadboard master switch is ON
- [ ] Batteries are charged (check voltage)
- [ ] All connections are secure
- [ ] No visible shorts or damaged wires
- [ ] Serial Monitor shows output at 115200 baud
- [ ] Arduino IDE shows successful upload
- [ ] All three libraries are installed

---

## Table of Contents

- [Power Issues](#power-issues)
- [Compilation Errors](#compilation-errors)
- [Upload Issues](#upload-issues)
- [Audio Problems](#audio-problems)
- [Electromagnet Issues](#electromagnet-issues)
- [LED Problems](#led-problems)
- [Button Issues](#button-issues)
- [Performance Issues](#performance-issues)
- [Battery Problems](#battery-problems)

---

## Power Issues

### System Won't Power On

**Symptoms:**
- No LEDs light up
- ESP32 is completely dead
- No Serial Monitor output

**Solutions:**

1. **Check Battery Charge**
   ```
   - Press battery wake button
   - Check battery indicator LEDs
   - Charge if needed (via Micro-USB port)
   - Try with fresh batteries
   ```

2. **Verify Breadboard Master Switch**
   ```
   - Is switch in ON position?
   - Check power LED on breadboard module
   - Try toggling switch off/on
   ```

3. **Check USB Cable Connection**
   ```
   - Verify USB cable from battery to breadboard
   - Try different USB cable
   - Check for loose connections
   ```

4. **Check Breadboard Power Module**
   ```
   - Measure voltage at (+) rail (should be 5V)
   - Verify input connections
   - Check module LEDs
   ```

---

### Random Resets or Brownouts

**Symptoms:**
- ESP32 restarts unexpectedly
- LEDs dim when coil activates
- Serial Monitor shows boot messages repeatedly

**Solutions:**

1. **Insufficient Current**
   ```
   Problem: Total current draw exceeds battery capacity
   
   Fix:
   - Reduce LED brightness: l80
   - Lower max duty: d70
   - Use less aggressive magnet mode
   - Check battery capacity (use higher capacity cells)
   ```

2. **Poor Connections**
   ```
   Problem: Loose wires cause voltage drops
   
   Fix:
   - Check USB cable connection
   - Verify breadboard power connections
   - Ensure all wires are secure
   - Check for oxidized contacts
   ```

3. **Battery Depletion**
   ```
   Problem: Batteries running low
   
   Fix:
   - Charge batteries via Micro-USB
   - Monitor battery indicator
   - Replace with fresh batteries
   ```

---

### Voltage Too Low/High

**Symptoms:**
- Measured voltage at ESP32 is not 5V
- Components behave erratically

**Solutions:**

**If voltage is low (<4.5V):**
- Battery is depleted - charge or replace
- Power module failing - test or replace
- Too much current draw - reduce load
- USB cable resistance - use shorter/thicker cable

**If voltage is high (>5.5V):**
- Power module misconfigured - check settings
- Replace power module (may be faulty)
- Don't exceed 5.5V - may damage ESP32

---

## Compilation Errors

### "AudioSource is ambiguous"

**Error Message:**
```
error: reference to 'AudioSource' is ambiguous
```

**Solution:**
This is fixed in the latest firmware. Update to the current version from GitHub.

---

### "Library not found"

**Error Messages:**
```
fatal error: AudioTools.h: No such file or directory
fatal error: BluetoothA2DPSink.h: No such file or directory  
fatal error: Adafruit_NeoPixel.h: No such file or directory
```

**Solutions:**

1. **Install Missing Libraries**
   ```
   Arduino IDE â Sketch â Include Library â Manage Libraries
   
   Search and install:
   - "AudioTools" by pschatzmann
   - "ESP32-A2DP" by pschatzmann  
   - "Adafruit NeoPixel" by Adafruit
   ```

2. **Restart Arduino IDE** after installing libraries

---

### "Board not found" or ESP32 Missing

**Error:**
```
Error: Board esp32:esp32:esp32 not found
```

**Solutions:**

1. **Install ESP32 Board Support**
   ```
   File â Preferences â Additional Board URLs:
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   
   Tools â Board â Boards Manager
   Search: "ESP32"
   Install: "esp32 by Espressif Systems"
   ```

2. **Select Correct Board**
   ```
   Tools â Board â ESP32 Arduino â ESP32 Dev Module
   ```

---

## Upload Issues

### Can't Find COM Port

**Problem:** ESP32 doesn't appear in COM port list

**Solutions:**

1. **Install USB Drivers**
   - ESP32-A1S uses CH340 or CP2102 USB chip
   - Download drivers from manufacturer
   - Restart computer after install

2. **Try Different USB Cable**
   - Some cables are charge-only
   - Use a data-capable cable

---

### Upload Fails / Timeout

**Error:**
```
Failed to connect to ESP32
A fatal error occurred: Timed out waiting for packet header
```

**Solutions:**

1. **Hold BOOT Button**
   ```
   - Press and hold BOOT button
   - Click Upload
   - Keep holding until "Connecting..." appears
   - Release button
   ```

2. **Check Upload Settings**
   ```
   Tools â Upload Speed â Try "115200"
   Tools â Flash Frequency â "80MHz"
   ```

---

## Audio Problems

### No Bluetooth Connection

**Symptoms:**
- Device doesn't appear in Bluetooth list
- Can't pair with phone/computer

**Solutions:**

1. **Verify Bluetooth Started**
   ```
   Check Serial Monitor for:
   "Bluetooth active" or "FerroWave_AUX"
   ```

2. **Restart ESP32**
   ```
   - Power cycle device
   - Wait for Bluetooth initialization
   - Check Serial output
   ```

3. **Forget and Re-pair**
   ```
   - Remove device from Bluetooth settings
   - Restart phone/computer
   - Search for devices again
   ```

---

### Audio Stuttering or Crackling

**Symptoms:**
- Music playback has gaps
- Audio glitches frequently
- Choppy sound

**Solutions:**

1. **Power Issues**
   ```
   - Check battery charge level
   - Reduce LED brightness
   - Lower magnet duty cycle
   ```

2. **Ensure Latest Firmware**
   ```
   - Update to latest non-blocking version
   - Old versions had delay() issues
   ```

---

### AUX Input Not Working

**Symptoms:**
- No audio when AUX cable plugged in
- Auto-switch doesn't work

**Solutions:**

1. **Manual Switch**
   ```
   Serial Monitor command: aux
   This forces AUX input mode
   ```

2. **Verify Cable**
   ```
   - Use different 3.5mm cable
   - Check cable has 3 conductors (TRS)
   - Test cable with other device
   ```

---

## Electromagnet Issues

### Coil Doesn't Activate

**Symptoms:**
- No magnetic field
- Ferrofluid doesn't move
- Test pulse `t` does nothing

**Solutions:**

1. **Check MOSFET Module**
   ```
   - Verify power (VCC and GND connected)
   - Check module LED (should light when active)
   - Measure voltage at gate pin
   - Try different MOSFET module
   ```

2. **Test GPIO 22**
   ```
   Serial Monitor: type 't' for test pulse
   
   Expected:
   - MOSFET LED lights up
   - Coil gets warm
   - 2 second pulse
   ```

3. **Verify Coil**
   ```
   - Measure coil resistance (should be 2-10Î© typically)
   - Check coil connections
   - Test coil directly with 5V (briefly!)
   ```

4. **Check Wiring**
   ```
   Coil (+) â Breadboard (+) rail (5V)
   Coil (-) â MOSFET DRAIN
   MOSFET SOURCE â Breadboard (-) rail (GND)
   GPIO 22 â MOSFET GATE (direct connection)
   ```

---

### Coil Always On

**Symptoms:**
- Magnet never turns off
- Coil gets very hot
- Can't control via software

**Solutions:**

1. **Check for Short**
   ```
   - Power OFF
   - Disconnect gate wire from GPIO 22
   - If coil still on, MOSFET is shorted
   - Replace MOSFET module
   ```

2. **GPIO Stuck High**
   ```
   - Measure voltage at GPIO 22 (should be ~0V when idle)
   - If stuck high, ESP32 issue
   - Try different GPIO pin
   ```

---

### Weak Magnetic Field

**Symptoms:**
- Ferrofluid barely moves
- Expected more dramatic effect

**Solutions:**

1. **Increase Power**
   ```
   Serial commands:
   d90  (increase max duty to 90%)
   s150 (increase sensitivity to 150%)
   ```

2. **Try Different Mode**
   ```
   Try aggressive modes:
   2 (SPIKE)
   4 (CHAOS)
   ```

3. **Check Voltage**
   ```
   - Measure voltage at coil when active
   - Should be close to 5V
   - If much lower, voltage drop issue
   ```

---

### Coil Overheating

**Symptoms:**
- Coil gets very hot to touch
- Plastic melting smell
- System shuts down

**Solutions:**

1. **Reduce Duty Cycle**
   ```
   Serial commands:
   d60  (lower max duty)
   b5   (lower base duty)
   ```

2. **Use Higher Frequency**
   ```
   Serial command:
   f100  (higher frequency = less heating)
   ```

3. **Add Cooling**
   ```
   - Mount heatsink to coil
   - Add small fan
   - Improve airflow around coil
   ```

---

## LED Problems

### LEDs Don't Light Up

**Symptoms:**
- Ring stays dark
- No startup animation
- All modes show nothing

**Solutions:**

1. **Check Power**
   ```
   - Verify LED VCC connected to (+) rail (5V)
   - Verify LED GND connected to (-) rail
   - Measure voltage at LED strip (should be 5V)
   ```

2. **Check Data Connection**
   ```
   - GPIO 23 â 330Î© resistor â LED DIN
   - Verify resistor is present
   - Check for broken wire
   ```

3. **Increase Brightness**
   ```
   Serial command: l255
   Sets maximum brightness
   ```

4. **Try Different Mode**
   ```
   Serial commands:
   c1  (rainbow - easiest to see)
   c3  (solid blue pulse)
   ```

---

### Wrong Colors or Glitching

**Symptoms:**
- Colors are incorrect
- Random flickering
- Some LEDs wrong color

**Solutions:**

1. **Check Ground**
   ```
   CRITICAL: LED GND must share ground with ESP32
   
   - Verify common ground connection
   - Use breadboard (-) rail for all grounds
   - Check continuity
   ```

2. **Data Line Issues**
   ```
   - Check 330Î© resistor is present
   - Keep data wire short (< 12 inches)
   - Route away from power wires
   ```

3. **Power Insufficient**
   ```
   - Reduce LED brightness: l100
   - LEDs draw a lot of current at full white
   ```

---

## Button Issues

### Buttons Don't Respond

**Symptoms:**
- Pressing buttons does nothing
- No Serial output when pressed

**Solutions:**

1. **Check Firmware**
   ```
   - Ensure using latest firmware with button support
   - Verify button code is present
   - Check Serial output for button messages
   ```

2. **Verify Button Pins**
   ```
   ESP32-A1S button pins:
   GPIO 36, 39, 34, 35, 32, 33
   
   - These are built into the board
   - Check pin mappings in code
   ```

---

## Performance Issues

### Laggy Response

**Symptoms:**
- Delay between audio and visual response
- Sluggish magnet/LED updates

**Solutions:**

1. **Ensure Latest Firmware**
   ```
   - Old versions had blocking delays
   - Update to non-blocking version
   ```

2. **Reduce Serial Output**
   ```
   - Lots of Serial.print() slows system
   - Comment out debug messages
   ```

---

## Battery Problems

### Battery Drains Quickly

**Symptoms:**
- Only runs for 20-30 minutes
- Battery gets warm

**Solutions:**

1. **Reduce Power Consumption**
   ```
   - Lower LED brightness: l80
   - Use lower magnet modes: 1 or 8
   - Reduce max duty: d70
   ```

2. **Check Battery Health**
   ```
   - Use higher capacity cells (3500mAh)
   - Check battery age (old batteries = less capacity)
   - Ensure all 4 cells are balanced
   ```

---

### Battery Won't Charge

**Symptoms:**
- No charging indicator
- Batteries don't charge via Micro-USB

**Solutions:**

1. **Check Charger**
   ```
   - Use 5V USB charger (1A-2A)
   - Try different charger
   - Check USB cable
   ```

2. **Check Battery Pack**
   ```
   - Verify Micro-USB port not damaged
   - Check charging LED indicator
   - May need to replace battery pack
   ```

---

## Still Having Issues?

### Get Help

1. **Check Documentation**
   - Read [README.md](readme.md)
   - Review [WIRING.md](wiring.md)
   - Check [commands.md](commands.md)

2. **GitHub Issues**
   - Search existing issues
   - Create new issue with details:
     * Hardware setup
     * Firmware version
     * Serial Monitor output
     * Photos of wiring
     * What you've already tried

3. **Community Support**
   - MakerWorld: https://makerworld.com/en/crowdfunding/70-ferrowave
   - GitHub Discussions

---

**Project**: [FerroWave](https://github.com/matoslav/FerroWave)  
**Author**: Makarov87  
**Crowdfunded by**: MakerWorld Community
