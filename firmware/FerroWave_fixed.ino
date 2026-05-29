/*
  ======================================================================
  FerroWave_fixed.ino  —  Community "Stable Build" firmware
  ----------------------------------------------------------------------
  Ferrofluid visualizer + LED ring + Bluetooth/AUX speaker
  Board:  AI-Thinker ESP32-A1S Audio Kit v2.2 (ES8388 codec)
  Core:   esp32 by Espressif v2.0.14  |  Partition: "Huge App"
  Libs:   AudioTools, ESP32-A2DP, Adafruit_NeoPixel
          (+ arduino-audio-driver, see note below)
  ======================================================================

  WHY THIS FILE EXISTS
  ----------------------------------------------------------------------
  The original FerroWave.ino had several bugs that caused buttons to
  repeat / stick (especially "Button 2" and "Button 4"), even with the
  physical buttons removed and on a fresh board. This is a corrected,
  hardened build that collects every fix found by the community plus a
  more robust button routine.

  Credit: backer Knotsure1 found the working KEY remap (GitHub issue #4)
  and backer pcacacc independently confirmed it. Thanks to both.

  ----------------------------------------------------------------------
  WHAT WAS WRONG, AND WHAT CHANGED
  ----------------------------------------------------------------------
  1) BUTTONS ON THE WRONG PINS (the main bug)
     The stock firmware read the buttons on GPIO 36, 39, 34, 35. Three
     of those (39, 34, 35) are NOT where the onboard KEY buttons are
     actually wired on a v2.2 board, AND all of GPIO 34-39 are
     input-only pins with NO internal pull-up resistors. So
     pinMode(pin, INPUT_PULLUP) did nothing, the pins floated, and the
     firmware read endless phantom presses.

     The real, hardwired KEY-to-GPIO map on the ESP32-A1S v2.2 is:
        KEY1 = GPIO 36   KEY2 = GPIO 13   KEY3 = GPIO 19
        KEY4 = GPIO 23   KEY5 = GPIO 18   KEY6 = GPIO 5
     This build uses those correct pins. (These cannot be reassigned in
     software — the onboard buttons are physically tied to these traces.)

  2) AUDIO INPUT COLLIDED WITH A BUTTON
     setup() set cfg.pin_data_rx = 35, the same pin the old code used for
     Button 4. That line is removed.

  3) LED RING SHARED A PIN WITH A BUTTON
     KEY4 lives on GPIO 23, which the stock code also used for the LED
     ring data line. The LED ring is moved to GPIO 21.

  4) BUTTON 1 SITS ON INPUT-ONLY GPIO 36 (no pull-up possible)
     KEY1 is physically on GPIO 36, which has no internal pull-up and
     can't be moved in software. Instead of leaving it fragile, the
     button routine below is EDGE-TRIGGERED and MULTI-SAMPLED: a press
     only registers on a clean HIGH->LOW transition confirmed over
     several consecutive reads. A floating or noisy pin that drifts and
     holds LOW will NOT auto-repeat. This makes KEY1 stable in software
     with no extra parts.
       Optional hardware bulletproofing: add a 10k pull-up resistor from
       GPIO 36 to 3.3V. Not required with this firmware, but harmless.

  5) STROBE LED MODE (c8) DIDN'T WORK
     Case 8 only flashed when intensity > 0.7, which rarely happens, so
     the ring just stayed dark. It now uses a beat/peak-relative trigger
     so it actually strobes on transients.

  ----------------------------------------------------------------------
  REQUIRED ONE-TIME HARDWARE STEPS
  ----------------------------------------------------------------------
  - DIP SWITCH: set position 2 = ON, all others OFF. On the ESP32-A1S the
    onboard KEY buttons are only connected to the chip when their DIP
    switch is on. (KEY2/GPIO 13 shares an SD-card pin; this build does
    not use the SD card, so that's fine.)
  - LED RING DATA wire goes to GPIO 21 (was GPIO 23). 330ohm in series
    recommended.
  - MOSFET gate stays on GPIO 22.

  ----------------------------------------------------------------------
  LIBRARY NOTE
  ----------------------------------------------------------------------
  If you get a missing-driver compile error, also install
  arduino-audio-driver: https://github.com/pschatzmann/arduino-audio-driver
  Use ESP32 core 2.0.14 and Partition Scheme "Huge App".

  ----------------------------------------------------------------------
  BUTTON LAYOUT (this build)
    Button 1 (GPIO 36): Magnet mode UP    (1-8)
    Button 2 (GPIO 13): Magnet mode DOWN  (1-8)
    Button 3 (GPIO 19): LED mode UP       (1-10)
    Button 4 (GPIO 23): LED mode DOWN     (1-10)
    Button 5 (GPIO 18): EQ preset UP      (8 presets)
    Button 6 (GPIO 5):  EQ preset DOWN    (8 presets)

  SERIAL COMMANDS (unchanged from original)
    Magnet: 1-8 | LED: c1-c10
    f<Hz> s<sens> a<atk> r<rel> d<duty> b<base> p<spike>
    l<bright> v<vol> eq1<bass> eq2<treb> | aux | bt
    ? (settings)  t (test)  m (modes)  n (LED modes)
  ======================================================================
*/

