/*
 * scicalci.c
 *
 * Created: 23-07-2023 11:01:44
 * Author : pradu
 */ 

#define F_CPU 16000000UL	// Define the clock frequency is 16MHz

// Header Files
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LCD PORTC	// LCD Data port connected to PORTC
#define RS 0		// Register Selector
#define RW 1		// Read Write Mode
#define EN 2		// Enable

// Function declarations
void LCD_init();
void LCD_command(unsigned char cmd);
void LCD_data(unsigned char data);
void LCD_string(char* str);
char keypad_scan();

int main()
{
	// Set up ports
	DDRA = 0x0F;    // Rows as O/P, Columns as I/P for 4*4 Matrix keypad
	LCD_init();		// Initialize LCD
	
	// Variables for calculator operation
	char key;
	char operand1[16];
	char operand2[16];
	char result_str[16];
	char operator='\0';
	int result='\0';

	LCD_command(0x01);		// Clear LCD screen

	while (1)
	{
		// Scan keypad for input
		key = keypad_scan();

		// If a numeric key is pressed
		if (key >= '0' && key <= '9')
		{
			if (operator == '\0')
			{
				// Append digit to the first operand
				operand1[strlen(operand1)] = key;
				_delay_ms(200);		// De-bounce delay
				LCD_data(key);
			}
			else
			{
				// Append digit to the second operand
				operand2[strlen(operand2)] = key;
				_delay_ms(200);		// De-bounce delay
				LCD_data(key);
			}
		}
		
		// If an operator key is pressed
		else if (key == '+' || key == '-' || key == '*' || key == '/')
		{
			// Set the operator
			operator = key;
			_delay_ms(200);		// De-bounce delay
			// Move cursor to the second line of the LCD
			LCD_command(0xC0);
			LCD_data(key);
		}
		// If the equals key is pressed
		else if (key == '=')
		{
			// Null-terminate the operands
			operand1[strlen(operand1)] = '\0';
			operand2[strlen(operand2)] = '\0';

			// Perform the operation
			switch (operator)
			{
				case '+':
				result = atoi(operand1) + atoi(operand2);
				break;
				
				case '-':
				result = atoi(operand1) - atoi(operand2);
				break;
				
				case '*':
				result = atoi(operand1) * atoi(operand2);
				break;
				
				case '/':
				result = atoi(operand1) / atoi(operand2);
				break;
				
			}
			LCD_command(0x01);		// Clear the LCD screen
			
			// Display the result on the LCD
			sprintf(result_str, "ANS: %d", result);
			LCD_string(result_str);

			// Reset variables
			memset(operand1, 0, sizeof(operand1));
			memset(operand2, 0, sizeof(operand2));
			operator = '\0';

			_delay_ms(5000);		// Wait for a moment
			LCD_command(0x01);		// Clear the LCD screen
		}
	}
}

void LCD_init()
{
	DDRC = 0xFF;		// define LCD PORTC as output port
	DDRD |= (1<<RS) | (1<<RW) | (1<<EN);		// Define RS , RW and EN as output
	
	PORTD &= ~(1<<EN);
	LCD_command(0x38);  // 2 lines, 5x7 matrix
	LCD_command(0x0E);  // Display on, cursor on
	LCD_command(0x01);  // Clear display
	LCD_command(0x80);	// Forcing cursor to beginning of first line
	_delay_ms(2);
	char str1[]="Developed by: ";
	char str2[]="Pradumn Kumar Bind ";
	
	for (int i = 0; str1[i] != '\0'; i++)
	{
		LCD_data(str1[i]);
		_delay_ms(40);
	}
	LCD_command(0xC0);	// Forcing cursor to beginning of second line
	for (int i = 0; str2[i] != '\0'; i++)
	{
		LCD_data(str2[i]);
		_delay_ms(150);
	}
	_delay_ms(5000);
	LCD_command(0x01);  // Clear display
}

void LCD_command(unsigned char cmd)
{
	LCD = cmd;
	PORTD &= ~(1 << RS);  // RS = 0
	PORTD &= ~(1 << RW);  // RW = 0
	PORTD |= (1 << EN);   // EN = 1
	_delay_ms(1);		// Low to High pulse for EN
	PORTD &= ~(1 << EN);  // EN = 0
}

void LCD_data(unsigned char data)
{
	LCD = data;
	PORTD |= (1 << RS);   // RS = 1
	PORTD &= ~(1 << RW);  // RW = 0
	PORTD |= (1 << EN);   // EN = 1
	_delay_ms(1);
	PORTD &= ~(1 << EN);  // EN = 0
}

void LCD_string(char* str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
	{
		LCD_data(str[i]);
	}
}

char keypad_scan()
{
	PORTA = 0x0F;
	while (1)
	{
		if (PINA != 0x0F)
		{
			_delay_ms(20);
			if (PINA != 0x0F)
			{
				while (PINA != 0x0F)
				{
					// Check for key in the 1st row
					PORTA = 0xFE;
					if (PINA != 0xFE)
					{
						switch (PINA)
						{
							case 0xEE: return '7';
							case 0xDE: return '8';
							case 0xBE: return '9';
							case 0x7E: return '/';
						}
					}

					// Check for key in the 2nd row
					PORTA = 0xFD;
					if (PINA != 0xFD)
					{
						switch (PINA)
						{
							case 0xED: return '4';
							case 0xDD: return '5';
							case 0xBD: return '6';
							case 0x7D: return '*';
						}
					}

					// Check for key in the 3rd row
					PORTA = 0xFB;
					if (PINA != 0xFB)
					{
						switch (PINA)
						{
							case 0xEB: return '1';
							case 0xDB: return '2';
							case 0xBB: return '3';
							case 0x7B: return '-';
						}
					}

					// Check for key in the 4th row
					PORTA = 0xF7;
					if (PINA != 0xF7)
					{
						switch (PINA)
						{
							case 0xE7: LCD_command(0x01);	break;	// clear the screen
							case 0xD7: return '0';
							case 0xB7: return '=';
							case 0x77: return '+';
						}
					}
				}
			}
		}
	}
}