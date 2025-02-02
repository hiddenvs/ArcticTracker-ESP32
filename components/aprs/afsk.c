
#include <stdbool.h>
#include "defines.h"
#include "afsk.h"
#include "system.h"



/* Important: Sample rate must be divisible by bitrate */
/* Sampling clock setup */
#define CLOCK_DIVIDER 16
#define CLOCK_FREQ (TIMER_BASE_CLK / CLOCK_DIVIDER)
#define FREQ(f) (CLOCK_FREQ/(f))

/* Common stuff for afsk-RX and afsk-TX. */
static bool    rxMode = false; 
static bool    txOn = false; 
static int     rxEnable = 0;
static mutex_t afskmx; 

void afsk_rxSampler(void *arg);
void afsk_txBitClock(void *arg);


/*********************************************************
 * ISR for clock
 *********************************************************/



static bool afsk_sampler(void *arg) 
{   
// FIXME
//    clock_clear_intr(AFSK_TIMERGRP, AFSK_TIMERIDX);
    if (rxMode)
        rxSampler_isr(arg); 
    else
        afsk_txBitClock(arg); 
    return true;
}



/**********************************************************
 *  Init shared clock for AFSK RX and TX 
 **********************************************************/

void afsk_init() 
{
    afskmx = mutex_create();
    clock_init(AFSK_TIMERGRP, AFSK_TIMERIDX, CLOCK_DIVIDER, afsk_sampler, false);
}


/**********************************************************
 * Allow app to enable/disable starting of receiver
 * clock. To save battery. 
 **********************************************************/

void afsk_rx_enable() {
    rxEnable++; 
}
void afsk_rx_disable() {
    if (rxEnable > 0)
       rxEnable--; 
}


/**********************************************************
 * Turn receiving on and off
 * These are called from ISR handlers when squelch opens
 **********************************************************/
 
void afsk_rx_start() {
    if (!rxEnable) 
        return;
    afsk_PTT(false); 
    clock_stop(AFSK_TIMERGRP, AFSK_TIMERIDX);     
    rxMode = true; 
    clock_start(AFSK_TIMERGRP, AFSK_TIMERIDX, FREQ(AFSK_SAMPLERATE));
}

   
void afsk_rx_stop() {
    if (!rxEnable)
        return;
    clock_stop(AFSK_TIMERGRP, AFSK_TIMERIDX);  
    rxMode=false; 
    rxSampler_nextFrame(); 
    afsk_rx_newFrame();

    if (txOn)
        clock_start(AFSK_TIMERGRP, AFSK_TIMERIDX, FREQ(AFSK_BITRATE));
}


 
/***********************************************************
 * Turn transmitter clock on/off
 ***********************************************************/
 
void afsk_tx_start() {
    mutex_lock(afskmx);
    clock_stop(AFSK_TIMERGRP, AFSK_TIMERIDX);
    clock_start(AFSK_TIMERGRP, AFSK_TIMERIDX, FREQ(AFSK_BITRATE));
    txOn = true; 
    mutex_unlock(afskmx);
}

 
 
/***********************************************************
 *  Stop transmitter.
 ***********************************************************/
 
 void afsk_tx_stop() {
    mutex_lock(afskmx);
    clock_stop(AFSK_TIMERGRP, AFSK_TIMERIDX); 
    afsk_PTT(false); 
    txOn = false; 
    if (rxMode)
        clock_start(AFSK_TIMERGRP, AFSK_TIMERIDX, FREQ(AFSK_SAMPLERATE));
    mutex_unlock(afskmx);
 }
 
 