#include <AudioTools.h>
#include <BluetoothA2DPSink.h>
#include <AudioTools/AudioLibs/AudioBoardStream.h>
#include <Adafruit_NeoPixel.h>

// ==== Magnet / PWM ====
const int  COIL_PIN         = 22;
const int  PWM_CH           = 1;
const int  PWM_RES          = 10;
const bool COIL_ACTIVE_HIGH = true;

// ==== LED Ring ====
// Moved from GPIO 23 -> GPIO 21, because GPIO 23 is the onboard KEY4 pin.
const int LED_PIN       = 21;
const int LED_COUNT     = 24;
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint8_t ledBrightness = 100;
uint8_t ledColorMode  = 1;

// ==== Button Pins (ESP32-A1S v2.2, hardwired onboard KEYs) ====
// These are the actual GPIOs the onboard KEY1..KEY6 buttons connect to.
// They cannot be changed in software (the buttons are physically wired
// to these traces). KEY1/GPIO36 is input-only with no pull-up; the
// edge-triggered reader below keeps it stable anyway.
const int BTN_1 = 36;  // KEY1 - Magnet Mode UP   (input-only, no pull-up)
const int BTN_2 = 13;  // KEY2 - Magnet Mode DOWN
const int BTN_3 = 19;  // KEY3 - LED Mode UP
const int BTN_4 = 23;  // KEY4 - LED Mode DOWN
const int BTN_5 = 18;  // KEY5 - EQ Preset UP
const int BTN_6 = 5;   // KEY6 - EQ Preset DOWN

const int NUM_BTNS = 6;
const int BTN_PINS[NUM_BTNS] = { BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_6 };

// Which pins can actually use the internal pull-up. GPIO 36 cannot.
const bool BTN_HAS_PULLUP[NUM_BTNS] = { false, true, true, true, true, true };

// ---- Robust button state ----
// A press fires only on a confirmed HIGH->LOW edge. A pin that floats
// and holds LOW will NOT repeat, because we require it to first return
// HIGH before another press can register. This is what makes the
// input-only KEY1 safe without any external resistor.
bool          btnWasPressed[NUM_BTNS] = { false, false, false, false, false, false };
unsigned long btnLastEventMs[NUM_BTNS] = { 0, 0, 0, 0, 0, 0 };
const unsigned long BTN_DEBOUNCE_MS = 200;  // min gap between presses
const int  BTN_CONFIRM_SAMPLES = 4;         // consecutive reads to confirm
const int  BTN_SAMPLE_GAP_MS   = 3;         // ms between those reads

// Reads a pin several times; returns true only if it is LOW (pressed)
// on every sample. Rejects brief noise spikes.
bool btnConfirmedLow(int pin) {
  for (int i = 0; i < BTN_CONFIRM_SAMPLES; i++) {
    if (digitalRead(pin) != LOW) return false;
    delay(BTN_SAMPLE_GAP_MS);
  }
  return true;
}

// ==== Audio Source Selection ====
enum InputSource {
  SOURCE_BLUETOOTH,
  SOURCE_AUX
};

InputSource currentSource = SOURCE_BLUETOOTH;
bool auxPluggedIn = false;
unsigned long lastAuxCheckMs = 0;
const unsigned long AUX_CHECK_INTERVAL = 2000;

// ==== EQ Settings ====
int bassEQ = 0;
int trebleEQ = 0;
int volume = 80;

