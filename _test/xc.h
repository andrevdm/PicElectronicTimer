#ifndef __XC_H__
#define __XC_H__


void eeprom_write( int a, int v );
int eeprom_read( int a );
void __delay_ms( int i );
void __delay_us( int i );

#define bit short
extern int PORTA;
extern int PORTB;
extern int PORTC;
extern int PORTD;
extern int PORTE;

typedef union {
    struct {
        short LATA0                  :1;
        short LATA1                  :1;
        short LATA2                  :1;
        short LATA3                  :1;
        short LATA4                  :1;
        short LATA5                  :1;
        short LATA6                  :1;
    };
    struct {
        short LA0                    :1;
    };
    struct {
        short                        :1;
        short LA1                    :1;
    };
    struct {
        short                        :2;
        short LA2                    :1;
    };
    struct {
        short                        :3;
        short LA3                    :1;
    };
    struct {
        short                        :4;
        short LA4                    :1;
    };
    struct {
        short                        :5;
        short LA5                    :1;
    };
    struct {
        short                        :6;
        short LA6                    :1;
    };
    struct {
        short                        :7;
        short LA7                    :1;
    };
    struct {
        short                        :7;
        short LATA7                  :1;
    };
} LATAbits_t;
LATAbits_t LATAbits;

typedef union {
    struct {
        short LATE0                  :1;
        short LATE1                  :1;
        short LATE2                  :1;
    };
    struct {
        short LE0                    :1;
    };
    struct {
        short                        :1;
        short LE1                    :1;
    };
    struct {
        short                        :2;
        short LE2                    :1;
    };
    struct {
        short                        :3;
        short LE3                    :1;
    };
    struct {
        short                        :4;
        short LE4                    :1;
    };
    struct {
        short                        :5;
        short LE5                    :1;
    };
    struct {
        short                        :6;
        short LE6                    :1;
    };
    struct {
        short                        :7;
        short LE7                    :1;
    };
} LATEbits_t;
LATEbits_t LATEbits;


typedef union {
    struct {
        unsigned RA0                    :1;
        unsigned RA1                    :1;
        unsigned RA2                    :1;
        unsigned RA3                    :1;
        unsigned RA4                    :1;
        unsigned RA5                    :1;
        unsigned RA6                    :1;
    };
    struct {
        unsigned AN0                    :1;
        unsigned AN1                    :1;
        unsigned AN2                    :1;
        unsigned AN3                    :1;
        unsigned T0CKI                  :1;
        unsigned AN4                    :1;
        unsigned OSC2                   :1;
    };
    struct {
        unsigned                        :2;
        unsigned VREFM                  :1;
        unsigned VREFP                  :1;
        unsigned                        :1;
        unsigned LVDIN                  :1;
    };
    struct {
        unsigned                        :5;
        unsigned HLVDIN                 :1;
    };
    struct {
        unsigned                        :7;
        unsigned RA7                    :1;
    };
    struct {
        unsigned                        :7;
        unsigned RJPU                   :1;
    };
    struct {
        unsigned ULPWUIN                :1;
    };
} PORTAbits_t;
PORTAbits_t PORTAbits;

typedef union {
    struct {
        unsigned LATC0                  :1;
        unsigned LATC1                  :1;
        unsigned LATC2                  :1;
        unsigned                        :3;
        unsigned LATC6                  :1;
        unsigned LATC7                  :1;
    };
    struct {
        unsigned LC0                    :1;
    };
    struct {
        unsigned                        :1;
        unsigned LC1                    :1;
    };
    struct {
        unsigned                        :2;
        unsigned LC2                    :1;
    };
    struct {
        unsigned                        :3;
        unsigned LC3                    :1;
    };
    struct {
        unsigned                        :4;
        unsigned LC4                    :1;
    };
    struct {
        unsigned                        :5;
        unsigned LC5                    :1;
    };
    struct {
        unsigned                        :6;
        unsigned LC6                    :1;
    };
    struct {
        unsigned                        :7;
        unsigned LC7                    :1;
    };
} LATCbits_t;
LATCbits_t LATCbits;

#endif
