# FerroWave Wiring Guide

Complete wiring instructions for the FerroWave ferrofluid visualizer.

---

## Table of Contents

- [Safety Warning](#safety-warning)
- [Required Components](#required-components)
- [Power Distribution](#power-distribution)
- [ESP32-A1S Connections](#esp32-a1s-connections)
- [Electromagnet Circuit](#electromagnet-circuit)
- [LED Ring Wiring](#led-ring-wiring)
- [Battery Power System](#battery-power-system)
- [Wiring Diagram](#wiring-diagram)
- [Step-by-Step Assembly](#step-by-step-assembly)
- [Testing](#testing)
- [Troubleshooting](#troubleshooting)

---

## Safety Warning

⚠️ **IMPORTANT SAFETY INFORMATION**

- Always disconnect power before making wiring changes
- Double-check polarity before connecting power
- Electromagnets can get HOT - monitor temperature during operation
- Use proper gauge wire for current requirements
- Include a flyback diode to protect the MOSFET
- Never exceed the voltage/current ratings of components
- **Battery Safety**: Never short circuit 18650 cells
- Use proper 18650 batteries with protection circuits
- Charge batteries with appropriate charger only
- Monitor battery temperature during use
- Work in a well-ventilated area
- Keep ferrofluid away from electronics and clothing (it stains!)

---

## Required Components

### Main Components

| Component | Specification | Notes |
|-----------|--------------|-------|
| ESP32-A1S Audio Kit v2.2 | With ES8388 codec | Must be v2.2 or compatible |
| MOSFET Trigger Module | AOD4184 or similar | Low-side N-channel with flyback diode |
| Electromagnet | 5V, 1-2A | Sized for 5V operation |
| WS2812B LED Ring | 24 LEDs (or similar) | 5V addressable RGB |
| 4x18650 Battery Holder | 5V output module | With built-in 5V regulation |
| Breadboard Power Module | Dual rail | Creates 5V power rails |

### Additional Components

| Component | Specification | Purpose |
|-----------|--------------|---------|
| Flyback Diode | SS34, 1N5819, or UF4007 | Coil protection (if not on module) |
| LED Data Resistor | 330Ω | Protects WS2812B data line |
| Wire | 22-24 AWG | For signal connections |
| Power Wire | 18-20 AWG | For power distribution |
| Heat Shrink Tubing | Various sizes | Insulate connections |
| Dupont Connectors | Male/Female | Easy connections |
| Breadboard (optional) | Full or half size | For prototyping |

### Battery System

| Component | Specifications | Notes |
|-----------|---------------|-------|
| 18650 Batteries | 4x cells, 2500-3500mAh | Use protected cells |
| 4x18650 Holder | With 5V USB-A output | Built-in boost converter |
| USB-A to Breadboard Cable | Standard USB cable | Connects battery to power module |

---

## Power Distribution

### Single 5V Rail System

The 4x18650 battery pack provides a clean 5V rail for all components:

```
4x18650 Battery Pack (5V Output)
         |
         v
    Power Switch
         |
         v
  Breadboard Power Module
    (+ and - rails)
         |
         ├──> ESP32-A1S (5V + GND)
         ├──> LED Ring (5V + GND)
         └──> Electromagnet + (5V)
              Electromagnet - via MOSFET
```

### Power Distribution Details

**Advantages of this setup:**
- ✅ Single voltage rail - simple and clean
- ✅ Portable with battery power
- ✅ No voltage converters needed
- ✅ Breadboard rails make connections easy
- ✅ Lower power consumption (5V coil)

**Current Draw Estimates:**
- ESP32-A1S: ~300-500mA
- LED Ring (24 LEDs): 500mA-1.4A (depends on brightness)
- Electromagnet: 1-2A (depends on duty cycle)
- **Total**: 2-4A typical, up to 5A peak

**Battery Life Calculation:**
- 4x 3000mAh cells = ~3000mAh at 5V
- At 2A average: ~1.5 hours runtime
- At 3A average: ~1 hour runtime

---

## ESP32-A1S Connections

### Pin Assignments

| Pin | Function | Connection |
|-----|----------|------------|
| GPIO 22 | PWM Output | MOSFET Gate |
| GPIO 21 | LED Data | WS2812B DIN (through 330Ω) |
| GPIO 36 | Button 1 | Magnet Mode ↑ (Built-in) |
| GPIO 39 | Button 2 | Magnet Mode ↓ (Built-in) |
| GPIO 34 | Button 3 | LED Mode ↑ (Built-in) |
| GPIO 35 | Button 4 | LED Mode ↓ (Built-in) |
| GPIO 32 | Button 5 | EQ Preset ↑ (Built-in) |
| GPIO 33 | Button 6 | EQ Preset ↓ (Built-in) |
| 5V | Power In | From breadboard + rail |
| GND | Ground | From breadboard - rail |

### I2S Audio (Pre-wired on board)

| Pin | Function |
|-----|----------|
| GPIO 27 | I²S BCK (Bit Clock) |
| GPIO 25 | I²S WS (Word Select) |
| GPIO 26 | I²S DATA OUT |
| GPIO 35 | I²S DATA IN (AUX) |
| GPIO 0 | I²S MCLK |

### Power Connection

```
Breadboard (+) Rail  ──> ESP32-A1S 5V Pin
Breadboard (-) Rail  ──> ESP32-A1S GND Pin
```

---

## Electromagnet Circuit

### Complete Electromagnet Wiring

```
           Breadboard (+) Rail (5V)
                     │
                     ├─────────────┐
                     │             │
                 [Flyback Diode]   │
                     │        [Electromagnet]
                     │             │
                     └─────────────┤
                                   │
                              Drain (D)
                                   │
                           [MOSFET Trigger]
                                   │
                              Source (S)
                                   │
                         Breadboard (-) Rail
                                  
ESP32 GPIO 22 ──────────> Gate (G)
```

### Component Details

**MOSFET Trigger Module:**
- **Input**: 3.3V logic from GPIO 22
- **Output**: Switches coil ground (up to 5A)
- **Built-in**: Flyback diode, status LED
- **Recommended**: AOD4184 module or similar
- **Voltage**: 5V coil operation

**Flyback Diode (if external):**
- **Cathode (stripe)** → Coil + (breadboard +5V rail)
- **Anode** → Coil - (MOSFET drain)
- **Purpose**: Protects MOSFET from voltage spikes
- **Required**: Yes, if not built into MOSFET module

**Gate Connection:**
- Connect GPIO 22 directly to MOSFET gate (no resistor needed)
- Some designs use 100Ω resistor, but not required for this build

### MOSFET Module Connection

```
ESP32-A1S                    MOSFET Trigger Module
─────────                    ──────────────────────
GPIO 22  ──────────────────>  Signal In (S/SIG)
GND      ──────────────────>  GND

Breadboard (+) Rail ──────>  V+ or VCC
Breadboard (-) Rail ──────>  GND

MOSFET Trigger Module        Electromagnet
──────────────────────       ─────────────
OUT+ / DRAIN ─────────────>  Coil (-)

Breadboard (+) Rail ──────>  Coil (+)
```

**Status LED on module:**
- Most MOSFET trigger modules have a built-in LED
- LED lights when coil is energized
- Useful for debugging

---

## LED Ring Wiring

### WS2812B Connection

```
Breadboard (+) Rail ─────────> LED Ring VCC/5V
Breadboard (-) Rail ─────────> LED Ring GND
ESP32 GPIO 21 ─[330Ω]──────> LED Ring DIN
```

### Important Notes

**Data Line Resistor:**
- **Required**: 330Ω resistor between GPIO 21 and DIN
- **Purpose**: Level shifting protection, signal integrity
- **Position**: Close to ESP32 output

**Power Considerations:**
- 24 LEDs at full white = ~1.4A
- Keep LED brightness moderate to extend battery life
- Default brightness (100/255) draws ~500mA

**Wiring Tips:**
- Keep data line short (< 12 inches if possible)
- Twist data and ground wires together
- Use same ground reference as ESP32

### LED Ring Pinout

Standard WS2812B rings have 3 connections:

| Pin | Name | Connection |
|-----|------|------------|
| 1 | VCC/5V | Breadboard + rail |
| 2 | GND | Breadboard - rail |
| 3 | DIN | GPIO 21 through 330Ω resistor |

Some rings also have:
- **DOUT**: Data out (for chaining, not used)

---

## Battery Power System

### 4x18650 Battery Pack Setup

```
[18650][18650][18650][18650]  ←─ 4 cells in holder
         |
    [5V Boost Module]           ←─ Built into holder
         |
    [USB-A Output Port]         ←─ 5V output
         |
    [USB Cable]                 ←─ USB-A to breadboard
         |
         v
  [Breadboard Power Module]
    (with master switch)
         |
         +────────> (+) Rails
         |
         └────────> (-) Rails
```

**Charging:**
- Micro-USB charging port on battery pack
- Use 5V USB charger (1A-2A recommended)
- Charge indicator LED on pack

**Wake/Power:**
- Button on battery pack: Wake-up only (activates USB output)
- Master power switch: On breadboard power module
- Press battery wake button, then turn on breadboard switch

### Battery Pack Features

**Typical 4x18650 5V modules include:**
- ✅ Built-in voltage regulation to 5V
- ✅ USB-A output port (5V)
- ✅ Micro-USB charging port (5V input)
- ✅ Protection circuits
- ✅ LED charging/power indicator
- ✅ Wake-up button to activate output
- ✅ 2A-5A output capability

### Power Module Connection

```
4x18650 Pack                Breadboard Power Module
────────────                ───────────────────────
USB-A Output  ──[USB Cable]──> USB Input or VIN/GND
                               (via cut USB cable)

                       Master Switch: ON
                               |
                       (+) Rails distribute to:
                       ├─> ESP32-A1S
                       ├─> LED Ring  
                       └─> Electromagnet

                       (-) Rails (common ground) to:
                       ├─> ESP32-A1S
                       ├─> LED Ring
                       └─> MOSFET Source
```

**Note:** If breadboard power module has USB input, use standard USB-A to Micro-USB cable. Otherwise, cut USB cable and connect red wire to VIN, black to GND.

### Breadboard Power Module

Most breadboard power modules have:
- Dual voltage inputs (we only use one at 5V)
- Two sets of (+) and (-) rails
- **Master power switch** (main on/off control)
- Status LEDs
- Optional USB input port

**Connection:**
```
Battery Pack USB-A ──[USB Cable]──> Power Module USB/VIN Input

Power Module Master Switch: Controls entire system

Power Module creates two rails:
  Rail 1: (+) and (-) 
  Rail 2: (+) and (-) (paralleled)
```

### Power Switch

**System Power Control:**
1. **Battery Wake Button**: Press to activate USB-A output on battery pack
2. **Breadboard Master Switch**: Main on/off for entire FerroWave system

**Power-On Sequence:**
1. Press battery pack wake button (activates 5V output)
2. Turn on breadboard power module master switch
3. System powers on

**Power-Off Sequence:**
1. Turn off breadboard power module master switch
2. Battery pack will auto-sleep after period of inactivity

---

## Wiring Diagram

### Complete System Overview

```
┌──────────────────────────────────────────────────────┐
│         4x18650 Battery Pack (5V USB-A Out)          │
│         Micro-USB In (charging) | Wake Button        │
└────────────┬─────────────────────────────────────────┘
             │
        [USB Cable]
             │
             v
    ┌────────────────────┐
    │  Breadboard Power  │
    │      Module        │
    │  [Master Switch]   │
    └────────────────────┘
         (+)    (-)
          │      │
    ──────┴──────┴──────
    │      │      │
    │      │      └────────────────┤
    │      │                       │
    │      │                       │
    v      v                       v
┌────────────────┐            ┌────────┐
│  ESP32-A1S     │            │ MOSFET │
│  ┌──────────┐  │            │ Module │
│  │ GPIO 22  ├──┼────────────>  Gate  │
│  │ GPIO 21  ├──┼─[330Ω]─┐   └────────┘
│  │          │  │         │       │
│  │   5V     │<─┘         │     Drain
│  │   GND    │<───────────┤       │
│  └──────────┘            │       v
└────────────────┘          │  [Electromagnet]
                            │       │
                            │       v
                        ┌───────────────┐
                        │  LED Ring     │
                        │  (24 LEDs)    │
                        │               │
                        │  DIN  VCC GND │
                        └───┬───┬───┬───┘
                            │   │   │
                            │   │   └───> (-) Rail
                            │   └───────> (+) Rail
                            └───────────> GPIO 21
```

---

## Step-by-Step Assembly

### Step 1: Prepare Battery Pack

1. Insert 4x 18650 batteries (observe polarity!)
2. Press wake button to activate output
3. Verify 5V output at USB-A port with multimeter (if desired)
4. Connect USB cable from battery to breadboard power module

### Step 2: Setup Breadboard Power

1. Connect USB cable from battery pack to breadboard power module
   - If module has USB input, use standard cable
   - Otherwise cut cable and connect red→VIN, black→GND
2. Turn on breadboard master switch
3. Verify 5V on both (+) rails with multimeter
4. Verify ground continuity on both (-) rails

### Step 3: Connect ESP32-A1S

1. Connect ESP32 5V pin to breadboard (+) rail
2. Connect ESP32 GND pin to breadboard (-) rail
3. Power on and verify ESP32 LED lights up
4. If programming, connect USB cable now

### Step 4: Wire MOSFET Module

1. Connect MOSFET module VCC to breadboard (+) rail
2. Connect MOSFET module GND to breadboard (-) rail
3. Connect GPIO 22 directly to MOSFET gate (no resistor)
4. Verify gate voltage is LOW when ESP32 is off

### Step 5: Connect Electromagnet

1. Connect electromagnet (+) to breadboard (+) rail
2. Connect electromagnet (-) to MOSFET DRAIN
3. Connect MOSFET SOURCE to breadboard (-) rail
4. Double-check flyback diode orientation (if external):
   - Cathode (stripe) to (+) rail
   - Anode to coil (-)

### Step 6: Wire LED Ring

1. Solder 330Ω resistor to a wire
2. Connect resistor to GPIO 21
3. Connect other end of resistor to LED DIN
4. Connect LED VCC to breadboard (+) rail
5. Connect LED GND to breadboard (-) rail

### Step 7: Final Checks

1. ✅ All (+) connections go to breadboard (+) rail
2. ✅ All (-) connections go to breadboard (-) rail  
3. ✅ No exposed wire that could short
4. ✅ All connections are secure
5. ✅ Flyback diode is correct orientation (if external)
6. ✅ 330Ω resistor is in LED data line
7. ✅ GPIO 22 connects directly to MOSFET gate
8. ✅ USB cable from battery to breadboard is secure
9. ✅ Breadboard master switch is accessible

---

## Testing

### Pre-Power Test

**Before connecting battery:**

1. Visual inspection of all connections
2. Check for shorts with multimeter:
   - (+) rail to (-) rail should be open (> 1MΩ)
3. Verify no loose wires
4. Check polarity of all components

### Initial Power-On

1. Press battery pack wake button
2. Turn on breadboard master switch
3. **Expected behavior:**
   - ESP32 LED lights up
   - LED ring shows brief startup animation
   - No smoke, strange smells, or heat
4. If anything unexpected, turn off master switch immediately

### Functional Testing

**LED Test:**
1. LED ring should do startup animation
2. Type `c1` in Serial Monitor
3. Rainbow effect should appear
4. Try different modes (c2, c3, etc.)

**Electromagnet Test:**
1. Type `t` in Serial Monitor
2. Should hear/feel magnet activate for 2 seconds
3. MOSFET module LED should light up
4. Check coil for excessive heat

**Audio Test:**
1. Pair via Bluetooth
2. Play music
3. LEDs should react to audio
4. Ferrofluid should move with music

---

## Troubleshooting

### Power Issues

**ESP32 won't power on:**
- Check battery charge level
- Verify power switch is on
- Check breadboard power module connections
- Measure voltage at (+) rail (should be ~5V)

**System resets randomly:**
- Battery may be depleted
- Current draw too high (reduce LED brightness)
- Poor connections causing voltage drops

### Electromagnet Issues

**Coil doesn't activate:**
- Check MOSFET module power (VCC and GND)
- Verify GPIO 22 connection to gate
- Test with `t` command
- Check MOSFET module LED indicator

**Coil gets very hot:**
- Reduce max duty: `d70`
- Reduce sensitivity: `s80`
- Add heatsink to coil
- Improve airflow

### LED Issues

**LEDs don't light:**
- Check LED VCC connection to (+) rail
- Check LED GND connection to (-) rail
- Verify 330Ω resistor in data line
- Try `l255` for maximum brightness

**LEDs flicker or glitch:**
- Check ground connection (must be shared with ESP32)
- Verify 330Ω resistor is present
- Try shorter wires to LED DIN

---

**Project**: [FerroWave](https://github.com/matoslav/FerroWave)  
**Author**: Makarov87  
**Crowdfunded by**: MakerWorld Community
