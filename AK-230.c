 /*
 * File:   AK-230.c
 * Author: s.mueller
 *
 * Created on 19. Oktober 2013, 17:33
 */

// Alte MCU
//#include <16f873.h>
// Neue MCU
#include <16F883.h>
#device *=16 ADC=10


#fuses XT,NOWDT,PUT,NOPROTECT,NOCPD,NOBROWNOUT
#use delay(clock=4MHz)   // 4MHz external Clock
#use rs232(baud=19200, xmit=PIN_C6, rcv=PIN_C7, enable=PIN_B1, parity=N, bits=8)

#define BAT_MONITOR     PIN_A0
#define MAINS_MONITOR   PIN_A2
#define HUPE            PIN_B3
#define LED             PIN_B5
#define HUPE_SEKUNDEN   120     // Anzahl der Sekunden, die Hupe ertönen soll

int counter = 0;           // Counter um die Sekunden auszurechnen
int hupe_sek = HUPE_SEKUNDEN *2;
int1 alarm_flag = 0;           // 0 -> kein Alarm, 1 -> Alarm


/** Interrupt Callback Funktion
 * Wird aufgerufen wenn TIMER0 überläuft.
 */
#INT_TIMER0
void TIMER0_isr()
{
    if (alarm_flag == 1) {
        // Interrupt 15 mal aufgerufen == 1 Sekunde vergangen
        if (counter++ == 15) {
            // Hupe betätigen/ ausschalten
            output_toggle(LED);

            if (hupe_sek > 0 ) {
                output_toggle(HUPE);
                hupe_sek--;
                // Diese Prüfung sorgt dafür das die Hupe aus bleibt
                if (hupe_sek == 0)
                    output_low(HUPE);
            }

            // Counter wieder zurück setzten, nachdem dieser Loop durchlaufen wurde
            counter = 0;
        }
    }
}

// Initalisiert den Timer
void init_timer() {
    setup_timer_0(T0_INTERNAL | T0_DIV_128);
    enable_interrupts(GLOBAL);
    enable_interrupts(INT_TIMER0);
    //set_timer0(0);
}


void main()
{
//    printf ("AK-230-AKKU-HU\r\n");
    // Timer for Hupe and LED
    init_timer();
    
    // Hupe und LED ausschalten, so dass diese später gemeinsam starten.
    // Will man beide versetzt starten, dann eines High setzen!
    output_low(LED);
    output_low(HUPE);

    alarm_flag = 1;
    
   // Main Loop
    while(1) {
        // No MAINS and BAT_MONITOR is up -> we run on accu
        if (!input (MAINS_MONITOR) && input (BAT_MONITOR))  {
//            printf ("Alarm Flag set!\r\n");
            alarm_flag = 1;
        } else {
//            printf ("Alarm Flag unset!\r\n");
            alarm_flag = 0;
            // Hupe und LED aus, kein Alarm
            output_low(LED);
            output_low(HUPE);
        }
    }
}