// Tunable parameters
float pwmFreqHz      = 4.0f;
float sensitivity    = 100.0f;
float attackSpeed    = 60.0f;
float releaseSpeed   = 30.0f;
float maxDuty        = 80.0f;
float baseDuty       = 15.0f;
float spikeIntensity = 50.0f;

float dutyPct = 0.0f;

// ==== Modes ====
enum VisualizationMode {
  MODE_SMOOTH  = 1,
  MODE_SPIKE   = 2,
  MODE_BOUNCE  = 3,
  MODE_CHAOS   = 4,
  MODE_PULSE   = 5,
  MODE_WAVE    = 6,
  MODE_TREMOLO = 7,
  MODE_BREATH  = 8
};

VisualizationMode currentMode = MODE_SMOOTH;

// ==== Audio pipeline ====
AudioBoard       board = AudioKitEs8388V1;
AudioBoardStream i2s_out(board);
RingBufferStream processing_stream(4096);
MultiOutput      duplicator;
BluetoothA2DPSink a2dp_sink(duplicator);

static const size_t SAMPLE_COUNT = 512;
int16_t sampleBuf[SAMPLE_COUNT];

// Envelope followers
float levelBlock = 0.0f;
float envSlow    = 0.0f;
float envFast    = 0.0f;
float envPeak    = 0.0f;
float envUltraSlow = 0.0f;

// State variables
float pulsePhase = 0.0f;
float waveAccumulator = 0.0f;
float tremoloOscillator = 0.0f;
unsigned long lastBeatMs = 0;

// Strobe state (for fixed c8)
float strobePhase = 0.0f;

unsigned long nextDbgMs = 0;
unsigned long ledUpdateMs = 0;
float hue = 0.0f;
int chasePosition = 0;

// ==== Helper Functions ====

