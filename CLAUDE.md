# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

USB MIDI controller firmware for SparkFun Pro Micro (ATmega32U4). Converts hardware inputs (10 buttons + rotary encoder) into USB MIDI messages.

## Build & Upload

This is an Arduino sketch. Use Arduino IDE or arduino-cli:

```bash
# Compile
arduino-cli compile --fqbn sparkfun:avr:promicro KontrolMaster_MIDI

# Upload (replace /dev/cu.usbmodem* with actual port)
arduino-cli upload -p /dev/cu.usbmodem* --fqbn sparkfun:avr:promicro KontrolMaster_MIDI
```

**Required library:** MIDIUSB (install via Arduino Library Manager or `arduino-cli lib install MIDIUSB`)

**Board setup:** SparkFun AVR Boards package required (`arduino-cli core install sparkfun:avr`)

## Architecture

Single-file firmware (`KontrolMaster_MIDI/KontrolMaster_MIDI.ino`) with these sections:

- **PIN DEFINITIONS**: Hardware pin mappings for encoder (pins 0-1) and 10 buttons
- **MIDI CONFIGURATION**: Channel, note assignments (C3-A3), and CC numbers for encoder
- **STATE VARIABLES**: Button debounce tracking and encoder position
- **SETUP**: Pin initialization with INPUT_PULLUP, interrupt attachment for encoder
- **MAIN LOOP**: Calls `handleButtons()` and `handleEncoder()` each cycle
- **HANDLERS**: Button polling with 20ms debounce; encoder via interrupt-driven Gray code decoding
- **MIDI FUNCTIONS**: `sendNoteOn()`, `sendNoteOff()`, `sendControlChange()` wrappers for MIDIUSB

## Key Implementation Details

- Buttons use INPUT_PULLUP with common ground (LOW = pressed)
- Encoder uses hardware interrupts on both pins for responsive rotation detection
- Encoder sends separate CCs for each direction (CC#1 up, CC#2 down) with 0→127 pulses to ensure detection by MIDI software
- All MIDI messages include immediate `MidiUSB.flush()` calls

## Configuration

Modify these constants to change behavior:
- `MIDI_CHANNEL`: Output channel (default 1)
- `buttonNotes[]`: MIDI note numbers for each button
- `ENCODER_CC_UP/DOWN`: CC numbers for encoder directions
- `buttonPins[]`: Hardware pin assignments
