#case
/*
 * File:   AK-230.c
 * Author: s.mueller
 *
 * Created on 19. Oktober 2013, 17:33
 * 
 * Description:
 * Diese Software ist f�r mehrer Ger�te Versionen der AK230 Klasse.
 * Diese Anschlussk�sten existieren in 230Volt und 24Volt Versionen, mit und
 * ohne Accu f�r die Notstromversorgung.
 * 
 * Funktionsweisen der verschiedenen Typen:
 *     230V ohne Accu -> Alarm sobald Ger�t mit Spannung versorgt wird
 *     230V mit Accu -> Alarm sobald Spannungsversorgung 230V ausf�llt
 *     24V mit Accu -> Alarm sobald Spannungsversorgung 24V ausf�llt
 */

// Alte MCU (Microcontoller)
#include <16f873.h>
// Neue MCU (Microcontoller)
//#include <16F883.h>
#device *=16 ADC=10

#fuses XT,NOWDT,PUT,NOPROTECT,NOCPD,NOBROWNOUT
#use delay(clock=4MHz)   // 4MHz external Clock
//#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, enable=PIN_B1, parity=N, bits=8)

//RTC variables
#define XTAL_FREQUENCY  4000000
#define TIMER1_FREQUENCY (XTAL_FREQUENCY / 4)      // 1 clock tick = 1 instr. cycle = crystal frequency / 4
int32 ticker;
int8 seconds=0;

// Hardware mit Akku? 1 f�r JA und 0 f�r NEIN
#define HARDWARE_MIT_AKKU   1

#define BAT_MONITOR     PIN_A0
#define MAINS_MONITOR   PIN_A2
#define HUPE            PIN_B3
#define LED             PIN_B5
// Default Wert: Anzahl der Sekunden, die Hupe ert�nen soll
#define HUPE_SEKUNDEN   120     

// Counter um die Sekunden auszurechnen
int counter = 0;
// Die Konstante HUPE_SEKUNDEN (default Wert) wird einer Variablen zugewiesen,
// so das wir den Wert sp�ter zur�ck setzen k�nnen 
// (Zum Beispiel im Accu Betrieb, wenn Spannungsversorgung wieder herstestellt wurde)
int hupe_sek = HUPE_SEKUNDEN;
// 0 -> kein Alarm, 1 -> Alarm
int1 alarm_flag = 0;


// https://www.ccsinfo.com/forum/viewtopic.php?t=26177
void RTC_init() {
    ticker = TIMER1_FREQUENCY;                  // Initalize clock counter to number of clocks per second
    setup_timer_1( T1_INTERNAL | T1_DIV_BY_1);  // Initalize 16bit timer1 to interrupt
                                                // exactly every 65536 clock cycles
                                                // (about 76 times per second)
    enable_interrupts(GLOBAL);
    enable_interrupts(INT_TIMER1);
}

/** Handle des Alarms
 * Wird aufgerufen wenn sekunde wechselt, im TIMER1 interrupt TIMER1_isr().
 */
void handle_alarm() {
    if (alarm_flag == 1) {
        output_toggle(LED);

        if (hupe_sek > 0) {
            // HUPE ein.-/ ausschalten
            output_toggle(HUPE);
            hupe_sek--;
            // Diese Pr�fung sorgt daf�r das die Hupe aus bleibt
            if (hupe_sek == 0)
                output_low(HUPE);
        }

        // Counter wieder zur�ck setzten, nachdem dieser Loop durchlaufen wurde
        counter = 0;
    }
}

// Zero drift Real Time Clock (RTC)
// https://www.ccsinfo.com/forum/viewtopic.php?t=26177
#int_TIMER1                               
void TIMER1_isr() {
    ticker -= 65536;
    if (ticker < 65536) {
        ticker += TIMER1_FREQUENCY;
        seconds++;
        handle_alarm();
    }
}

void main() {
    // Timer f�r HUPE und LED
    RTC_init();
    
    // Hupe und LED ausschalten, so dass diese sp�ter gemeinsam starten.
    // Will man beide versetzt starten, dann eines High setzen!
    output_low(LED);
    output_low(HUPE);

    // Main Loop
    while (1) {
        // Kein ACCU vorhanden (Alarm wird sofort eingeschalten.)
        if (!HARDWARE_MIT_AKKU && input(MAINS_MONITOR)) {
            alarm_flag = 1;
        }
            // ACCU vorhanden, Baterie vorhanden (BAT), Haupt Spannungsversorgung AUSGEFALLEN (!MAINS)
            // Allein der Umstand das wir dies messen k�nnen bedeutet 
            // das wir uns im Accubetrieb befinden.
        else if (HARDWARE_MIT_AKKU && input(BAT_MONITOR) && !input(MAINS_MONITOR)) {
            alarm_flag = 1;
        }            
            // Dieser else if Block ist wichtig. Hier landen wir wenn 
            // ACCU vorhanden, Baterie vorhanden (BAT), Haupt Spannungsversorgung WIEDERHERGESTELLT
        else if (HARDWARE_MIT_AKKU && input(BAT_MONITOR) && input(MAINS_MONITOR)) {
            // Reset Alarm (Alarm aus)
            alarm_flag = 0;
            // Reset Hupe Counter
            hupe_sek = HUPE_SEKUNDEN;
            // Guard Hupe und LED aus!
            output_low(LED);
            output_low(HUPE);
        } else {
            // Guard Hupe und LED aus!
            output_low(LED);
            output_low(HUPE);
        }
    }
}