#include "TBITMODEM.h"
#include "ITIMINT.h"
#include "hard_rut.hpp"

#ifdef MODEM_RX
TBITMODRX *mod_rx = 0;
const S_GPIOPIN pin_rxisr = {GPIOA, GPIO_PIN_0};
#endif

#ifdef MODEM_TX
TBITMODTX *mod_tx = 0;
#endif

static inline void tx_pin (bool val);


static void isr_tim ()
{

}





int main ()
{
#ifdef MODEM_RX
mod_rx = new TBITMODRX (&pin_rxisr, 1, ESYSTIM_1);
#endif
while (true)
	{
	
	}
}

