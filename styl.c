/*
 * styl - Nano Stylophone for attiny13a
 * Copyright (C) 2011  Bob Clough <bob@clough.me>
 * Copyright (C) 2011  Charles Yarnold <charlesyarnold@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define F_CPU 8000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1) 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x)) 

#include "notes.h"

void playnote (uint8_t a)
{
	OCR0A=a;
	OCR0B=a/2;
    //OCR0B=a;
}

void dontplay ()
{
	playnote(0);
}

void pwm_init (void)
{
	DDRB |= BIT(1);
	TCCR0A = BIT(COM0B1);   // Clear OC0A on Compare Match
	TCCR0A |= BIT(WGM00);   // PWM mode, 8 bit 
	TCCR0B = BIT(WGM02);	// TOP = OCRA
	TCCR0B |= BIT(CS01) | BIT(CS00); // 64x prescaler
}

void adc_init (void)
{
	ADMUX  = BIT(MUX1);
	ADCSRA = BIT(ADEN);
}

int adc_read ()
{
      ADCSRA |= BIT(ADSC);
      while(bit_get(ADCSRA, BIT(ADSC)) !=0){};
      return ADC; 
}

int main(void)
{
	bit_set(DDRB, BIT(3));
    bit_set(DDRB, BIT(2));
	bit_clear(PORTB, BIT(3));
	bit_set(PORTB, BIT(2));
	pwm_init();
	adc_init();
    int recording;
    int saved;
    int playing;
    int reading;
    int noteSlot = 0;
	while(1)
	{
        if(playing)
        {
            // GET NOTE
            reading = eeprom_read_byte ((uint8_t*)noteSlot);
            reading = reading * 4;
            //reading = 900;
        }
        else
        {
            // READ ADC
            reading = adc_read();
        }


		if (reading < 128)
		{
			bit_clear(PORTB, BIT(3));
			//bit_clear(PORTB, BIT(2));
			dontplay();

            if(recording){
                _delay_ms(10);
                if(adc_read()==reading)
                {
                    saved = 0;
                }
            }
		}
		else
		{
            if(recording && !saved)
            {
                // RECORD NOTE
                int readingLess = reading / 4;
                eeprom_write_byte ((uint8_t *)noteSlot, (uint8_t)readingLess);

                noteSlot++;
                if(noteSlot>60)
                {
                    recording = 0;
                    noteSlot = 0;
                    bit_flip(PORTB, BIT(2));
                }
                else
                {
                    saved = 1;
                }
            }
            
            
			bit_set(PORTB, BIT(3));
            if ((reading <= 310)) // RECORD
			{
                // DEBOUNCE
                _delay_ms(50);
                if(adc_read()==reading)
                {
                    // Debounce ok, toggle recording mode
                    recording = !recording;
                    if(!recording)
                    {
                        noteSlot++;
                        eeprom_write_byte ((uint8_t *)noteSlot, (uint8_t)0);
                    }
                    noteSlot = 0;
                    bit_flip(PORTB, BIT(2));

                    while(adc_read()>128){}
                }
			}
            else if ((reading <= 340)) // PLAY
			{
                if(!recording){
                    _delay_ms(2);
                    int secondRead = adc_read();
                    if(secondRead==reading || secondRead==reading+1 || secondRead==reading-1 || secondRead==reading+2 || secondRead==reading-2)
                    {
                        // Debounce ok, start playing
                        playing = 1;
                        noteSlot = 0;
                        bit_clear(PORTB, BIT(2));
                    }
                        while(adc_read()>128){}
                }
			}
			else if ((reading <= 359))
			{
				playnote(A1);
			}
			else if (reading <= 374)
			{
				playnote(AS1);
			}
			else if (reading <= 387)
			{
				playnote(B1);
			}
			else if (reading <= 402)
			{
				playnote(C1);
			}
			else if (reading <= 418)
			{
				playnote(CS1);
			}
			else if (reading <= 436)
			{
				playnote(D1);
			}
			else if (reading <= 455)
			{
				playnote(DS1);
			}
			else if (reading <= 477)
			{
				playnote(E1);
			}
			else if (reading <= 500)
			{
				playnote(F1);
			}
			else if (reading <= 525)
			{
				playnote(FS1);
			}
			else if (reading <= 554)
			{
				playnote(G1);
			}
			else if (reading <= 586)
			{
				playnote(GS1);
			}
			else if (reading <= 621)
			{
				playnote(A2);
			}
			else if (reading <= 661)
			{
				playnote(AS2);
			}
			else if (reading <= 707)
			{
				playnote(B2);
			}
			else if (reading <= 760)
			{
				playnote(C2);
			}
			else if (reading <= 821)
			{
				playnote(CS2);
			}
			else if (reading <= 892)
			{
				playnote(D2);
			}
			else if (reading <= 977)
			{
				playnote(DS2);
			}
			else if (reading > 977)
			{
				playnote(E2);
			}
		}

        if(playing)
        {
            // wait for time
            _delay_ms(200);
            dontplay();
            _delay_ms(200);
            noteSlot++;
            if(noteSlot>60 || reading==0)
            {
                noteSlot = 0;
                playing = 0;
                bit_set(PORTB, BIT(2));
            }
        }
        else
        {
            // loop quickly
		    _delay_us(255);
        }
	}
}

