/*
 * main.c
 * A clock for two or more daisy-chained I2C 8x16-segment displays
 *
 * May 2013, Andrew Villeneuve
 */

/*
 * ======== Module Dependencies ========
 */

#include "i2c.h"
#include "max6955.h"
#include "display.h"

/*
 * ======== Standard MSP430 includes ========
 */
#include <msp430g2452.h>

/*
 * ======== Pin Mappings ===========
 */
#define BUTTON_INC BIT1 //Port 2
#define BUTTON_DEC BIT0 //Port 2
#define BUTTON_SET BIT4	//Port 2

#define SET_OPERATE 0
#define SET_YEAR 1
#define SET_WDAY 2
#define SET_MONTH 3
#define SET_DAY 4
#define SET_HOUR 5
#define SET_MIN 6
#define SET_SEC 7

#define DISP_NATURAL 1
#define DISP_BINARY 2
#define DISP_ROMAN 3

/** Variables **/
/* I2C addresses of all attached LED modules, in the order that they're
 * connected. MODULE_COUNT must be defined in display.h
 */
char module_addr[MODULE_COUNT] = { (char)0x65, (char)0x60 };

static const char weekdays[] = "Sun Mon Tue Wed Thu Fri Sat Sun ";
static const char months[] = "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";
static const char daycount[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const char roman_numerals[] = "           I    II   III    IV     V    VI   VII  VIII    IX     X    XI   XII  XIII   XIV    XV   XVI  XVII XVIII   XIX    XX   XXI  XXII XXIII  XXIV   XXV  XXVI XXVIIXXVIII  XXIX   XXX  XXXI XXXIIXXXIII XXXIV  XXXV XXXVIXXXVIIXXXVII XXXIX    XL   XLI  XLII XLIII  XLIV   XLV  XLVI XLVIIXLVIII  XLIX     L    LI   LII  LIII   LIV    LV   LVI  LVII LVIII   LIX";

char set_mode = SET_OPERATE;
char disp_mode = DISP_NATURAL;

int year = 2014;
int month = 6;	//0 - 11, not 1 - 12
int day = 7;	//1 - 31, not 0 - 30
int hour = 14;
int min = 42;
int sec = 23;
int wday = 6;	//0 - 6, 0 is Sunday

void global_max_setreg(char reg, char val) {
	char i;
	for (i = 0; i < MODULE_COUNT; i++) {
		max_setreg(module_addr[i], reg, val);
	}
}

void naturalTime() {
	char str[16];
	char i;
	for (i = 0; i < 16; i++) {
		str[i] = ' ';
	}

	if (set_mode == SET_YEAR) {
		str[0] = (year / 1000) + 0x30;
		str[1] = (year / 100) % 10 + 0x30;
		str[2] = (year / 10) % 10 + 0x30;
		str[3] = (year % 10) + 0x30;
	}

	if ((set_mode == SET_OPERATE) || (set_mode == SET_WDAY)) {
		for (i = 0; i < 4; i++) {
			str[i] = weekdays[(4 * wday) + i];
		}
	}

	if ((set_mode == SET_OPERATE) || (set_mode == SET_MONTH)) {
		for (i = 4; i < 8; i++) {
			str[i] = months[(4 * month) + (i - 4)];	//Fuck you, it's magic
		}
		str[6] = str[6] + 128;		//Enable decimal point following month
	}

	if ((set_mode == SET_OPERATE) || (set_mode == SET_DAY)) {
		str[7] = (day / 10) + 0x30;
		str[8] = (day % 10) + 0x30;
		//str[9] = ' ';
	}

	if ((set_mode == SET_OPERATE) || (set_mode == SET_HOUR)) {
		str[0xa] = (hour / 10) + 0x30;	//0x30 == (char)'0'
		str[0xb] = (hour % 10) + 0xb0;	//0xb0 == '0' + 128...enables decimal point
	}

	if ((set_mode == SET_OPERATE) || (set_mode == SET_MIN)) {
		str[0xc] = (min / 10) + 0x30;
		str[0xd] = (min % 10) + 0xb0;
	}

	if ((set_mode == SET_OPERATE) || (set_mode == SET_SEC)) {
		str[0xe] = (sec / 10) + 0x30;
		str[0xf] = (sec % 10) + 0x30;
	}

	display(str);
}

void binaryTime() {
	char str[16];
	char i;
	for (i = 0; i < 16; i++) {
		str[i] = '0';
	}

	char hour12 = hour;
	if (hour12 > 12) {
		hour12 -= 12;
	}
	for (i = 0; i < 4; i++) {
		if (hour12 & (1 << (3 - i))) { str[i] = '1'; }
	}
	str[3] += 128;

	for (i = 0; i < 6; i++) {
		if (min & (1 << (5 - i))) { str[4 + i] = '1'; }
		if (sec & (1 << (5 - i))) { str[10 + i] = '1'; }
	}
	str[9] += 128;

	display(str);
}

void romanTime() {
	char str[16];
	char i;
	for (i = 0; i < 16; i++) {
		str[i] = ' ';
	}

	char hour12 = hour;
	if (hour12 > 12) {
		hour12 -= 12;
	}
	for (i = 0; i < 4; i++) {
		str[i] = roman_numerals[(hour12 * 6) + i + 2];
	}
	str[3] += 128;

	for (i = 0; i < 6; i++) {
		str[4 + i] = roman_numerals[(min * 6) + i];
		str[10 + i] = roman_numerals[(sec * 6) + i];
	}
	str[9] += 128;

	display(str);
}

//Increment time by x seconds. x = 0 will refactor out-of-range values.
void tick(char x) {
	sec+=x;
	if (sec > 59) {
		sec = 0;
		min+=x;
	}
	if (sec < 0) {
		sec = 59;
	}
	if (min > 59) {
		min = 0;
		hour+=x;
	}
	if (min < 0) {
		min = 59;
	}
	if (hour > 23) {
		hour = 0;
		day+=x;
		wday+=x;
	}
	if (hour < 0) {
		hour = 23;
	}
	if (day > daycount[month]) {
		day = 1;
		month+=x;
	}
	if (day < 1) {
		day = daycount[month];
	}
	if (wday > 6) {
		wday = 0;
	}
	if (wday < 0) {
		wday = 6;
	}
	if (month > 11) {
		month = 0;
		year+=x;
	}
	if (month < 0) {
		month = 11;
	}
}

//Increment the currently selected display field by dir amount
void increment_current(int dir) {
	if (set_mode == SET_OPERATE) {
		disp_mode += dir;
		if (disp_mode > DISP_ROMAN) {
			disp_mode = DISP_NATURAL;
		} else if (disp_mode < DISP_NATURAL) {
			disp_mode = DISP_ROMAN;
		}
	} else {
		if (set_mode == SET_YEAR) { year += dir; }
		if (set_mode == SET_MONTH) { month += dir; }
		if (set_mode == SET_WDAY) { wday += dir; }
		if (set_mode == SET_DAY) { day += dir; }
		if (set_mode == SET_HOUR) { hour += dir; }
		if (set_mode == SET_MIN) { min += dir; }
		if (set_mode == SET_SEC) { sec += dir; }
		tick(0);
	}
}

void main(void) {
	/* MCU Initialization */

    // WDT Setup
	WDTCTL = (WDTPW + WDTHOLD);				//Disable watchdog

	//Clock setup
	BCSCTL1 = CALBC1_1MHZ;		// Set MCLK to factory-calibrated 1Mhz DCO (part 1)
	DCOCTL = CALDCO_1MHZ;		// Set MCLK to factory-calibrated 1Mhz DCO (part 2)
	BCSCTL2 = (SELS);			// SELM=DCO, DIVM=/1, SELS=LFXT1CLK, DIVS=/1, DCOR=Off
	BCSCTL3 = (XCAP0 + XCAP1);	// LFXT1S=32kHz Crystal, XCAP=12.5pF

	//osc_check();				// Block until we've successfully stabilized the oscillator (or error if we cannot).

	//Port setup
	P1DIR = ~(BUTTON_INC);					// Setup port 1 input pins
	P1REN = (SDA + SCL + BIT4);				// Setup port 1 pullup resistor enable
	P1OUT = (SDA + SCL);					// Setup port 1 pullup resistor pullup mode
	P1IFG = 0;								// Clear IRQ flag

    P2DIR = ~(BUTTON_DEC + BUTTON_SET + BUTTON_INC);	// Setup port 2 input pins
	P2REN = (BUTTON_DEC + BUTTON_SET + BUTTON_INC);		// Setup port 2 pullup/pulldown resistor enable
	P2OUT = (BUTTON_DEC + BUTTON_SET + BUTTON_INC);		// Setup port 2 pullup/pulldown resistor pullup mode
	P2IE = (BUTTON_DEC + BUTTON_SET + BUTTON_INC);		// Listen for keypress interrupts
	P2IES |= (BUTTON_DEC + BUTTON_SET + BUTTON_INC);	// IRQ trigger on falling edge
	P2IFG = 0;								// Clear IRQ flag

    // Timer_A Setup - used for RTC tick
    TACCR0 = (32768);						// 1 second period
    TACCTL0 = (CCIE);						// Enable interrupt on TACCR0
    TACTL = (TASSEL1 + MC0 + TACLR + TAIE);	// Use SMCLK, Up mode, clear, enable interrupts

    /* Display driver initialization */
    i2c_init();								//Initialize MSP430 I2C module

    char i;
    for (i = 0; i < MODULE_COUNT; i++) {
    	max_init(module_addr[i]);			//Initialize MAX6955 module
    }

    //main loop
    while(1) {
    	if (disp_mode == DISP_NATURAL) {
    		naturalTime();				//Calculate and render time
    	} else if (disp_mode == DISP_ROMAN) {
    		romanTime();
    	} else {
    		binaryTime();
    	}
		_BIS_SR(GIE + LPM1_bits);	//Go to sleep (LPM1)
    }
}

/* Called with a 1-Hz period by the RTC
 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
	tick(1);
	_BIC_SR_IRQ(LPM1_bits);	//Stay awake after exiting ISR
}

//BUG: This MCU doesn't appear to have a timer A1, but this keeps getting called for unknown reasons
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) {
	TAIV = 0;
}

#pragma vector = NMI_VECTOR
__interrupt void NMI_ISR(void) {
}

//Catch keypresses
#pragma vector=PORT2_VECTOR
__interrupt void keypress_ISR(void) {
	if ((P2IN & BUTTON_SET) == 0) {
		disp_mode = DISP_NATURAL;
		set_mode++;
		if (set_mode > SET_SEC) {
			set_mode = SET_OPERATE;
		}
		_BIC_SR_IRQ(LPM1_bits);	//Stay awake after exiting ISR (in order to force a display refresh)
	} else if ((P2IN & BUTTON_INC) == 0) {
		increment_current(1);
		_BIC_SR_IRQ(LPM1_bits);
	} else if ((P2IN & BUTTON_DEC) == 0) {
		increment_current(-1);
		_BIC_SR_IRQ(LPM1_bits);
	}
	long i;
	for (i = 25000; i > 0; i--) {}	//Poor man's stupid, bad, terrible, awful debouncing
	P1IFG = 0;
	P2IFG = 0;	//Clear IRQ
}

//Catch-all for unused IRQ vectors
#pragma vector=PORT1_VECTOR, WDT_VECTOR, USI_VECTOR, ADC10_VECTOR, COMPARATORA_VECTOR
__interrupt void ISR_trap(void)
{
}

