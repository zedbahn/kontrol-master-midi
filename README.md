# Kontrol Master MIDI

USB MIDI firmware for the **Kontrol Master** — a desktop controller originally created by [Touch Innovations](https://www.touchinnovations.com/) in 2016/2017. This project provides updated firmware using a SparkFun Pro Micro (ATmega32U4) to give the hardware a second life as a modern USB MIDI controller.

10 buttons + rotary encoder send MIDI messages over USB, designed for use with BetterTouchTool (BTT) or any MIDI-compatible software.

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
2. Set the rotary knob to **Command 191 (Control Change)**
3. Set **Control Number 1** → **Volume Down**
4. Set **Control Number 2** → **Volume Up**

The key is mapping CC#1 to Volume Down and CC#2 to Volume Up.

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

---

## Building & Uploading

### Step 1: Install Arduino IDE

Download and install from https://www.arduino.cc/en/software

### Step 2: Add SparkFun Board Support

1. Open Arduino IDE
2. Go to **Arduino IDE → Settings** (or **Preferences** on older versions)
3. In **Additional Boards Manager URLs**, add:
   ```
   https://raw.githubusercontent.com/sparkfun/Arduino_Boards/main/IDE_Board_Manager/package_sparkfun_index.json
   ```
4. Click **OK**

### Step 3: Install SparkFun AVR Boards

1. Go to **Tools → Board → Boards Manager**
2. Search for **SparkFun AVR Boards**
3. Click **Install**

### Step 4: Install MIDIUSB Library

1. Go to **Tools → Manage Libraries**
2. Search for **MIDIUSB**
3. Install the library by **Arduino**

### Step 5: Select Board and Port

1. Connect the SparkFun Pro Micro via USB
2. Go to **Tools → Board → SparkFun AVR Boards → SparkFun Pro Micro**
3. Go to **Tools → Processor → ATmega32U4 (5V, 16MHz)**
4. Go to **Tools → Port** and select the port (usually shows as "usbmodem" on macOS)

### Step 6: Upload

1. Open `KontrolMaster_MIDI/KontrolMaster_MIDI.ino`
2. Click the **Upload** button (→ arrow)
3. Wait for "Done uploading" message