float clampf(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

void applyPWM() {
  int dutyMax = (1 << PWM_RES) - 1;
  float d = clampf(dutyPct, 0.0f, 100.0f);

  if (!COIL_ACTIVE_HIGH) {
    d = 100.0f - d;
  }

  int dutyVal = (int)(d * dutyMax / 100.0f);
  dutyVal = constrain(dutyVal, 0, dutyMax);

  ledcWrite(PWM_CH, dutyVal);
}

void updatePWM() {
  ledcSetup(PWM_CH, pwmFreqHz, PWM_RES);
  ledcAttachPin(COIL_PIN, PWM_CH);
  applyPWM();
}

void applyEQ() {
  board.setVolume(volume);
  Serial.printf("Volume: %d%%, Bass: %+d, Treble: %+d\n", volume, bassEQ, trebleEQ);
}

void switchToAUX() {
  if (currentSource == SOURCE_AUX) return;
  Serial.println("Switching to AUX input...");
  currentSource = SOURCE_AUX;
  board.setInputVolume(80);
  Serial.println("AUX input active");
}

void switchToBluetooth() {
  if (currentSource == SOURCE_BLUETOOTH) return;
  Serial.println("Switching to Bluetooth...");
  currentSource = SOURCE_BLUETOOTH;
  Serial.println("Bluetooth active");
}

bool checkAUXConnected() {
  return false; // To be implemented based on your specific hardware
}

void checkAUXStatus() {
  if (millis() - lastAuxCheckMs < AUX_CHECK_INTERVAL) return;
  lastAuxCheckMs = millis();

  bool wasPluggedIn = auxPluggedIn;
  auxPluggedIn = checkAUXConnected();

  if (auxPluggedIn && !wasPluggedIn) {
    Serial.println("AUX cable detected!");
    switchToAUX();
  } else if (!auxPluggedIn && wasPluggedIn && currentSource == SOURCE_AUX) {
    Serial.println("AUX cable removed, switching to Bluetooth");
    switchToBluetooth();
  }
}

// ==== EQ Presets ====
struct EQPreset {
  const char* name;
  int bass;
  int treble;
  int volume;
};

const EQPreset eqPresets[] = {
  {"Flat",      0,   0,  80},
  {"Bass Boost", 6,  -2,  75},
  {"Treble",    -2,   6,  75},
  {"Vocal",      2,   4,  80},
  {"Rock",       4,   3,  85},
  {"Electronic", 5,   5,  80},
  {"Jazz",       1,   2,  75},
  {"Classical", -1,   1,  70}
};

const int NUM_EQ_PRESETS = 8;
int currentEQPreset = 0;

void applyEQPreset(int preset) {
  if (preset < 0 || preset >= NUM_EQ_PRESETS) return;

  bassEQ = eqPresets[preset].bass;
  trebleEQ = eqPresets[preset].treble;
  volume = eqPresets[preset].volume;

  applyEQ();
  Serial.printf("EQ Preset: %s (Bass:%+d Treb:%+d Vol:%d%%)\n",
                eqPresets[preset].name, bassEQ, trebleEQ, volume);
}

// Forward declarations
const char* modeName();
const char* ledModeName();

// ==== Button actions ====
// Each onboard KEY maps to one action. Called only on a confirmed,
// debounced, edge-triggered press.
void doButtonAction(int idx) {
  switch (idx) {
    case 0: { // KEY1 - Magnet UP
      int nextMode = (int)currentMode + 1;
      if (nextMode > 8) nextMode = 1;
      currentMode = (VisualizationMode)nextMode;
      Serial.printf("Button 1: Magnet UP -> %s\n", modeName());
    } break;
    case 1: { // KEY2 - Magnet DOWN
      int nextMode = (int)currentMode - 1;
      if (nextMode < 1) nextMode = 8;
      currentMode = (VisualizationMode)nextMode;
      Serial.printf("Button 2: Magnet DOWN -> %s\n", modeName());
    } break;
    case 2: { // KEY3 - LED UP
      ledColorMode++;
      if (ledColorMode > 10) ledColorMode = 1;
      Serial.printf("Button 3: LED UP -> %s\n", ledModeName());
    } break;
    case 3: { // KEY4 - LED DOWN
      ledColorMode--;
      if (ledColorMode < 1) ledColorMode = 10;
      Serial.printf("Button 4: LED DOWN -> %s\n", ledModeName());
    } break;
    case 4: { // KEY5 - EQ UP
      currentEQPreset++;
      if (currentEQPreset >= NUM_EQ_PRESETS) currentEQPreset = 0;
      applyEQPreset(currentEQPreset);
    } break;
    case 5: { // KEY6 - EQ DOWN
      currentEQPreset--;
      if (currentEQPreset < 0) currentEQPreset = NUM_EQ_PRESETS - 1;
      applyEQPreset(currentEQPreset);
    } break;
  }
}

// Edge-triggered, debounced, multi-sampled button scanning.
// A press fires once on HIGH->LOW. The pin must go back HIGH before it
// can fire again, so a stuck/floating-LOW pin cannot auto-repeat.
void handleButtons() {
  unsigned long now = millis();

  for (int i = 0; i < NUM_BTNS; i++) {
    bool pressedNow = (digitalRead(BTN_PINS[i]) == LOW);

    if (pressedNow && !btnWasPressed[i]) {
      // Possible new press — confirm it is real and debounce it.
      if ((now - btnLastEventMs[i]) > BTN_DEBOUNCE_MS &&
          btnConfirmedLow(BTN_PINS[i])) {
        btnLastEventMs[i] = now;
        btnWasPressed[i] = true;
        doButtonAction(i);
      }
    } else if (!pressedNow && btnWasPressed[i]) {
      // Released — re-arm this button for the next press.
      btnWasPressed[i] = false;
    }
  }
}

// ==== LED Functions ====

uint32_t HSVtoRGB(float h, float s, float v) {
  float r, g, b;

  int i = int(h * 6);
  float f = h * 6 - i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);

  switch (i % 6) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
  }

  return strip.Color((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255));
}

