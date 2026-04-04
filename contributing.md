# Contributing to FerroWave

Thank you for your interest in contributing to FerroWave! This project was made possible by the MakerWorld community through crowdfunding, and we welcome contributions from makers, developers, and enthusiasts worldwide.

---

## About FerroWave

**FerroWave** is an open-source ferrofluid visualizer created by **Makarov87** and crowdfunded by the amazing **MakerWorld community**.

- **Crowdfunding Campaign**: https://makerworld.com/en/crowdfunding/70-ferrowave
- **GitHub Repository**: https://github.com/matoslav/FerroWave
- **License**: MIT License (see LICENSE file)

**This project would not exist without the support of MakerWorld backers. Thank you!** ð

---

## How You Can Contribute

We welcome contributions in many forms:

### ð» Code Contributions
- Bug fixes
- New magnet behavior modes
- Additional LED effects
- Performance optimizations
- New features (AUX improvements, MIDI support, etc.)

### ð Documentation
- Improve existing documentation
- Add tutorials and guides
- Translate documentation
- Create video tutorials
- Share build photos and tips

### ð§ Hardware
- Test on different ESP32 boards
- Design 3D printed enclosures
- Create PCB layouts
- Improve power management solutions
- Document hardware modifications

### ð¨ Creative
- Share your ferrofluid display designs
- Create demo videos
- Design artwork or graphics
- Suggest new visual effects

### ð Bug Reports
- Report issues you encounter
- Provide detailed reproduction steps
- Share Serial Monitor logs
- Document workarounds you've found

---

## Getting Started

### Prerequisites

Before contributing code, make sure you have:

1. **Arduino IDE** (1.8.x or 2.x)
2. **ESP32 board support** (version 2.0.14+)
3. **Required libraries** (see [LIBRARIES.md](libraries.md))
4. **ESP32-A1S Audio Kit** (recommended for testing)
5. Basic understanding of C++ and Arduino

### Setting Up Your Development Environment

1. **Fork the repository** on GitHub
2. **Clone your fork**:
   ```bash
   git clone https://github.com/YOUR-USERNAME/FerroWave.git
   cd FerroWave
   ```
3. **Install dependencies** (libraries via Arduino Library Manager)
4. **Open the firmware** in Arduino IDE
5. **Test on hardware** if possible

---

## Contribution Workflow

### 1. Create an Issue First

Before starting work on a new feature or major change:

1. **Search existing issues** to avoid duplicates
2. **Create a new issue** describing:
   - What you want to add/change
   - Why it's useful
   - How you plan to implement it
3. **Wait for feedback** from maintainers

For bug fixes, you can skip this step if the issue already exists.

### 2. Create a Branch

Create a descriptive branch name:

```bash
git checkout -b feature/new-magnet-mode
git checkout -b fix/led-flicker-issue
git checkout -b docs/wiring-improvements
```

### 3. Make Your Changes

**Code Guidelines:**
- Follow existing code style and formatting
- Add comments for complex logic
- Keep functions focused and concise
- Test thoroughly on hardware if possible
- Don't break existing functionality

**Documentation Guidelines:**
- Use clear, simple language
- Include examples where helpful
- Add images/diagrams if relevant
- Proofread for spelling and grammar

### 4. Test Your Changes

**For Code Changes:**
- Compile without errors
- Test on actual hardware if possible
- Verify existing features still work
- Check Serial Monitor output
- Test with different presets/modes

**For Documentation:**
- Read through your changes
- Check all links work
- Verify formatting renders correctly
- Test any code examples provided

### 5. Commit Your Changes

Write clear commit messages:

```bash
git add .
git commit -m "Add new RIPPLE magnet mode for water-like effects"
```

**Good commit message examples:**
- `Fix LED flickering on mode change`
- `Add Spanish translation to README`
- `Improve battery life with sleep mode`
- `Update wiring diagram for clarity`

**Bad commit message examples:**
- `fixes`
- `update`
- `changes`

### 6. Push and Create Pull Request

```bash
git push origin feature/new-magnet-mode
```

Then on GitHub:
1. Go to your fork
2. Click "Pull Request"
3. Fill in the PR template with:
   - **Description**: What does this PR do?
   - **Motivation**: Why is this change needed?
   - **Testing**: How did you test it?
   - **Screenshots**: If visual changes, include images
   - **Related Issues**: Link any related issues

### 7. Respond to Feedback

- Be open to suggestions
- Make requested changes promptly
- Ask questions if feedback is unclear
- Be respectful and professional

---

## Code Style Guidelines

### General Principles

- **Readable over clever**: Clear code is better than short code
- **Comment non-obvious logic**: Explain *why*, not just *what*
- **Consistent naming**: Follow existing conventions
- **Avoid blocking delays**: Use millis() timing instead of delay()

