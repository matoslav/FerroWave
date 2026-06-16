# Old firmware has been retired

The previous `FerroWave.ino` and `FerroWave_fixed.ino` have been merged into a
single canonical file:

**`firmware/FerroWave.ino`**

That one file is now the only firmware you need. It already includes every fix
the community found, so there is nothing separate to flash.

## If you were using the old build

The earlier firmware read the onboard buttons on the wrong pins and put the LED
ring data line on GPIO 23, which is actually the onboard KEY4 button. That caused
two common problems:

- buttons repeating or sticking on their own, and
- the LED ring staying dark or showing random/incorrect colors.

The canonical `FerroWave.ino` fixes both. The key hardware change to be aware of:

- **LED ring DATA wire goes to GPIO 21** (it used to be documented as GPIO 23).
- MOSFET gate stays on GPIO 22.
- The onboard KEY buttons are hardwired and handled correctly in software now.

Full details are in the comment header at the top of `FerroWave.ino`.

## Why there is no `FerroWave_fixed.ino` anymore

Having two `.ino` files in the same folder was confusing, and Arduino tries to
compile every `.ino` in a sketch folder together, which could cause build errors.
So the "fixed" build simply became the main `FerroWave.ino`.
