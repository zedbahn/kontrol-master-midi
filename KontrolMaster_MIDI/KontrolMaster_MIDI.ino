/*
 * Kontrol Master - USB MIDI Controller
 * For SparkFun Pro Micro (ATmega32U4)
 *
 * Hardware:
 * - 10 buttons with common ground
 * - 1 rotary encoder
 *
 * Created: 2016-2017
 * Updated for USB-MIDI: 2026
 */

#include <MIDIUSB.h>

// ===== PIN DEFINITIONS =====

// Rotary Encoder
#define ENCODER_A    0   // RX1/D0 - Green wire
#define ENCODER_B    1   // TX0/D1 - Yellow wire

// Button pins (10 buttons with common ground)
const int buttonPins[10] = {
  9,   // Orange1 - Purple wire
  10,  // Orange2 - Orange wire
  8,   // White1 - Black wire
  6,   // White2 - Grey wire
  7,   // White3 - White wire
  18,  // Yellow1 - Yellow wire (A0)
  19,  // Yellow2 - Brown wire (A1)
  16,  // Red - Red wire
  14,  // Blue - Blue wire
  15   // Green - Green wire
};

// ===== MIDI CONFIGURATION =====

#define MIDI_CHANNEL 1  // MIDI channel (1-16)

// MIDI note numbers for each button (C3 to A3)
const int buttonNotes[10] = {
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57  // C3, C#3, D3, D#3, E3, F3, F#3, G3, G#3, A3
};

// MIDI CC for encoder (two separate CCs so BTT can distinguish direction)
#define ENCODER_CC_UP   1   // CC#1 for clockwise
#define ENCODER_CC_DOWN 2   // CC#2 for counter-clockwise

// ===== STATE VARIABLES =====

// Button states
bool buttonStates[10];
bool lastButtonStates[10];
unsigned long lastDebounceTime[10];
const unsigned long debounceDelay = 20;  // 20ms debounce

// Encoder states
volatile int encoderPos = 0;
volatile int lastEncoderPos = 0;
int lastEncoded = 0;


// ===== SETUP =====

void setup() {
  // Initialize button pins as INPUT_PULLUP (buttons connect to GND)
  for (int i = 0; i < 10; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttonStates[i] = HIGH;
    lastButtonStates[i] = HIGH;
    lastDebounceTime[i] = 0;
  }

  // Initialize encoder pins
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  // Attach interrupts for encoder (for smooth rotation detection)
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), updateEncoder, CHANGE);

  // Small delay for startup
  delay(100);
}

// ===== MAIN LOOP =====

void loop() {
  handleButtons();
  handleEncoder();
}

// ===== BUTTON HANDLING =====

void handleButtons() {
  for (int i = 0; i < 10; i++) {
    int reading = digitalRead(buttonPins[i]);

    // Check if state changed and debounce
    if (reading != lastButtonStates[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonStates[i]) {
        buttonStates[i] = reading;

        // Button pressed (LOW because of pullup with GND connection)
        if (buttonStates[i] == LOW) {
          sendNoteOn(MIDI_CHANNEL, buttonNotes[i], 127);
        } else {
          sendNoteOff(MIDI_CHANNEL, buttonNotes[i], 0);
        }
      }
    }

    lastButtonStates[i] = reading;
  }
}

// ===== ENCODER HANDLING =====

void updateEncoder() {
  int MSB = digitalRead(ENCODER_A);
  int LSB = digitalRead(ENCODER_B);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
    encoderPos++;
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
    encoderPos--;
  }

  lastEncoded = encoded;
}

void handleEncoder() {
  if (encoderPos != lastEncoderPos) {
    int delta = encoderPos - lastEncoderPos;

    if (delta > 0) {
      // Clockwise: pulse CC#1 (0 then 127) so BTT always sees a change
      sendControlChange(MIDI_CHANNEL, ENCODER_CC_UP, 0);
      sendControlChange(MIDI_CHANNEL, ENCODER_CC_UP, 127);
    } else {
      // Counter-clockwise: pulse CC#2 (0 then 127)
      sendControlChange(MIDI_CHANNEL, ENCODER_CC_DOWN, 0);
      sendControlChange(MIDI_CHANNEL, ENCODER_CC_DOWN, 127);
    }

    lastEncoderPos = encoderPos;
  }
}


// ===== MIDI FUNCTIONS =====

void sendNoteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | (channel - 1), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
}

void sendNoteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | (channel - 1), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
}

void sendControlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | (channel - 1), control, value};
  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}
