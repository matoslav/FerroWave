# AUX Input on FerroWave

This document describes the AUX input support added to the FerroWave firmware.

## Supported hardware

- AI-Thinker ESP32-A1S Audio Kit v2.2
- ES8388 audio codec
- A female 3.5 mm jack wired to the board AUX IN pins (LINEINL / LINEINR)

## What was changed

The firmware can now switch between two audio sources:

1. **Bluetooth** (default) — the original behaviour. Audio is decoded by the ESP32 and sent to the ES8388 DAC over I2S. The LED ring and the ferrofluid magnet are driven from the decoded audio.
2. **AUX / line-in** — audio from the 3.5 mm jack is routed directly through the ES8388 codec analog bypass to the speakers.

## How to switch source

### Serial commands

Connect to the board at 115200 baud and type:

- `aux` — switch to AUX input
- `bt`  — switch back to Bluetooth

### Physical button

An optional external button can be connected between **GPIO 34** and **GND**. A single press toggles between Bluetooth and AUX. The button is debounced in software.

## Known limitation

In AUX mode the audio never reaches the ESP32. The visualizer (LED ring and ferrofluid magnet) is therefore **idle** while AUX is active.

We tried reading the AUX signal from the ES8388 ADC through the ESP32 I2S interface in full-duplex (RXTX) mode and copying it back to the DAC so the visualizer could react to it. On this hardware that produced audible white-noise bursts in the background. The analog bypass is the cleanest solution we found.

If you know a way to get a clean full-duplex AUX passthrough on the ESP32-A1S + ES8388, we would love to hear about it.

## Required libraries

In addition to the libraries already used by FerroWave, you need:

- [arduino-audio-driver](https://github.com/pschatzmann/arduino-audio-driver) — provides the low-level ES8388 functions used for the analog bypass.

## Build settings

No change from the original firmware:

- ESP32 core: 2.0.14
- Partition scheme: Huge App
- Board: ESP32 Dev Module (or AI Thinker ESP32-A1S if available)

## Hardware note: no automatic jack detection

The stock ESP32-A1S v2.2 board does not expose a GPIO connected to the AUX jack insert/removal switch. Source switching is therefore manual (serial command or button) unless you add a separate jack-detect circuit.