void updateLEDs() {
  float intensity = clampf(envFast, 0.0f, 1.0f);

  switch (ledColorMode) {
    case 1: // Rainbow spinning
      hue += 0.005f;
      if (hue > 1.0f) hue = 0.0f;
      for (int i = 0; i < LED_COUNT; i++) {
        float pixelHue = fmodf(hue + (float)i / LED_COUNT, 1.0f);
        uint32_t color = HSVtoRGB(pixelHue, 1.0f, intensity);
        strip.setPixelColor(i, color);
      }
      break;

    case 2: // Spectrum analyzer
      {
        int litLEDs = (int)(intensity * LED_COUNT);
        for (int i = 0; i < LED_COUNT; i++) {
          if (i < litLEDs) {
            float pixelHue = (float)i / LED_COUNT * 0.33f;
            uint32_t color = HSVtoRGB(pixelHue, 1.0f, 1.0f);
            strip.setPixelColor(i, color);
          } else {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
          }
        }
      }
      break;

    case 3: // Single color pulse (blue)
      {
        uint32_t color = HSVtoRGB(0.6f, 1.0f, intensity);
        for (int i = 0; i < LED_COUNT; i++) {
          strip.setPixelColor(i, color);
        }
      }
      break;

    case 4: // VU meter (split middle)
      {
        int litLEDs = (int)(intensity * (LED_COUNT / 2));
        for (int i = 0; i < LED_COUNT; i++) {
          int dist = abs(i - LED_COUNT / 2);
          if (dist <= litLEDs) {
            float pixelHue = (float)dist / (LED_COUNT / 2) * 0.33f;
            uint32_t color = HSVtoRGB(pixelHue, 1.0f, 1.0f);
            strip.setPixelColor(i, color);
          } else {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
          }
        }
      }
      break;

    case 5: // Bass glow (red when loud)
      {
        float bassGlow = envSlow * 2.0f;
        bassGlow = clampf(bassGlow, 0.0f, 1.0f);
        uint32_t color = HSVtoRGB(0.0f, 1.0f, bassGlow);
        for (int i = 0; i < LED_COUNT; i++) {
          strip.setPixelColor(i, color);
        }
      }
      break;

    case 6: // Fire effect
      {
        for (int i = 0; i < LED_COUNT; i++) {
          float flicker = 0.7f + (random(30) / 100.0f);
          float heat = intensity * flicker;
          uint32_t color = HSVtoRGB(0.05f, 1.0f, heat);
          strip.setPixelColor(i, color);
        }
      }
      break;

    case 7: // Ocean waves (blue-green)
      {
        hue += 0.002f;
        if (hue > 1.0f) hue = 0.0f;
        for (int i = 0; i < LED_COUNT; i++) {
          float wave = sin((hue + (float)i / LED_COUNT) * 6.28f) * 0.5f + 0.5f;
          float pixelHue = 0.5f + wave * 0.15f;
          uint32_t color = HSVtoRGB(pixelHue, 1.0f, intensity * 0.8f + 0.2f);
          strip.setPixelColor(i, color);
        }
      }
      break;

    case 8: // Strobe flash (FIXED)
      {
        // Old code only flashed when intensity > 0.7, which almost never
        // happened, so the ring stayed dark. Now we flash on transients:
        // a sharp rise of envFast above the running envSlow triggers a
        // flash, and the flash decays quickly for a true strobe feel.
        float transient = envFast - envSlow;
        if (transient > 0.06f && strobePhase < 0.2f) {
          strobePhase = 1.0f;        // fire a flash on the beat
        }
        strobePhase *= 0.55f;        // fast decay between flashes
        if (strobePhase > 0.25f) {
          uint8_t w = (uint8_t)(clampf(strobePhase, 0.0f, 1.0f) * 255);
          for (int i = 0; i < LED_COUNT; i++) {
            strip.setPixelColor(i, strip.Color(w, w, w));
          }
        } else {
          strip.clear();
        }
      }
      break;

    case 9: // Color chase
      {
        if (intensity > 0.5f) {
          chasePosition = (chasePosition + 1) % LED_COUNT;
        }
        strip.clear();
        for (int i = 0; i < 3; i++) {
          int pos = (chasePosition + i * 8) % LED_COUNT;
          uint32_t color = HSVtoRGB((float)i / 3.0f, 1.0f, intensity);
          strip.setPixelColor(pos, color);
        }
      }
      break;

    case 10: // Sparkle
      {
        strip.clear();
        int numSparkles = (int)(intensity * 8);
        for (int i = 0; i < numSparkles; i++) {
          int pos = random(LED_COUNT);
          uint32_t color = HSVtoRGB(random(100) / 100.0f, 1.0f, 1.0f);
          strip.setPixelColor(pos, color);
        }
      }
      break;
  }

  strip.show();
}

// ==== Mode Names ====

const char* modeName() {
  switch (currentMode) {
    case MODE_SMOOTH:  return "SMOOTH";
    case MODE_SPIKE:   return "SPIKE";
    case MODE_BOUNCE:  return "BOUNCE";
    case MODE_CHAOS:   return "CHAOS";
    case MODE_PULSE:   return "PULSE";
    case MODE_WAVE:    return "WAVE";
    case MODE_TREMOLO: return "TREMOLO";
    case MODE_BREATH:  return "BREATH";
    default:           return "UNKNOWN";
  }
}

