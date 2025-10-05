#ifndef _TMODEM_BIT_
#define _TMODEM_BIT_

#include "stdint.h"
#include "hard_rut.hpp"

#ifdef MODEM_RX
	#include "IEXTINT.h"
	#include "ITIMINT.h"
#endif

#define C_WAKEUP_BYTE 0xFF
#define C_PREAMBLE_BYTE_A 0x55
#define C_PREAMBLE_BYTE_B 0xAA
#define C_SLEEP_BYTE 0x00

#pragma pack (push,1)
typedef struct {
	uint8_t data[4];
} t_coder_t;

typedef struct {
	uint8_t data[2];
} t_coder_mnch_t;

typedef struct {
	uint8_t preamble_a;
	uint8_t preamble_b;
	uint8_t len;
} t_frame_prefix_t;

typedef struct {
	uint16_t crc16;
} t_frame_postfix_t;
#pragma pack (pop)





class TBITMODTX {
		const S_GPIOPIN *pin;

		uint8_t *buffer;
		const uint32_t c_alloc_size;
	
		void task_periodic ();
		void clear ();
		
		uint8_t *lp_tx_data;
		uint8_t tx_data;
		uint8_t tx_bitmask;
		uint32_t tx_sz;
	
		bool get_next_bits (bool &v);
		bool f_cur_modulat_bit;
		uint8_t phases_cnt;
	
		uint16_t data_coder (void *src, void *dst, uint8_t sz);
		void coder_data (uint8_t data, t_coder_t &dst); 
		
		uint16_t coder_calc16 (void *src, uint16_t sz); 
		bool set_bit_mncharr_array (void *d, uint32_t ixbit, bool v);
		uint32_t add_mncharr_ix;
		bool add_mncharr_bit (bool v);
		bool add_mncharr_preamble ();
		bool add_mncharr_data (uint8_t dat);
		bool add_mncharr_arr (void *src, uint8_t sz);
		uint16_t mncharr_data_coder (void *src, void *dst, uint8_t sz);
	
	public:
		TBITMODTX (const S_GPIOPIN *p, uint32_t sz);
		bool is_free ();
		bool send (void *src, uint8_t sz);
};



#ifdef MODEM_RX

class TRXINBIT {
	public:
		
		enum EBITSTATE {EBITSTATE_NONE, EBITSTATE_SYNC_START, EBITSTATE_RXDATA, EBITSTATE_STOP, EBITSTATE_RX_COMPLETE, EBITSTATE_ENDENUM};
		TRXINBIT (uint32_t size);
		void start ();
		void stop ();
		bool add_data_bit (bool val);
		EBITSTATE state ();
		
	private:
		EBITSTATE state_sw;
		uint8_t *buffer;
		const uint32_t c_alloc_size;
		void decode ();
};



class TBITMODRX: public IEXTISRCB, public ITIMCB {
		enum ERXSTATE {ERXSTATE_NONE = 0, ERXSTATE_SYNC, ERXSTATE_DATA, ERXSTATE_END, ERXSTATE_WAITBLANK, ERXSTATE_ENDENUM};
		enum EPWMTYPE {EPWMTYPE_WAKEUP = EPWMCHNL_PWM1, EPWMTYPE_EMPTY = EPWMCHNL_PWM2, EPWMTYPE_ENDENUM};

	
		const S_GPIOPIN *pinisr;
		const uint32_t c_data_period;
		uint32_t c_wakeup_period;
		uint32_t c_empty_check_period;
		bool f_inp_data_await;
		
		ERXSTATE rx_state;
		
		void timer_sync ();
		void gpio_isr_enable ();
		void gpio_isr_disable ();
		TRXINBIT *bits;
	
		virtual void isr_gpio_cb_isr (uint8_t isr_n, bool pinstate) override;
		TEXTINT_ISR *ext_isr_obj;
	
		TTIM_ISR *timer_isr;
		const ESYSTIM sys_tim;
		virtual void tim_comp_cb_user_isr (ESYSTIM t, EPWMCHNL ch) override;
	
	public:
		TBITMODRX (const S_GPIOPIN *p, uint32_t sz, const ESYSTIM t);
		uint32_t check_in ();
		uint32_t in (void *dst, uint32_t sz_max);
		
};
#endif

#endif
