#ifndef _H_EXTERNAL_INTERRUPT_CLASS_H_
#define _H_EXTERNAL_INTERRUPT_CLASS_H_



#include "hard_rut.hpp"




class IEXTISRCB {
	public:
		virtual void isr_gpio_cb_isr (uint8_t isr_n, bool pinstate) = 0;
};



class TEXTINT_ISR {
	protected:
		uint8_t c_isr_nmbr;
		S_GPIOPIN *c_pin_in;
		IEXTISRCB *cb_exec;
		//virtual void isr_gpio_cb_int (uint8_t isr_n, bool pinstate) = 0;
		
	
	public:
		static long gpio_pin_ix_from_mask (uint16_t msk);
		void set_cb (IEXTISRCB *cb);
		TEXTINT_ISR (S_GPIOPIN *p, EGPINTMOD md);
	
		void gpio_isr (uint16_t pinn);
		void enable_extint_isr (bool st);
		static TEXTINT_ISR *isr_this[C_MAXGPIOPININTERRUPT];
};




#endif