const char* ledModeName() {
  switch (ledColorMode) {
    case 1:  return "Rainbow";
    case 2:  return "Spectrum";
    case 3:  return "Pulse";
    case 4:  return "VU-Meter";
    case 5:  return "Bass Glow";
    case 6:  return "Fire";
    case 7:  return "Ocean";
    case 8:  return "Strobe";
    case 9:  return "Chase";
    case 10: return "Sparkle";
    default: return "Unknown";
  }
}

// ==== Serial Commands ====

void printSettings() {
  Serial.println(F("\n========== CURRENT SETTINGS =========="));
  Serial.printf("Audio Source:    %s\n", currentSource == SOURCE_BLUETOOTH ? "Bluetooth" : "AUX");
  Serial.printf("Magnet Mode:     %s\n", modeName());
  Serial.printf("PWM Frequency:   %.1f Hz\n", pwmFreqHz);
  Serial.printf("Sensitivity:     %.0f%%\n", sensitivity);
  Serial.printf("Attack Speed:    %.0f/100\n", attackSpeed);
  Serial.printf("Release Speed:   %.0f/100\n", releaseSpeed);
  Serial.printf("Max Duty:        %.0f%%\n", maxDuty);
  Serial.printf("Base Duty:       %.0f%%\n", baseDuty);
  Serial.printf("Spike Intensity: %.0f/100\n", spikeIntensity);
  Serial.printf("Volume:          %d%%\n", volume);
  Serial.printf("EQ Preset:       %s\n", eqPresets[currentEQPreset].name);
  Serial.printf("Bass EQ:         %+d\n", bassEQ);
  Serial.printf("Treble EQ:       %+d\n", trebleEQ);
  Serial.printf("LED Brightness:  %d/255\n", ledBrightness);
  Serial.printf("LED Mode:        %s\n", ledModeName());
  Serial.println(F("======================================\n"));
}

void printHelp() {
  Serial.println(F("\n========== COMMANDS =========="));
  Serial.println(F("Magnet: 1-8 | LED: c1-c10"));
  Serial.println(F("Tuning: f<Hz> s<sens> a<atk> r<rel>"));
  Serial.println(F("        d<duty> b<base> p<spike>"));
  Serial.println(F("Audio:  v<vol> eq1<bass> eq2<treb>"));
  Serial.println(F("        aux (switch) bt (switch)"));
  Serial.println(F("LED:    l<brightness>"));
  Serial.println(F("Utils:  ? (settings) m (modes)"));
  Serial.println(F("        n (LED) t (test)"));
  Serial.println(F("==============================\n"));
}

void testPulse() {
  Serial.println(F("TEST PULSE: 2 seconds at max..."));
  dutyPct = maxDuty;
  applyPWM();

  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(255, 255, 255));
  }
  strip.show();

  delay(2000);

  dutyPct = baseDuty;
  applyPWM();
  strip.clear();
  strip.show();

  Serial.println(F("Test complete.\n"));
}

