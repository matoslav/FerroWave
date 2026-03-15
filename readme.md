# FerroWave

Audio-reactive **ferrofluid visualizer** built on **ESP32-A1S (ES8388)**.  
Streams Bluetooth audio (A2DP) or AUX input into the on-board codec, extracts live envelopes, and drives:

* a **coil + MOSFET trigger module** (PWM on GPIO 22) for ferrofluid motion
* a **WS2812B LED ring** (GPIO 23) for synced visuals
* **Six onboard buttons** for live preset control (no computer needed!)

> Created by **Makarov87** and **crowdfunded by the MakerWorld community**.  

**Crowdfunding Campaign:** https://makerworld.com/en/crowdfunding/70-ferrowave

---

## Acknowledgments

**This project was made possible by the MakerWorld community!**

FerroWave was successfully crowdfunded on MakerWorld, and it would not exist without the support, belief, and contributions of backers and makers from around the world.

**Special thanks to:**
- All MakerWorld backers who supported this campaign
- The MakerWorld platform for enabling maker-driven innovation
- Every contributor who helped bring this vision to life
- The open-source community for incredible libraries and tools

**Your support made this dream a reality. Thank you!** 🙏

---

## Project Status (AUX + Button Control Edition)

This repo reflects the **latest "AUX + Button Control Edition"** firmware with the finalized hardware layout.

**What's new in this version:**

* **AUX Input Support**: Auto-detects when 3.5mm cable is plugged in and switches from Bluetooth
* **Physical Button Controls**: Six onboard buttons for live control without serial/computer
  + Cycle through magnet modes (8 presets)
  + Cycle through LED effects (10 modes)
  + Switch between EQ presets (8 audio profiles)
* **Dual Input**: Seamlessly switch between Bluetooth and AUX input
* **Non-blocking performance**: Fixed audio stuttering issues for smooth playback

**Previous improvements:**

* Switched to **4x18650 battery pack** with breadboard power distribution:
  + Portable, cordless operation
  + Clean 5V rail for all components
  + 1-1.5 hours runtime typical
  + Simple, no voltage converters needed
* Low-side **MOSFET trigger module** (AOD4184-style):
  + Built-in flyback diode + status LED
  + 3.3 V logic input, 5V coil output
  + Much more maker-friendly than raw MOSFET wiring
* Finalized **pinout** to avoid conflicts with ES8388 codec and onboard buttons:
  + GPIO 22 → **MOSFET gate** (coil PWM)
  + GPIO 23 → **WS2812B LED DIN**
  + ES8388 on standard I²S pins (27 / 25 / 26 / 35 / 0)

---

## Table of Contents

