#ifndef F_CPU /*if F_CPU isn't alr defined*/
#define F_CPU 16000000UL /*setting clock time*/
#endif

/*libraries*/
#include <stdint.h> /*fixed-size types like uint_8*/
#include <avr/io.h> /*gives access to using PORTD, ADCSRA, etc*/
#include <avr/interrupt.h> /*gives interrupt functionality*/
#include <util/delay.h> /*gives delay functionality*/
#include <util/twi.h> /*gives i2c status codes*/

/*pins*/
#define MUX_MASK 0x3C /*since each port is a bit, this gives us coverage from D2-D5(bits 2-5)*/
#define MUX_SHIFT 2 /*bit shift for MUX, doing this because from bits 2 to 5 are S0-S3*/
#define PIN_EN PD6 /* mux enable, active LOW (bit 6) */
#define PIN_LED PD7 /* red fault LED (bit 7)*/

/*constants*/
#define NUM_CH 16 /*# of channels*/
#define I2C_ADDR 0x08 /*default i2c slave address*/
#define SETTLE_US 20 /*time between */
#define FAULT_C100 6000 /* 60.00 C, units of 0.01 C */
#define BLINK_MS 500
#define TEMP_STATS 0x00  /* max(0-1), min(2-3), avg(4-5) + fault flag(6) (7 bytes) */
#define ALL_TEMPS 0x01          /* all 16 temps (32 bytes) */

/*sensor curve*/
#define TABLE_LEN      33 /*33 entries in datasheet*/
#define TABLE_START_TEMP  (-40) /*starting point in datasheet*/
#define TABLE_TEMP_STEP   5 /*step of temp values*/
static const uint16_t table_mv[LUT_LEN] = { /*values from datsheet (in mV)*/
    2440, 2420, 2400, 2380, 2350, 2320, 2270, 2230, 2170, 2110, 2050,
    1990, 1920, 1860, 1800, 1740, 1680, 1630, 1590, 1550, 1510, 1480,
    1450, 1430, 1400, 1380, 1370, 1350, 1340, 1330, 1320, 1310, 1300
};

/*state variables*/
static int16_t  temps[NUM_CH];                 /* degrees C x 100 (more work to deal with decimals so
                                                values scale accordingly; keeps precision and efficiency)*/
static uint32_t last_toggle_ms;
static volatile uint32_t ms_ticks;             /* 1 ms counter */

/* Copies read by the I2C interrupt */
static volatile int16_t final_temps[NUM_CH];
static volatile int16_t final_max, final_min, final_avg;
static volatile uint8_t final_fault;  /*using different final variables because the other main ones 
                                        are constantly being updated and replaced*/
static volatile uint8_t reg_ptr = TEMP_STATS;


/*initializing the functions*/
static void gpio_init(void);
static void timer0_init(void);
static void adc_init(void);
static uint32_t millis(void);

static void mux_select(uint8_t ch);
static uint16_t adc_read_mv(void);
static int16_t mv_to_c100(uint16_t mv);
static void stats_compute(int16_t *mx, int16_t *mn, int16_t *avg);   /* (a) */
static void final(int16_t mx, int16_t mn, int16_t avg, uint8_t fault);
static void fault_led_update(uint8_t fault);

/* hardware initialization */

static void gpio_init(void)
{
    /*set mux pins, EN and LED as outputs (DDRD)
    drive EN low (mux on) and LED low (off) (PORTD) */
}

static void timer0_init(void)
{
    /*Timer0 CTC mode, prescaler 64, OCR0A = 249
    enable the compare-match interrupt (TIMSK0) */
}

ISR(TIMER0_COMPA_vect)
{
    /*ms_ticks++ */
}

static uint32_t millis(void)
{
    /*return ms_ticks, read with interrupts off (save/restore SREG) */
    return 0;
}

static void adc_init(void)
{
    /*AVcc reference, channel 0 (ADMUX)
    enable ADC, clock prescaler 128 (ADCSRA) */
}

static void twi_init(void)
{
    /*set slave address in TWAR (address << 1)
    enable TWI, ACK and interrupt in TWCR */
}

/*measurement*/
static void mux_select(uint8_t ch)
{
    /*write the 4 bits of ch to D2-D5 without touching other pins
      then wait SETTLE_US */
}

static uint16_t adc_read_mv(void)
{
    /*start a conversion (ADSC) and wait for it to finish
      throw away the first reading, keep the second
      convert counts (0-1023) to millivolts (5000 mV) */
    return 0;
}

static int16_t mv_to_c100(uint16_t mv)
{
    /*limit if mv is outside the table
      find the two table entries that bracket mV (voltage falls as
      temperature rises)
      linear regression between them, return degrees C x 100 */
    return 0;
}

static void stats_compute(int16_t *mx, int16_t *mn, int16_t *avg) /* * is the pointer to the variable
                                                                       essentially means go to * address 
                                                                       and get the value*/
{
    /*one pass over temps[]: find max, min, and sum
      average = sum / NUM_CH (use a 32-bit sum)-> using 32bit since there are
      16 elements in the arra and each element will take up 2 bits */
}

static void final(int16_t mx, int16_t mn, int16_t avg, uint8_t fault)
{
    /*with interrupts off, copy temps[] and the stats into the
      final_ variables, then restore the interrupt state */
}

static void fault_led_update(uint8_t fault)
{
    /*if no fault, LED off and return
      if fault, toggle the LED every BLINK_MS using millis()
      (no delay, so the scan keeps running) */
}

/* ================= I2C slave ================= */

static void tx_prepare(void)
{
    /*build tx_buf for the register in reg_ptr:
      REG_ALL     -> 16 temps, 2 bytes each, low byte first (32 bytes)
      REG_SUMMARY -> max, min, avg (2 bytes each) + fault byte (7 bytes) */
}


int main(void)
{
    gpio_init();
    timer0_init();
    adc_init();
    sei();                                  /* enable interrupts last */

    for (;;) { /*loops forever*/
        /*scan every channel */
        for (uint8_t ch = 0; ch < NUM_CH; ch++) {
            mux_select(ch);
            temps[ch] = mv_to_c100(adc_read_mv());
        }

        /*stats over all 16 temperatures */
        int16_t mx, mn, avg;
        stats_compute(&mx, &mn, &avg); /*& means the address of the variable*/

        /*fault if the hottest sensor is above 60 C */
        uint8_t fault = (mx > FAULT_C100);

        /*make data available to the I2C master, then update the LED */
        final(mx, mn, avg, fault);
        fault_led_update(fault);
    }
}