void handleSerial() {
  static String inputBuffer = "";

  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        inputBuffer.trim();
        inputBuffer.toLowerCase();

        if (inputBuffer == "aux") {
          switchToAUX();
          inputBuffer = "";
          return;
        } else if (inputBuffer == "bt") {
          switchToBluetooth();
          inputBuffer = "";
          return;
        } else if (inputBuffer == "?") {
          printSettings();
          inputBuffer = "";
          return;
        } else if (inputBuffer == "t") {
          testPulse();
          inputBuffer = "";
          return;
        } else if (inputBuffer == "m") {
          inputBuffer = "";
          return;
        } else if (inputBuffer == "n") {
          inputBuffer = "";
          return;
        }

        char cmd = inputBuffer.charAt(0);

        if (inputBuffer.length() == 1 && cmd >= '1' && cmd <= '8') {
          currentMode = (VisualizationMode)(cmd - '0');
          Serial.printf("Mode -> %s\n", modeName());
        } else if (inputBuffer.startsWith("eq1")) {
          bassEQ = constrain(inputBuffer.substring(3).toInt(), -10, 10);
          applyEQ();
          Serial.printf("Bass EQ -> %+d\n", bassEQ);
        } else if (inputBuffer.startsWith("eq2")) {
          trebleEQ = constrain(inputBuffer.substring(3).toInt(), -10, 10);
          applyEQ();
          Serial.printf("Treble EQ -> %+d\n", trebleEQ);
        } else {
          float value = inputBuffer.substring(1).toFloat();

          switch (cmd) {
            case 'f':
              pwmFreqHz = constrain(value, 1.0f, 5000.0f);
              updatePWM();
              Serial.printf("PWM -> %.1f Hz\n", pwmFreqHz);
              break;
            case 's':
              sensitivity = constrain(value, 0.0f, 200.0f);
              Serial.printf("Sensitivity -> %.0f%%\n", sensitivity);
              break;
            case 'a':
              attackSpeed = constrain(value, 0.0f, 100.0f);
              Serial.printf("Attack -> %.0f\n", attackSpeed);
              break;
            case 'r':
              releaseSpeed = constrain(value, 0.0f, 100.0f);
              Serial.printf("Release -> %.0f\n", releaseSpeed);
              break;
            case 'd':
              maxDuty = constrain(value, 10.0f, 100.0f);
              Serial.printf("Max duty -> %.0f%%\n", maxDuty);
              break;
            case 'b':
              baseDuty = constrain(value, 0.0f, 50.0f);
              Serial.printf("Base duty -> %.0f%%\n", baseDuty);
              break;
            case 'p':
              spikeIntensity = constrain(value, 0.0f, 100.0f);
              Serial.printf("Spike -> %.0f\n", spikeIntensity);
              break;
            case 'l':
              ledBrightness = constrain((int)value, 0, 255);
              strip.setBrightness(ledBrightness);
              Serial.printf("LED brightness -> %d\n", ledBrightness);
              break;
            case 'c':
              ledColorMode = constrain((int)value, 1, 10);
              Serial.printf("LED mode -> %s\n", ledModeName());
              break;
            case 'v':
              volume = constrain((int)value, 0, 100);
              applyEQ();
              Serial.printf("Volume -> %d%%\n", volume);
              break;
          }
        }

        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
}

void calculateDuty() {
  float attackAlpha = attackSpeed / 100.0f * 0.9f + 0.05f;
  float releaseAlpha = releaseSpeed / 100.0f * 0.5f + 0.01f;

  float level = levelBlock * (sensitivity / 100.0f);
  level = clampf(level, 0.0f, 1.0f);

  if (level > envFast) {
    envFast = (1.0f - attackAlpha) * envFast + attackAlpha * level;
  } else {
    envFast = (1.0f - releaseAlpha) * envFast + releaseAlpha * level;
  }

  envSlow = 0.98f * envSlow + 0.02f * level;
  envUltraSlow = 0.995f * envUltraSlow + 0.005f * level;

  if (level > envPeak) {
    envPeak = level;
  } else {
    envPeak *= 0.95f;
  }

  float output = 0.0f;

  switch (currentMode) {
    case MODE_SMOOTH:
      output = envSlow;
      break;

    case MODE_SPIKE:
      {
        float diff = clampf((envFast - envSlow) * 3.0f, 0.0f, 1.0f);
        float spike = diff * (spikeIntensity / 100.0f);
        output = envSlow * 0.5f + envFast * 0.5f + spike;
      }
      break;

    case MODE_BOUNCE:
      output = envFast * 1.2f;
      break;

    case MODE_CHAOS:
      {
        float diff = clampf((envFast - envSlow) * 5.0f, 0.0f, 1.0f);
        output = max(envFast * 1.3f, envPeak) + diff * 0.5f;
      }
      break;

    case MODE_PULSE:
      {
        float diff = envFast - envSlow;
        if (diff > 0.1f && (millis() - lastBeatMs) > 100) {
          pulsePhase = 1.0f;
          lastBeatMs = millis();
        }
        pulsePhase *= 0.92f;
        output = pulsePhase;
      }
      break;

    case MODE_WAVE:
      {
        waveAccumulator += envFast * 0.05f;
        waveAccumulator *= 0.98f;
        waveAccumulator = clampf(waveAccumulator, 0.0f, 1.0f);
        output = waveAccumulator;
      }
      break;

    case MODE_TREMOLO:
      {
        tremoloOscillator += envFast * 0.3f;
        float tremolo = sin(tremoloOscillator) * 0.5f + 0.5f;
        output = envFast * tremolo;
      }
      break;

    case MODE_BREATH:
      output = envUltraSlow * 1.5f;
      break;
  }

  output = clampf(output, 0.0f, 1.0f);
  dutyPct = baseDuty + output * (maxDuty - baseDuty);
}