* [Features](#features)
* [Hardware](#hardware)
* [Button Controls](#button-controls)
* [Wiring](#wiring-quick-reference)
* [Power](#power-4x18650-battery-system)
* [Pinout](#pinout)
* [Firmware Setup](#firmware-setup-arduino-ide)
* [Required Libraries](#required-libraries)
* [Build & Upload](#build--upload)
* [Serial Commands](#serial-tuning-panel)
* [How to Use](#how-to-use)
* [Troubleshooting](#troubleshooting)
* [Contributing](#contributing)
* [License](#license)

---

## Features

* **Dual Input Sources**:
  + **Bluetooth A2DP sink** for wireless audio
  + **AUX/Line-In** with auto-detection (switches when cable is plugged in)
* **Six Physical Buttons** for standalone operation:
  + Cycle magnet modes (up/down)
  + Cycle LED effects (up/down)
  + Switch EQ presets (up/down)
* **8 Magnet Behavior Modes**:
  + SMOOTH, SPIKE, BOUNCE, CHAOS, PULSE, WAVE, TREMOLO, BREATH
* **10 LED Visual Effects**:
  + Rainbow, Spectrum, VU-Meter, Pulse, Bass Glow, Fire, Ocean, Strobe, Chase, Sparkle
* **8 EQ Presets**:
  + Flat, Bass Boost, Treble, Vocal, Rock, Electronic, Jazz, Classical
* **Magnet control** via LEDC **PWM** on GPIO 22 with:
  + Adjustable frequency (1-5000 Hz)
  + Tunable duty window and sensitivity
  + Real-time envelope followers
* **Serial tuning panel** for advanced control:
  + Live parameter adjustment
  + Test pulses and diagnostics
  + Status readouts
* Designed to be **stable on ESP32 core 2.0.14+**

---

## Hardware

### Required Components

* **ESP32-A1S / ESP32-Audio-Kit v2.2** (with **ES8388** codec)
* **WS2812B LED ring** (24 LEDs recommended)
* **Low-side MOSFET trigger module** (AOD4184 or similar)
* **Electromagnet** (5V, 1-2A)
* **4x18650 Battery Pack** with 5V output
* **Breadboard Power Module**
* **Ferrofluid display container**

See [docs/BOM.md](docs/BOM.md) for complete bill of materials.

---

## Button Controls

| Button | GPIO | Function | Action |
|--------|------|----------|--------|
| **Button 1** | GPIO 36 | Magnet Mode | Cycle UP through 8 modes |
| **Button 2** | GPIO 39 | Magnet Mode | Cycle DOWN through 8 modes |
| **Button 3** | GPIO 34 | LED Effect | Cycle UP through 10 effects |
| **Button 4** | GPIO 35 | LED Effect | Cycle DOWN through 10 effects |
| **Button 5** | GPIO 32 | EQ Preset | Cycle UP through 8 presets |
| **Button 6** | GPIO 33 | EQ Preset | Cycle DOWN through 8 presets |

---

## Wiring (quick reference)

```
4x18650 Battery Pack (5V Output)
         |
    Power Switch
         |
  Breadboard Power Module
    (+ and - rails)
         |
         ├──> ESP32-A1S (5V + GND)
         ├──> LED Ring (5V + GND)
         └──> Electromagnet + (5V)

LED Ring DIN ──> GPIO 23 (through 330Ω resistor)
ESP32 GPIO 22 ──> MOSFET Gate
```

See [docs/WIRING.md](docs/WIRING.md) for complete instructions.

---

## Power (4x18650 Battery System)

* **Portable operation** with rechargeable batteries
* **1-1.5 hours** typical runtime
* **2-4A** typical draw, up to 5A peak
* Uses quality protected 18650 cells (Samsung, LG, Sony, Panasonic)

---

## Pinout

**Control:**
* GPIO 22 → MOSFET gate (PWM)
* GPIO 23 → LED ring DIN

**Buttons:**
* GPIO 36, 39, 34, 35, 32, 33 (built-in)

**I²S Audio:**
* GPIO 27 → BCK
* GPIO 25 → WS
* GPIO 26 → DATA_OUT

---

## Firmware Setup (Arduino IDE)

1. Install **ESP32 board support** (v2.0.14+)
2. Select **"ESP32 Dev Module"**
3. Install required libraries (see below)
4. Upload firmware
5. Connect via Bluetooth or AUX

---

## Required Libraries

Install via Arduino Library Manager:

1. **AudioTools** by pschatzmann
2. **ESP32-A2DP** by pschatzmann
3. **Adafruit NeoPixel** by Adafruit

See [docs/LIBRARIES.md](docs/LIBRARIES.md) for detailed installation.

---

## Build & Upload

1. Clone repository
2. Open `.ino` file in Arduino IDE
3. Select COM port
4. Press **Upload**
5. Open Serial Monitor (115200 baud)
6. Connect and play music!

---

## Serial Tuning Panel

**Magnet Modes (1-8):**
```
1=SMOOTH  2=SPIKE  3=BOUNCE  4=CHAOS
5=PULSE   6=WAVE   7=TREMOLO 8=BREATH
```

**LED Modes (c1-c10):**
```
c1=Rainbow  c2=Spectrum  c3=Pulse    c4=VU-Meter  c5=Bass Glow
c6=Fire     c7=Ocean     c8=Strobe   c9=Chase     c10=Sparkle
```

**Parameters:**
```
f<num> = PWM frequency    s<num> = Sensitivity
a<num> = Attack speed     r<num> = Release speed
d<num> = Max duty         b<num> = Base duty
p<num> = Spike intensity  l<num> = LED brightness
v<num> = Volume
```

**Utilities:**
```
? = Show settings    t = Test pulse
m = List modes       n = List LED modes
aux = Switch to AUX  bt = Switch to Bluetooth
```

See [commands.md](commands.md) for complete reference.

---

## How to Use

### Standalone (No Computer)
1. Power on
2. Connect Bluetooth or plug AUX cable
3. Play music
4. Use buttons to control presets

### Advanced (Serial Monitor)
1. Connect USB
2. Open Serial Monitor (115200 baud)
3. Use commands for fine tuning
4. Type `?` for current settings

---

## Troubleshooting

**No power?** Check battery charge and connections  
**No audio?** Verify Bluetooth pairing or AUX cable  
**Coil not working?** Test with `t` command  
**LEDs dark?** Try `l255` and check GPIO 23

See [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) for complete guide.

---

## Contributing

Contributions welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

**This project was crowdfunded by the MakerWorld community!**

---

## Community & Support

* **Issues**: [GitHub Issues](https://github.com/matoslav/FerroWave/issues)
* **Discussions**: [GitHub Discussions](https://github.com/matoslav/FerroWave/discussions)
* **MakerWorld**: https://makerworld.com/en/crowdfunding/70-ferrowave

If you want to support me and my FerroWave development, you can buy me a coffee at: https://ko-fi.com/makarov87

---

## Credits

**Created by**: Makarov87  
**Crowdfunded by**: MakerWorld Community

**Libraries:**
* [AudioTools](https://github.com/pschatzmann/arduino-audio-tools) by pschatzmann
* [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) by pschatzmann
* [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) by Adafruit

**Special Thanks:**
* All MakerWorld backers
* Open-source Arduino and ESP32 communities
* Everyone who contributed ideas and feedback

---

## License

MIT License - See the [license](license.md) file.

**Author**: Makarov87  
**Crowdfunded by**: MakerWorld Community

When using or modifying FerroWave:
- Credit Makarov87
- Link to https://github.com/matoslav/FerroWave
- Acknowledge MakerWorld crowdfunding

---

**Created by Makarov87**  
**Crowdfunded by the MakerWorld Community**  
**Built with ❤️ and ferrofluid**

Thank you to all MakerWorld backers! 🙏
