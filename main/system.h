 /*
 * Misc. System related stuff
 * By LA7ECA, ohanssen@acm.org
 */
 
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h" 

#include "driver/uart.h"
#include "esp_log.h"
#include "driver/timer.h"
#include "driver/adc.h"

#if !defined __DEF_SYSTEM_H__
#define __DEF_SYSTEM_H__

// #define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

/* Firmware upgrade and shutdown */
esp_err_t firmware_upgrade();
void systemShutdown(void);

/* Time */
typedef struct tm tm_t; 
extern void time_init(); 
bool   getUTC(tm_t *timeinfo);
time_t getTime();
char*  datetime2str(char*, time_t);
char*  time2str (char*, time_t);
char*  date2str (char*, time_t);

/* Hardware timer - as periodic clocks */
void clock_init(int group, int idx, uint16_t divider,   timer_isr_t isr, bool iram);
void clock_start(int group, int idx, double interval);
void clock_stop(int group, int idx);
void clock_changeInterval(int group, int idx, double interval);
void IRAM_ATTR clock_clear_intr(int group, int index);

 
/* Logging */
bool hasTag(char*tag);
void set_logLevels(void);
char* loglevel2str(esp_log_level_t lvl);
esp_log_level_t str2loglevel(char* str);


/* Read text line from serial or other.. */
bool readline(uart_port_t port, char* buf, const uint16_t max); 
bool freadline(FILE* f, char* buf, const uint16_t max); 

/* Text utilities */
uint8_t tokenize(char*, char*[], uint8_t, char*, bool);


/* ADC */
void adc_print_char();
void adc_init();
uint16_t adc1_read(uint8_t chan);
uint16_t adc2_read(uint8_t chan);
uint16_t adc_toVoltage(uint16_t val);
uint16_t adc_batt();
uint16_t adc_batt_status(char* line1, char* line2);
int16_t  adc_sample();
void     adc_calibrate();


/* Battery */
void batt_init(void);
bool batt_charge(void);



#define sleepMs(n)  vTaskDelay(pdMS_TO_TICKS(n))
#define t_yield     taskYIELD

/* Simplified semaphore operations */
typedef SemaphoreHandle_t semaphore_t;
#define sem_create(cnt)  xSemaphoreCreateCounting(65000, cnt)
#define sem_createBin()  xSemaphoreCreateBinary()
#define sem_delete(sem)  vSemaphoreDelete(sem)
#define sem_up(x)        xSemaphoreGive(x)
#define sem_upI(x)       xSemaphoreGiveFromISR(x, pdFALSE)
#define sem_down(x)      xSemaphoreTake(x, portMAX_DELAY)
#define sem_getCount(x)  uxSemaphoreGetCount(x)

typedef SemaphoreHandle_t mutex_t; 
#define mutex_create()   xSemaphoreCreateMutex()
#define mutex_lock(x)    xSemaphoreTake((x), portMAX_DELAY)
#define mutex_unlock(x)  xSemaphoreGive((x))


/* Make event groups look like simpler condition variables */
typedef EventGroupHandle_t cond_t;
#define cond_create          xEventGroupCreate
#define cond_wait(cond)      xEventGroupWaitBits(cond, BIT_0, pdFALSE, pdFALSE,  portMAX_DELAY)
#define cond_set(cond)       xEventGroupSetBits(cond, BIT_0)
#define cond_setI(cond)      _cond_setBitsI(cond, BIT_0)
#define cond_clear(cond)     xEventGroupClearBits(cond, BIT_0)
#define cond_clearI(cond)    xEventGroupClearBitsFromISR(cond, BIT_0)
#define cond_isSet(cond)     (xEventGroupGetBits(cond) & BIT_0)
#define cond_isSetI(cond)    (xEventGroupGetBitsFromISR(cond) & BIT_0)

#define cond_waitBits(cond, bits)       xEventGroupWaitBits(cond, bits, pdFALSE, pdFALSE,  portMAX_DELAY)
#define cond_setBits(cond, bits)        xEventGroupSetBits(cond, bits)
#define cond_setBitsI(cond, bits)       _cond_setBitsI(cond, bits)
#define cond_testBits(cond, bits)       (xEventGroupGetBits(cond) & bits)
#define cond_clearBits(cond, bits)      xEventGroupClearBits(cond, bits)


BaseType_t _cond_setBitsI(cond_t cond, BaseType_t bits);


#endif
