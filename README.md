# Kontrol Master MIDI

USB MIDI controller using SparkFun Pro Micro. 10 buttons + rotary encoder send MIDI messages over USB, designed for use with BetterTouchTool (BTT) or any MIDI-compatible software.

## Hardware

- SparkFun Pro Micro (ATmega32U4)
- 10 momentary push buttons (active low, common ground)
- 1 rotary encoder

## Button → MIDI Mapping

All buttons send on **MIDI Channel 1** with velocity 127.

Buttons are named by color. Where multiple buttons share a color (Orange, White, Yellow), they are numbered left to right (e.g., White 1, White 2, White 3).

| Button Color | Wire Color | Pin | MIDI Note | Note Name |
|--------------|------------|-----|-----------|-----------|
| Orange 1     | Purple     | 9   | 48        | C3        |
| Orange 2     | Orange     | 10  | 49        | C#3       |
| White 1      | Black      | 8   | 50        | D3        |
| White 2      | Grey       | 6   | 51        | D#3       |
| White 3      | White      | 7   | 52        | E3        |
| Yellow 1     | Yellow     | A0  | 53        | F3        |
| Yellow 2     | Brown      | A1  | 54        | F#3       |
| Red          | Red        | 16  | 55        | G3        |
| Blue         | Blue       | 14  | 56        | G#3       |
| Green        | Green      | 15  | 57        | A3        |

## Encoder → MIDI Mapping

The rotary encoder sends Control Change (CC) messages on **MIDI Channel 1**:

| Direction         | CC Number | Behavior              |
|-------------------|-----------|----------------------|
| Clockwise (right) | CC#1      | Pulses 0 → 127       |
| Counter-clockwise | CC#2      | Pulses 0 → 127       |

Each detent sends a single pulse (0 then 127) so BTT detects every click.

---

## MIDI Monitor (Debugging Tool)

MIDI Monitor is a free macOS app that displays all incoming MIDI messages — useful for verifying your controller is working and seeing exactly what data each button/encoder sends.

**Install:**
- Download from: https://www.snoize.com/midimonitor/
- Or install via Homebrew: `brew install --cask midi-monitor`

**Usage:**
1. Open MIDI Monitor
2. Go to **Sources** menu → check your device ("SparkFun Pro Micro" or similar)
3. Press buttons or turn the encoder — messages appear in real-time

**What you'll see:**
- Button press: `Note On, Channel 1, Note 55, Velocity 127`
- Button release: `Note Off, Channel 1, Note 55, Velocity 0`
- Encoder clockwise: `Control, Channel 1, Controller 1, Value 0` then `Value 127`
- Encoder counter-clockwise: `Control, Channel 1, Controller 2, Value 0` then `Value 127`

Use this to confirm your wiring and troubleshoot if BTT isn't responding to inputs.

---

## BetterTouchTool Configuration

### Setting Up MIDI in BTT

1. Open BTT Preferences
2. Go to **Automations & Named & Other Triggers** in the left sidebar
3. Click **+** → **MIDI Trigger**
4. Your controller should appear as "SparkFun Pro Micro" or similar USB MIDI device

### Configuring the Encoder for Volume Control

1. Add a new MIDI trigger
2. Turn the encoder **clockwise** one click — BTT captures: `Control Change, Channel 1, CC#1`
3. Set the action to **Control Volume → Volume Up** (or use AppleScript for finer control)
4. Repeat for counter-clockwise: `CC#2` → **Volume Down**

**For smoother volume steps**, use this AppleScript action instead:

```applescript
-- Volume Up (for CC#1)
set volume output volume ((output volume of (get volume settings)) + 5)
```

```applescript
-- Volume Down (for CC#2)
set volume output volume ((output volume of (get volume settings)) - 5)
```

### Configuring a Button to Show/Hide an Application

Example: Red button (Note 55 / G3) toggles Spotify

1. Add a new MIDI trigger
2. Press the **Red** button — BTT captures: `Note On, Channel 1, Note 55`
3. Set the action to one of these options:

**Option A: Simple toggle (recommended)**
- Action: **Controlling Other Applications → Toggle Application Visibility**
- Select: Spotify (or any app)

**Option B: AppleScript for more control**
```applescript
tell application "System Events"
    if (name of processes) contains "Spotify" then
        set visible of process "Spotify" to not (visible of process "Spotify")
        tell application "Spotify" to activate
    else
        tell application "Spotify" to activate
    end if
end tell
```

**Option C: Launch or bring to front**
- Action: **Controlling Other Applications → Launch Application / Open File / Start Apple Script**
- This opens the app if closed, or brings it to front if running

### Quick Reference: BTT Trigger Values

| Physical Input | BTT Trigger Type | Channel | Value |
|----------------|------------------|---------|-------|
| Orange 1       | Note On          | 1       | 48    |
| Orange 2       | Note On          | 1       | 49    |
| White 1        | Note On          | 1       | 50    |
| White 2        | Note On          | 1       | 51    |
| White 3        | Note On          | 1       | 52    |
| Yellow 1       | Note On          | 1       | 53    |
| Yellow 2       | Note On          | 1       | 54    |
| Red            | Note On          | 1       | 55    |
| Blue           | Note On          | 1       | 56    |
| Green          | Note On          | 1       | 57    |
| Encoder CW     | Control Change   | 1       | CC#1  |
| Encoder CCW    | Control Change   | 1       | CC#2  |

---

## Building & Uploading

Requires Arduino IDE or arduino-cli with:
- SparkFun AVR Boards package
- MIDIUSB library

```bash
arduino-cli compile --fqbn sparkfun:avr:promicro KontrolMaster_MIDI
arduino-cli upload -p /dev/cu.usbmodem* --fqbn sparkfun:avr:promicro KontrolMaster_MIDI
```
