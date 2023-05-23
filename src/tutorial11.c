#include <stdint.h>
#include <stdio.h>
#include "qutyio.h"

/***
 * Tutorial 11: Introduction
 *
 * In this week's tutorial you will implement a serial command parser
 * which accepts input from a user via the UART interface and responds
 * to a number of commands.
 *
 * Implementing a serial interface is a core requirement for Assessment 2,
 * and use of a state machine is the preferred way to achieve the required
 * behavior. Refer to the Assessment 2 functional specifications.
 */

typedef enum
{
	START,
	ESCAPE,
	CMD,
	CMD_ON,
	CMD_OFF,
	CMD_SET,
	VAL,
	CHECK,
	CHECK_PASS,
	ACK,
	NACK
} sp_state;

int main(void)
{

	display_init();
	display_hex(0);

	serial_init();

	sp_state state = START;

	/***
	 * Ex 11.0
	 *
	 * The state machine shown in "state_machine_tut11.png" implements a serial
	 * command parser with the following functionality:
	 *   Start character:  'a'
	 *   Escape character: 'b'
	 *   Commands:
	 *     'c' turn on display
	 *     'd' turn off display
	 *     'e' display payload byte as hex
	 *   Command 'e' is followed by a payload byte of one character and a checksum
	 *   byte which should be equal to the payload byte plus one.
	 *   On receipt of a valid command the command parser responds with "ACK\n".
	 *   On receipt of an invalid command (including a failed checksum) the
	 *   command parser responds with "NACK\n".
	 *
	 * Your task is to implement this serial command parser. We have already
	 * declared an enum with all of the states of the command parser for you,
	 * and performed all initialisation required above. You can use the stdio
	 * functions to access the serial interface (getchar(), printf() etc.) and
	 * the functions provided by "qutyio.h" to control the display (display_on(),
	 * display_off(), display_hex()).
	 *
	 * Examples:
	 *   User enters: abc
	 *   Response: ACK\n
	 *   Result: Display turns on
	 *
	 *   User enters: abb
	 *   Response: NACK\n
	 *   Result: No action taken
	 *
	 *   User enters: aaabd
	 *   Response: ACK\n
	 *   Result: Display turns off
	 *
	 *   User enters: abeAB
	 *   Response: ACK\n
	 *   Result: Display shows "41" (if on)
	 *
	 *   User enters: abeAA
	 *   Response: NACK\n
	 *   Result: No action taken
	 */

	volatile uint8_t cmd = 0x00;
	volatile uint8_t payload = 0x00;
	volatile uint8_t x = 0x00;
	volatile uint8_t y = 0x00;

	while (1)
	{
		switch (state) {
			case START:
				if (getchar() == 'a') {
					state = ESCAPE;
				}
				break;

			case ESCAPE:
				if (getchar() == 'b') {
					state = CMD;
				} else {
					state = START;
				}
				break;


			case CMD:
				cmd = getchar();
				switch (cmd) {
					case 'c':
						state = CMD_ON;
						break;
					case 'd':
						state = CMD_OFF;
						break;
					case 'e':
						state = CMD_SET;
						break;
					default:
						state = NACK;
						break;
				}
				break;

			case CMD_ON:
				display_on();
				state = ACK;
				break;

			case CMD_OFF:
				display_hex(0);
				display_off();
				state = ACK;
				break;

			case CMD_SET:
				payload = getchar();
				state = VAL;
				break;

			case VAL:
				x = payload;
				payload = getchar();
				state = CHECK;
				break;

			case CHECK:
				y = payload;
				if (y == (x + 1)){
					state = CHECK_PASS;
				} else if (y != (x +1)) {
					state = NACK;
				}
				break;

			case CHECK_PASS:
				display_hex(x);
				state = ACK;
				break;

			case ACK:
				printf("ACK\n");
				state = START;
				break;

			case NACK:
				printf("NACK\n");
				state = START;
				break;

			default:
				state = START;
				display_off();
				display_hex(0);
				break;
			} // end switch

	} // end while

} // end main()