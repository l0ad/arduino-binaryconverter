/*
Copyright (C) 2012 Sebastien Jean <baz dot jean at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * BIN/DEC/HEX converter using an arduino and a TM1638 module
 * (see project wiki pages for details)
 *
 * The TM1638 module is used as follows:
 *
 * - each of the 8 buttons allows to toggle on/off a binary digit (left button is most significant bit)
 * - the resulting binary value is displayed using the 8 leds (left leds is obviously most significant bit)
 * - the left 4-digits are used to display the corresponding decimal value
 * - the right 4-digits are used to display the corresponding hexadecimal value
 *
 * @author S.jean
 * @date nov. 2012
 */

#include <Arduino.h>

// N.B. This projects embeds for convenience the TM16XX lib (v2.1.3) from the project that can be found at
// http://code.google.com/p/tm1638-library/
#include <TM1638.h>

/**
 * minimum delay for taking into account butoon state change (debounce)
 */
const long DEBOUNCE_DELAY = 250;

/**
 * TM1638 module using data pin 8, clock pin 9 and strobe pin 7 (see TM16XX lib)
 */
TM1638 module(8, 9, 7, true, 3);

/**
 * on/off state of the 8 buttons on the TM1638 module.
 * Right button is most-significant bit. 0 means off, 1 means on.
 */
byte buttonsValues;

/**
 * the decimal value to display
 */
String valueDec;

/**
 * the hexadecimal value to display
 */
String valueHex;

/**
 *  date of last button state change
 */
long debounceTime;

/**
 * utility function used to reflesh the 8-digits display each time a button state change
 */
void refreshDisplay() {
	// turn on leds according to buttons states
	module.setLEDs(buttonsValues & 0xFF);

	// compute decimal value from buttons on/off values
	// N.B. as most significant bit represents the right button, values has to be reversed
	byte value = ((buttonsValues & 0b10000000) >> 7) | ((buttonsValues & 0b01000000) >> 5)
			| ((buttonsValues & 0b00100000) >> 3) | ((buttonsValues & 0b00010000) >> 1)
			| ((buttonsValues & 0b00001000) << 1) | ((buttonsValues & 0b00000100) << 3)
			| ((buttonsValues & 0b00000010) << 5) | ((buttonsValues & 0b00000001) << 7);

	valueDec = String(value);
	valueHex = String(value, HEX);
	valueHex.toUpperCase();

	// clear 8-digits display
	module.clearDisplay();

	//display decimal and hexadecimal strings
	module.setDisplayToString(valueDec, 0x0, 0);
	module.setDisplayToString(valueHex, 0x8, 4);

}

/**
 * Arduino's setup function, called once at startup, after init
 */
void setup() {

	// perform boot-time display

	buttonsValues = module.getButtons();
	debounceTime = millis();
	refreshDisplay();
}

/**
 * Arduino's loop function, called in loop (incredible, isn't it ?)
 */
void loop() {

	// get buttons state (which buttons have been pressed)
	byte newKeys = module.getButtons();

	// if at least one button has been pressed
	if (newKeys != 0) {
		// ensure debounce
		if ((millis() - debounceTime) > DEBOUNCE_DELAY) {
			debounceTime = millis();
			// change on/off state of pressed buttons
			buttonsValues ^= newKeys;
			// and refresh display
			refreshDisplay();
		}
	}
}