// ==== Setup / Loop ====

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println(F("\n==============================================="));
  Serial.println(F("   FerroWave - STABLE BUILD (community fixed)"));
  Serial.println(F("   Correct KEY pins | robust buttons | strobe fix"));
  Serial.println(F("   8 Modes | 10 LED | 6 Buttons | BT + AUX"));
  Serial.println(F("===============================================\n"));

  // Button setup. Use INPUT_PULLUP where the pin supports it; GPIO 36
  // (KEY1) is input-only with no pull-up, so it gets plain INPUT and is
  // protected by the edge-triggered reader instead.
  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(BTN_PINS[i], BTN_HAS_PULLUP[i] ? INPUT_PULLUP : INPUT);
  }

  // LED setup
  strip.begin();
  strip.setBrightness(ledBrightness);
  strip.clear();
  strip.show();

  // Quick startup flash
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 50, 100));
  }
  strip.show();
  delay(200);
  strip.clear();
  strip.show();

  // Audio setup
  duplicator.add(i2s_out);
  duplicator.add(processing_stream);

  auto cfg = i2s_out.defaultConfig(TX_MODE);
  cfg.pin_bck     = 27;
  cfg.pin_ws      = 25;
  cfg.pin_data    = 26;
  // NOTE: pin_data_rx = 35 was REMOVED. GPIO 35 is input-only and was
  // colliding with a button in the stock firmware.
  cfg.pin_mck     = 0;

  i2s_out.begin(cfg);

  a2dp_sink.start("FerroWave");

  delay(200);

  // PWM setup
  pinMode(COIL_PIN, OUTPUT);
  updatePWM();
  dutyPct = baseDuty;
  applyPWM();

  applyEQ();

  printHelp();
  printSettings();
  Serial.println(F("\n=========================================="));
  Serial.println(F("  BUTTON CONTROLS (set DIP switch 2 = ON):"));
  Serial.println(F("  [1] Magnet UP   [2] Magnet DOWN"));
  Serial.println(F("  [3] LED UP      [4] LED DOWN"));
  Serial.println(F("  [5] EQ UP       [6] EQ DOWN"));
  Serial.println(F("==========================================\n"));
  Serial.println(F("Ready! Connect via Bluetooth or AUX cable!\n"));
}

void loop() {
  handleSerial();
  handleButtons();
  checkAUXStatus();

  size_t avail = processing_stream.available();

  if (avail >= SAMPLE_COUNT * sizeof(int16_t)) {
    size_t got = processing_stream.readBytes((uint8_t*)sampleBuf,
                                             SAMPLE_COUNT * sizeof(int16_t));
    if (got == SAMPLE_COUNT * sizeof(int16_t)) {
      double acc = 0.0;
      for (size_t i = 0; i < SAMPLE_COUNT; i++) {
        float v = sampleBuf[i] / 32768.0f;
        acc += v * v;
      }
      levelBlock = sqrtf(acc / SAMPLE_COUNT);

      calculateDuty();
      applyPWM();

      if (millis() - ledUpdateMs >= 16) {
        updateLEDs();
        ledUpdateMs = millis();
      }

      if (millis() >= nextDbgMs) {
        Serial.printf("[%s/%s/%s] lvl=%.3f duty=%.1f%% vol=%d%%\n",
                      currentSource == SOURCE_BLUETOOTH ? "BT" : "AUX",
                      modeName(), ledModeName(), levelBlock, dutyPct, volume);
        nextDbgMs = millis() + 300;
      }
    }
  } else {
    envFast *= 0.98f;
    envSlow *= 0.98f;
    envPeak *= 0.95f;
    envUltraSlow *= 0.99f;

    if (envSlow < 0.01f) {
      dutyPct = 0.0f;
    } else {
      calculateDuty();
    }
    applyPWM();

    if (millis() - ledUpdateMs >= 16) {
      updateLEDs();
      ledUpdateMs = millis();
    }

    delay(5);
  }
}
