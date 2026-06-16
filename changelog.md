# Changelog

All notable changes to FerroWave will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.0.0] - 2024-12-14

### Added
- **AUX Input Support**: Auto-detection and manual switching for 3.5mm line input
- **Physical Button Controls**: Six onboard buttons for standalone operation
  - Buttons 1/2: Cycle magnet modes up/down
  - Buttons 3/4: Cycle LED modes up/down
  - Buttons 5/6: Cycle EQ presets up/down
- **8 EQ Presets**: Optimized audio profiles (Flat, Bass Boost, Treble, Vocal, Rock, Electronic, Jazz, Classical)
- **Battery Power System**: 4x18650 battery pack with 5V output
- **Breadboard Power Module**: Clean power distribution for all components
- Non-blocking button debouncing (200ms)
- Manual audio source switching via serial commands (`aux`, `bt`)
- Volume control via serial (`v<num>`)

### Changed
- **Power System**: Replaced USB-C PD trigger with 4x18650 battery system
  - Simpler wiring (single 5V rail)
  - Truly portable and cordless
  - 1-1.5 hours typical runtime
- **Performance**: Eliminated all blocking `delay()` calls
  - Fixed audio stuttering issues
  - Smooth 60fps LED updates
  - Responsive button handling
- **Startup**: Reduced LED animation delay from 720ms to 200ms
- **AUX Check Interval**: Increased from 500ms to 2000ms for efficiency
- Updated documentation for battery-powered design

### Fixed
- Audio stuttering caused by blocking delays in button handling
- LED blink on startup (reduced animation time)
- Naming conflict with AudioTools library (renamed enum to `InputSource`)
- Performance degradation from frequent AUX detection checks

### Documentation
- Complete rewrite of README.md with battery system
- New WIRING.md with 4x18650 setup
- New BOM.md (Bill of Materials)
- New CONTRIBUTING.md with crowdfunding acknowledgment
- New TROUBLESHOOTING.md with comprehensive solutions
- Updated LICENSE with MakerWorld crowdfunding attribution
- New commands.md with all serial commands documented

---

## [1.0.0] - 2024-11-XX

### Added
- **8 Magnet Behavior Modes**: SMOOTH, SPIKE, BOUNCE, CHAOS, PULSE, WAVE, TREMOLO, BREATH
- **10 LED Visual Effects**: Rainbow, Spectrum, VU-Meter, Pulse, Bass Glow, Fire, Ocean, Strobe, Chase, Sparkle
- **Serial Tuning Panel**: Live parameter adjustment via Serial Monitor
- **PWM Frequency Control**: Adjustable from 1-5000 Hz (`f<num>`)
- **Sensitivity Control**: 0-200% gain adjustment (`s<num>`)
- **Attack/Release Control**: Independent envelope shaping (`a<num>`, `r<num>`)
- **Duty Cycle Control**: Max and base duty settings (`d<num>`, `b<num>`)
- **Spike Intensity**: Extra boost for transient modes (`p<num>`)
- **LED Brightness Control**: 0-255 brightness levels (`l<num>`)
- **Test Pulse Command**: 2-second full-power test (`t`)
- **Status Commands**: Show settings (`?`), list modes (`m`, `n`)
- Bluetooth A2DP audio sink
- ES8388 audio codec support
- Multi-envelope audio analysis (fast, slow, peak, ultra-slow)
- MOSFET trigger module support with built-in flyback protection
- USB-C PD power support (later replaced with battery)

### Hardware
- ESP32-A1S Audio Kit v2.2 support
- WS2812B LED ring control (GPIO 21)
- Electromagnet PWM control (GPIO 22)
- AOD4184 MOSFET trigger module integration

### Documentation
- Initial README.md
- Serial commands reference
- Wiring instructions
- Preset recommendations

---

## [0.5.0] - 2024-10-XX (Pre-release)

### Added
- Basic ferrofluid control via Bluetooth audio
- Simple PWM electromagnet driving
- Basic LED strip support
- Prototype firmware testing

### Initial Concept
- ESP32-based audio reactive ferrofluid
- Breadboard prototype
- Manual parameter tuning in code

---

## Crowdfunding Campaign

**FerroWave was successfully crowdfunded on MakerWorld!**

Campaign: https://makerworld.com/en/crowdfunding/70-ferrowave

Special thanks to all backers who made this project possible. Your support brought FerroWave from concept to reality.

---

## Future Roadmap

### Planned for v2.1
- [ ] 3D-printable enclosure design
- [ ] Battery level indicator
- [ ] Power-saving sleep mode
- [ ] Preset save/load functionality
- [ ] OLED display for status

### Planned for v3.0
- [ ] Web interface for wireless configuration
- [ ] MIDI control support
- [ ] Multiple coil support (3D movement)
- [ ] Smartphone app
- [ ] Advanced audio analysis (FFT)

### Under Consideration
- [ ] Microphone input for acoustic response
- [ ] SD card for storing presets
- [ ] Real-time frequency analysis visualization
- [ ] Integration with music streaming services

---

## Version Numbering

FerroWave follows Semantic Versioning:

- **MAJOR** version: Incompatible hardware or API changes
- **MINOR** version: New features in backwards-compatible manner
- **PATCH** version: Backwards-compatible bug fixes

---

## Links

- **Repository**: https://github.com/matoslav/FerroWave
- **Crowdfunding**: https://makerworld.com/en/crowdfunding/70-ferrowave
- **Issues**: https://github.com/matoslav/FerroWave/issues
- **Discussions**: https://github.com/matoslav/FerroWave/discussions

---

**Project**: FerroWave  
**Author**: Makarov87  
**Crowdfunded by**: MakerWorld Community  
**License**: MIT
