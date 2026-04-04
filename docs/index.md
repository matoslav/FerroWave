# FerroWave Documentation

Welcome to the FerroWave documentation! This index will help you find the information you need.

**Project**: FerroWave - Audio-Reactive Ferrofluid Visualizer  
**Author**: Makarov87  
**Crowdfunded by**: [MakerWorld Community](https://makerworld.com/en/crowdfunding/70-ferrowave)  
**License**: MIT

---

## Quick Links

- **[Main Repository](https://github.com/matoslav/FerroWave)** - GitHub repository
- **[MakerWorld Campaign](https://makerworld.com/en/crowdfunding/70-ferrowave)** - Crowdfunding page
- **[Issues](https://github.com/matoslav/FerroWave/issues)** - Report bugs or request features
- **[Discussions](https://github.com/matoslav/FerroWave/discussions)** - Community discussion

---

## ð Documentation

### Getting Started

Start here if you're new to FerroWave:

1. **[README.md](../README.md)** - Project overview, features, and quick start
2. **[BOM.md](../BOM.md)** - Complete bill of materials and shopping guide
3. **[LIBRARIES.md](../libraries.md)** - Required Arduino libraries installation
4. **[WIRING.md](../wiring.md)** - Step-by-step wiring instructions

### Building & Programming

Follow these guides to build and program your FerroWave:

- **[WIRING.md](../wiring.md)** - Complete wiring guide with diagrams
  - Power system setup (4x18650 battery)
  - ESP32-A1S connections
  - Electromagnet circuit
  - LED ring wiring
  - Step-by-step assembly

- **[LIBRARIES.md](../libraries.md)** - Library installation
  - Arduino IDE setup
  - ESP32 board support
  - Required libraries (AudioTools, ESP32-A2DP, NeoPixel)
  - Compilation settings

### Using FerroWave

Learn how to operate and customize:

- **[commands.md](../commands.md)** - Serial command reference
  - Magnet modes (1-8)
  - LED effects (c1-c10)
  - Tuning parameters
  - Audio source control
  - Preset recipes

- **[README.md](../README.md)** - Usage guide
  - Button controls
  - EQ presets
  - Recommended settings
  - Performance tips

### Troubleshooting & Support

Having issues? Check here:

- **[TROUBLESHOOTING.md](../troubleshooting.md)** - Comprehensive troubleshooting
  - Power issues
  - Compilation errors
  - Upload problems
  - Audio issues
  - Electromagnet problems
  - LED issues
  - Battery problems

### Contributing

Want to contribute to FerroWave?

- **[CONTRIBUTING.md](../CONTRIBUTING.md)** - Contribution guidelines
  - How to contribute
  - Code style guidelines
  - Development workflow
  - Community guidelines

- **[CHANGELOG.md](../CHANGELOG.md)** - Version history and roadmap

### Legal

- **[LICENSE](../LICENSE)** - MIT License with attribution requirements

---

## ð Document Overview

### Main Documents (Root Level)

| Document | Description | Audience |
|----------|-------------|----------|
| [README.md](../README.md) | Main project documentation | Everyone |
| [LICENSE](../LICENSE) | MIT License & attribution | Everyone |
| [CHANGELOG.md](../CHANGELOG.md) | Version history | Developers |
| [CONTRIBUTING.md](../CONTRIBUTING.md) | How to contribute | Contributors |
| [commands.md](../commands.md) | Serial command reference | Users |

### Documentation Folder (docs/)

| Document | Description | Audience |
|----------|-------------|----------|
| [BOM.md](../BOM.md) | Bill of materials | Builders |
| [LIBRARIES.md](../libraries.md) | Library installation | Builders |
| [WIRING.md](../wiring.md) | Wiring instructions | Builders |
| [TROUBLESHOOTING.md](../troubleshooting.md) | Problem solving | Users |
| [index.md](../docs/index.md) | This document | Everyone |

---

## ð Quick Start Guide

### For First-Time Builders

1. Read [README.md](../README.md) for project overview
2. Check [BOM.md](../BOM.md) and order parts
3. Install libraries following [LIBRARIES.md](../libraries.md)
4. Wire components using [WIRING.md](../wiring.md)
5. Upload firmware from `/firmware` folder
6. Learn commands from [commands.md](../commands.md)

### For Existing Users

1. Check [commands.md](../commands.md) for serial commands
2. Use [TROUBLESHOOTING.md](../troubleshooting.md) if issues arise
3. See [CHANGELOG.md](../CHANGELOG.md) for updates

### For Contributors

1. Read [CONTRIBUTING.md](../CONTRIBUTING.md)
2. Check [CHANGELOG.md](../CHANGELOG.md) for roadmap
3. Review existing issues and discussions

---

## ð¯ Features by Document

### Hardware Information
- **BOM.md** - What to buy
- **WIRING.md** - How to connect
- **TROUBLESHOOTING.md** - Hardware issues

### Software Information
- **LIBRARIES.md** - What to install
- **commands.md** - How to control
- **TROUBLESHOOTING.md** - Software issues

### Project Information
- **README.md** - Overview & features
- **CHANGELOG.md** - Version history
- **CONTRIBUTING.md** - How to help
- **LICENSE** - Legal terms

---

## ð§ By Skill Level

### Beginner
Start with these documents:
1. [README.md](../README.md)
2. [BOM.md](../BOM.md)
3. [LIBRARIES.md](../libraries.md)
4. [WIRING.md](../wiring.md)

### Intermediate
You might need:
- [commands.md](../commands.md) - For advanced tuning
- [TROUBLESHOOTING.md](../troubleshooting.md) - When issues arise

### Advanced
Dive deeper:
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Add features
- [CHANGELOG.md](../CHANGELOG.md) - See roadmap
- Source code in `/firmware`

---

## ð¨ By Task

### I want to build FerroWave
1. [BOM.md](../BOM.md) - Shopping list
2. [LIBRARIES.md](../libraries.md) - Software setup
3. [WIRING.md](../wiring.md) - Assembly guide

### I want to use FerroWave
1. [README.md](../README.md) - Basic operation
2. [commands.md](../commands.md) - Serial commands
3. Button controls (in README.md)

### I want to modify FerroWave
1. [CONTRIBUTING.md](../CONTRIBUTING.md) - Guidelines
2. [commands.md](../commands.md) - Parameter reference
3. Source code in `/firmware`

### I'm having problems
1. [TROUBLESHOOTING.md](../troubleshooting.md) - Problem solving
2. Check [GitHub Issues](https://github.com/matoslav/FerroWave/issues)
3. Ask in [Discussions](https://github.com/matoslav/FerroWave/discussions)

---

## ð¦ Repository Structure

```
FerroWave/
âââ README.md                 # Main documentation
âââ LICENSE                   # MIT License
âââ CHANGELOG.md             # Version history
âââ CONTRIBUTING.md          # Contribution guide
âââ commands.md              # Serial commands
âââ docs/                    # Documentation folder
â   âââ index.md            # This file
â   âââ BOM.md              # Bill of materials
â   âââ LIBRARIES.md        # Library installation
â   âââ WIRING.md           # Wiring guide
â   âââ TROUBLESHOOTING.md  # Troubleshooting
âââ firmware/               # Arduino firmware
â   âââ FerroWave.ino      # Main sketch
âââ hardware/              # (Planned) Schematics
```

---

## ð¡ Tips for Using This Documentation

### Search Tips
- Use Ctrl+F (or Cmd+F) to search within documents
- Check the table of contents in each document
- Look for related sections at document ends

### Navigation Tips
- Use the [Quick Links](#quick-links) section for common tasks
- Browse by [Skill Level](#-by-skill-level) if you're new
- Use [By Task](#-by-task) for specific goals

### Getting Help
1. Search existing documentation first
2. Check [TROUBLESHOOTING.md](../troubleshooting.md)
3. Search [GitHub Issues](https://github.com/matoslav/FerroWave/issues)
4. Ask in [GitHub Discussions](https://github.com/matoslav/FerroWave/discussions)
5. Create new issue if needed

---

## ð Special Thanks

**This project was made possible by the MakerWorld community through crowdfunding.**

Thank you to:
- All MakerWorld backers
- The open-source Arduino and ESP32 communities
- Contributors and testers
- Everyone who provided feedback and support

**[Support the campaign](https://makerworld.com/en/crowdfunding/70-ferrowave)** | **[Star on GitHub](https://github.com/matoslav/FerroWave)**

---

## ð Documentation Updates

This documentation is maintained alongside the FerroWave project. If you find errors or have suggestions:

1. Check if issue already exists
2. Create an [issue](https://github.com/matoslav/FerroWave/issues) describing the problem
3. Or submit a pull request with corrections

See [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines.

---

## ð External Resources

### Hardware
- [ESP32-A1S Documentation](https://docs.ai-thinker.com/)
- [WS2812B Datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)
- [18650 Battery Guide](https://batteryuniversity.com/)

### Software Libraries
- [AudioTools GitHub](https://github.com/pschatzmann/arduino-audio-tools)
- [ESP32-A2DP GitHub](https://github.com/pschatzmann/ESP32-A2DP)
- [Adafruit NeoPixel Guide](https://learn.adafruit.com/adafruit-neopixel-uberguide)

### Community
- [MakerWorld Platform](https://makerworld.com/)
- [Arduino Forum](https://forum.arduino.cc/)
- [ESP32 Community](https://www.esp32.com/)

---

**Last Updated**: December 2024  
**Documentation Version**: 2.0  
**Project**: [FerroWave](https://github.com/matoslav/FerroWave)  
**Author**: Makarov87  
**License**: MIT
