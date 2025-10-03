#ifndef _H_HARDWARE_RUTINES_H_
#define _H_HARDWARE_RUTINES_H_

// HRDCPU = 1  (stm32f1xx)
// HRDCPU = 2  (stm32f2xx)
// HRDCPU = 3  (stm32f3xx)
// HRDCPU = 4  (stm32f4xx)

#ifdef STM32H503xx
	#define HRDCPU 5
	#include "stm32h5xx_hal.h"

	enum ESYSUSART {ESYSUSART_1 = 0, ESYSUSART_2, ESYSUSART_3, ESYSUSART_ENDENUM};
	enum ESYSSPI {ESYSSPI_1 = 0, ESYSSPI_2, ESYSSPI_3, ESYSSPI_ENDENUM};
	enum ESYSTIM {ESYSTIM_1 = 0, ESYSTIM_2, ESYSTIM_3, ESYSTIM_4, ESYSTIM_5, ESYSTIM_6, ESYSTIM_7, ESYSTIM_ENDENUM};
	enum EGPIOIX {EGPIOIX_A = 0, EGPIOIX_B, EGPIOIX_C, EGPIOIX_D, EGPIOIX_E, EGPIOIX_F, EGPIOIX_ENDENUM};
#endif

#ifdef STM32F103x6
	#define HRDCPU 1
	#include "stm32f1xx_hal.h"
	#include "stm32f1xx_hal_conf.h"
	
	enum ESYSUSART {ESYSUSART_1 = 0, ESYSUSART_2, ESYSUSART_ENDENUM};
	enum ESYSSPI {ESYSSPI_1 = 0, ESYSSPI_ENDENUM};
	enum ESYSTIM {ESYSTIM_1 = 0, ESYSTIM_2, ESYSTIM_3, ESYSTIM_ENDENUM};
	enum EGPIOIX {EGPIOIX_A = 0, EGPIOIX_B, EGPIOIX_C, EGPIOIX_D, EGPIOIX_E, EGPIOIX_F, EGPIOIX_ENDENUM};
	
#endif


#ifdef STM32F103xB 
	#define HRDCPU 1
	#include "stm32f1xx_hal.h"
	#include "stm32f1xx_hal_conf.h"
	
	enum ESYSUSART {ESYSUSART_1 = 0, ESYSUSART_2, ESYSUSART_3, ESYSUSART_ENDENUM};
	enum ESYSSPI {ESYSSPI_1 = 0, ESYSSPI_2, ESYSSPI_ENDENUM};
	enum ESYSTIM {ESYSTIM_1 = 0, ESYSTIM_2, ESYSTIM_3, ESYSTIM_4, ESYSTIM_ENDENUM};
	enum EGPIOIX {EGPIOIX_A = 0, EGPIOIX_B, EGPIOIX_C, EGPIOIX_D, EGPIOIX_E, EGPIOIX_F, EGPIOIX_ENDENUM};
	
#endif

#ifdef STM32L432xx
	#define HRDCPU 4
	#include "stm32l4xx_hal.h"
	
	enum ESYSUSART {ESYSUSART_1 = 0, ESYSUSART_2, ESYSUSART_ENDENUM};
	enum ESYSSPI {ESYSSPI_1 = 0, ESYSSPI_2, ESYSSPI_3, ESYSSPI_ENDENUM};
	enum ESYSTIM {ESYSTIM_1 = 0, ESYSTIM_2, ESYSTIM_3, ESYSTIM_4, ESYSTIM_5, ESYSTIM_6, ESYSTIM_7, ESYSTIM_8, ESYSTIM_9, ESYSTIM_10, ESYSTIM_11, ESYSTIM_12, ESYSTIM_13, ESYSTIM_14, ESYSTIM_15, ESYSTIM_16, ESYSTIM_ENDENUM};
	enum EGPIOIX {EGPIOIX_A = 0, EGPIOIX_B, EGPIOIX_C, EGPIOIX_D, EGPIOIX_E, EGPIOIX_F, EGPIOIX_ENDENUM};
#endif


#if (HRDCPU == 1)
	#define C_HRDGPIOSPEED_LOW GPIO_SPEED_LOW
	#define C_HRDGPIOSPEED_MID GPIO_SPEED_MEDIUM
	#define C_HRDGPIOSPEED_HI GPIO_SPEED_HIGH
	#define C_HRDGPIO_NOPULL GPIO_NOPULL
	#define C_HRDGPIO_PULLUP GPIO_PULLUP
	#define C_HRDGPIO_PULLDW GPIO_PULLDOWN
#endif


#if (HRDCPU == 2)
	#define C_HRDGPIOSPEED_LOW
	#define C_HRDGPIOSPEED_MID
	#define C_HRDGPIOSPEED_HI
#endif

#if (HRDCPU == 3)
	#define C_HRDGPIOSPEED_LOW
	#define C_HRDGPIOSPEED_MID
	#define C_HRDGPIOSPEED_HI
#endif

#if (HRDCPU == 4)
	#define C_HRDGPIOSPEED_LOW GPIO_SPEED_LOW
	#define C_HRDGPIOSPEED_MID GPIO_SPEED_MEDIUM
	#define C_HRDGPIOSPEED_HI GPIO_SPEED_HIGH
	#define C_HRDGPIO_NOPULL GPIO_NOPULL
	#define C_HRDGPIO_PULLUP GPIO_PULLUP
	#define C_HRDGPIO_PULLDW GPIO_PULLDOWN
#endif