### Naming Conventions

```cpp
// Constants: ALL_CAPS with underscores
const int LED_PIN = 23;
const float MAX_DUTY = 80.0f;

// Variables: camelCase
float dutyPct = 0.0f;
int ledBrightness = 100;

// Functions: camelCase
void updateLEDs() { }
float clampf(float x, float lo, float hi) { }

// Enums: PascalCase for type, ALL_CAPS for values
enum VisualizationMode {
  MODE_SMOOTH = 1,
  MODE_SPIKE = 2
};
```

### Code Formatting

```cpp
// Use spaces, not tabs (2 spaces per indent level)
if (condition) {
  doSomething();
}

// Braces on same line for functions
void myFunction() {
  // code here
}

// Space after keywords
if (x > 0) { }
for (int i = 0; i < 10; i++) { }
while (running) { }

// No space after function names
myFunction();
Serial.println("text");
```

### Comments

```cpp
// Good: Explain WHY
// Use fast attack for percussive sounds
envFast = (1.0f - attackAlpha) * envFast + attackAlpha * level;

// Bad: Explain WHAT (obvious from code)
// Set envFast to the result of the calculation
envFast = (1.0f - attackAlpha) * envFast + attackAlpha * level;

// Good: Document non-obvious behavior
// PWM frequency below 10Hz creates visible pulsing effect
if (pwmFreqHz < 10.0f) {
  // ...
}
```

### Performance Considerations

```cpp
// â Good: Non-blocking
unsigned long lastUpdate = 0;
if (millis() - lastUpdate > 100) {
  updateLEDs();
  lastUpdate = millis();
}

// â Bad: Blocking (causes audio stutter)
updateLEDs();
delay(100);

// â Good: Efficient
float value = envFast * sensitivity;

// â Bad: Unnecessary division in loop
for (int i = 0; i < 100; i++) {
  float value = envFast * (sensitivity / 100.0f);
}
```

---

## Adding New Features

### Adding a New Magnet Mode

1. Add to enum in main sketch:
   ```cpp
   enum VisualizationMode {
     MODE_SMOOTH  = 1,
     MODE_SPIKE   = 2,
     // ... existing modes ...
     MODE_YOUR_NEW_MODE = 9
   };
   ```

2. Add case to `calculateDuty()` function:
   ```cpp
   case MODE_YOUR_NEW_MODE:
     // Your algorithm here
     output = yourCalculation();
     break;
   ```

3. Update `modeName()` function:
   ```cpp
   case MODE_YOUR_NEW_MODE: return "YOUR_NAME";
   ```

4. Update documentation (README.md, commands.md)
5. Test thoroughly with different music styles

### Adding a New LED Mode

1. Add to LED mode selection:
   ```cpp
   case 11: // New mode number
     // Your LED effect code
     break;
   ```

2. Update `ledModeName()` function
3. Update max mode count
4. Document the new mode
5. Test with different audio inputs

---

## Community Guidelines

### Code of Conduct

We follow a simple code of conduct:

1. **Be Respectful**: Treat everyone with respect and kindness
2. **Be Inclusive**: Welcome people of all backgrounds and skill levels
3. **Be Constructive**: Provide helpful feedback, not just criticism
4. **Be Patient**: Remember everyone is learning
5. **Be Collaborative**: Work together towards common goals

### Communication

- **Be Clear**: Explain your ideas clearly
- **Be Specific**: Provide details and examples
- **Be Responsive**: Reply to questions and feedback
- **Be Professional**: Keep discussions on-topic

---

## Recognition

### Contributors

All contributors will be recognized:
- Listed in CONTRIBUTORS.md file
- Mentioned in release notes
- Credited in documentation (for significant contributions)

### Commit Rights

Regular contributors may be invited to become maintainers with commit access.

---

## License

By contributing to FerroWave, you agree that your contributions will be licensed under the MIT License (same as the project).

You also agree that all contributions will properly credit:
- Original author: Makarov87
- Original project: FerroWave
- Crowdfunding community: MakerWorld backers

See [LICENSE](LICENSE) file for full details.

---

## Questions?

- **GitHub Issues**: For bug reports and feature requests
- **GitHub Discussions**: For questions and general discussion
- **MakerWorld**: https://makerworld.com/en/crowdfunding/70-ferrowave

---

## Thank You!

Thank you for contributing to FerroWave! Your help makes this project better for everyone.

**Special thanks to the MakerWorld community who made this project possible through crowdfunding!** ð

---

*This project is open source, but it wouldn't exist without the support of the MakerWorld backers. When you contribute, you're joining a community that believes in maker-driven innovation.*

---

**Project**: [FerroWave](https://github.com/matoslav/FerroWave)  
**Author**: Makarov87  
**Crowdfunded by**: MakerWorld Community  
**License**: MIT