#if (HRDCPU == 5)
	#define C_HRDGPIOSPEED_LOW GPIO_SPEED_FREQ_LOW
	#define C_HRDGPIOSPEED_MID GPIO_SPEED_FREQ_MEDIUM
	#define C_HRDGPIOSPEED_HI GPIO_SPEED_FREQ_HIGH
	#define C_HRDGPIOSPEED_VERY_HI GPIO_SPEED_FREQ_VERY_HIGH
	#define C_HRDGPIO_NOPULL GPIO_NOPULL
	#define C_HRDGPIO_PULLUP GPIO_PULLUP
	#define C_HRDGPIO_PULLDW GPIO_PULLDOWN
#endif

#define C_MAXGPIOPININTERRUPT 16



enum E_GPIO_AF {E_GPIO_AF0 = 0, E_GPIO_AF1, E_GPIO_AF2, E_GPIO_AF3, E_GPIO_AF4, E_GPIO_AF5, E_GPIO_AF6,E_GPIO_AF7, E_GPIO_AF8, E_GPIO_AF9, E_GPIO_AF10, E_GPIO_AF11, E_GPIO_AF12, E_GPIO_AF13, E_GPIO_AF14, E_GPIO_AF15, E_GPIO_AF_ENDENUM};
enum EHRTGPIOPULL {EHRTGPIOPULL_NO = C_HRDGPIO_NOPULL, EHRTGPIOPULL_UP = C_HRDGPIO_PULLUP, EHRTGPIOPULL_DOWN = C_HRDGPIO_PULLDW};
enum EHRTGPIOSPEED {EHRTGPIOSPEED_LOW = C_HRDGPIOSPEED_LOW, EHRTGPIOSPEED_MID = C_HRDGPIOSPEED_MID, EHRTGPIOSPEED_HI = C_HRDGPIOSPEED_HI};
enum EGPMD {EGPMD_OD = 0, EGPMD_PP, EGPMD_IN, EGPMD_AIN, EGPMD_ENDENUM};
enum EGPINTMOD {EGPINTMOD_RISING = 0, EGPINTMOD_FALLING = 1, EGPINTMOD_RISING_FALLING = 2, EGPINTMOD_ENDENUM};



typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} S_GPIOPIN;



typedef struct {
	S_GPIOPIN pin;
	EGPMD mod;
	bool out_mod_def_set;		// only for output modes: EGPMD_OD, EGPMD_PP
} S_GPMD_PIN_T;


typedef struct {
	S_GPIOPIN mosi;
	S_GPIOPIN miso;
	S_GPIOPIN sck;
	S_GPIOPIN cs;
} S_SPIGPIO_T;


#if (HRDCPU == 1)
void _pin_low_init_out_pp_af (const S_GPIOPIN *lp_pin , EHRTGPIOSPEED sp);
void _pin_low_init_out_od_af (const S_GPIOPIN *lp_pin, EHRTGPIOSPEED sp );
#endif
#if (HRDCPU == 2 || HRDCPU == 3 || HRDCPU == 4 || HRDCPU == 5)
void _pin_low_init_out_pp_af ( uint8_t af_codemux, S_GPIOPIN *lp_pin , EHRTGPIOSPEED sp);
void _pin_low_init_out_od_af ( uint8_t af_codemux, S_GPIOPIN *lp_pin, EHRTGPIOSPEED sp );
void _pin_low_init_in_af ( uint8_t af_codemux, S_GPIOPIN *lp_pin );
#endif

void hard_gpio_procedure_this_set (uint8_t p, void *cb);
void *hard_gpio_procedure_this_get (uint8_t p);

void *hard_timer_procedure_this_get (ESYSTIM p);
void hard_timer_procedure_this_set (ESYSTIM p, void *cb);

IRQn_Type hard_gpio_ext_irq_type (uint8_t p);
IRQn_Type hard_tim_irq_type (ESYSTIM p);
IRQn_Type hard_spi_irq_type (ESYSSPI p);
TIM_TypeDef *hard_get_tim (ESYSTIM t, bool *f_wdth_32);
USART_TypeDef *hard_get_usart (ESYSUSART s);
bool hard_tim_is_32bit (ESYSTIM tn);
SPI_TypeDef *hard_get_spi (ESYSSPI tn);
void hard_tim_clock_enable (ESYSTIM tn);
void hard_spi_clock_enable (ESYSSPI tn);
void hard_usart_clock_enable (USART_TypeDef *p);
void hard_usart_clock_disable (USART_TypeDef *p);
void hard_gpio_clock_enable (GPIO_TypeDef *port);
void _pin_low_init (S_GPMD_PIN_T *lp_pin, unsigned char cnt, EHRTGPIOSPEED sp, EHRTGPIOPULL pl);
void _pin_low_init_out_od (const S_GPIOPIN *lp_pin, unsigned char cnt, EHRTGPIOSPEED sp);
void _pin_low_init_out_pp (const S_GPIOPIN *lp_pin, uint8_t cnt, EHRTGPIOSPEED sp );

void _pin_low_init_in (const S_GPIOPIN *lp_pin, uint8_t cnt, EHRTGPIOSPEED sp, EHRTGPIOPULL pl);
void _pin_low_init_int (const S_GPIOPIN *lp_pin, unsigned char cnt, EGPINTMOD md, EHRTGPIOSPEED sp);
void _pin_low_init_adc (const S_GPIOPIN *lp_pin, uint8_t cnt, EHRTGPIOSPEED sp );
bool _pin_input (const S_GPIOPIN *lp_pin );
void _pin_output (const S_GPIOPIN *lp_pin, bool val );

#define _pin_set_to(x,y) _pin_output(x,y)
#define _pin_get(x) _pin_input(x)



#endif

