#include "TBITMODEM.h"
#include "rutine.hpp"



TBITMODTX::TBITMODTX (const S_GPIOPIN *p, uint32_t sz) : pin (p), c_alloc_size (sz*4 + sizeof(t_frame_prefix_t) + sizeof(t_frame_postfix_t))
{
_pin_low_init_out_pp ((S_GPIOPIN*)pin, 1, EHRTGPIOSPEED_MID);
_pin_output ((S_GPIOPIN*)pin, false);
buffer = new uint8_t[c_alloc_size];
clear ();
}



void TBITMODTX::clear ()
{
	lp_tx_data = 0;
	tx_bitmask = 0;
	tx_sz = 0;
}

		

bool TBITMODTX::is_free ()
{
	return lp_tx_data?false:true;
}



bool TBITMODTX::get_next_bits (bool &v)
{
bool rv = false;
if (lp_tx_data)
	{
	if (!tx_bitmask)
		{
		if (tx_sz)
			{
			tx_data = *lp_tx_data++;
			tx_bitmask = 128;
			tx_sz--;
			}
		else
			{
			lp_tx_data = 0;
			}
		}
	if (tx_bitmask)
		{
			v = (tx_data & tx_bitmask)?true:false;
			tx_bitmask = tx_bitmask >> 1;
			rv = true;
		}
	}
return rv;
}



bool TBITMODTX::send (void *src, uint8_t sz)
{
bool rv = false;
if (!lp_tx_data && sz && sz <= c_alloc_size)
	{
	uint16_t csz = data_coder (src, buffer, sz);
	if (csz)
		{
		tx_bitmask = 0;
		tx_sz = csz;
		lp_tx_data = buffer;
		rv = true;
		}
	}
return rv;
}



void TBITMODTX::task_periodic ()
{
if (!phases_cnt)
	{
		if (!get_next_bits (f_cur_modulat_bit)) return;	// end transmits
		phases_cnt = 2;	
	}
// старт пульса формируется с негативной фазы
switch (phases_cnt)
	{
	case 2:	// positive pulse
	case 1:	// negative pulse
		{
		// модулируем только состояние 1
		if (f_cur_modulat_bit) _pin_output ((S_GPIOPIN*)pin, (phases_cnt & 2));

		phases_cnt--;
		break;
		}
	}
}



uint16_t TBITMODTX::data_coder (void *src, void *dst, uint8_t sz)
{
uint16_t rv = 0;
	if (sz)
		{
		uint8_t calc_sz = sz;
		uint16_t coder_sz = 0, crc16 = 0;
		uint8_t *s = (uint8_t*)src;
		uint8_t *d = (uint8_t*)dst;
		t_coder_t cd_dt;
			
		// add wakeup
		coder_data (C_WAKEUP_BYTE, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((t_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		// add preamble 1
		coder_data (C_PREAMBLE_BYTE_A, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((t_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		// add preamble 2	
		coder_data (C_PREAMBLE_BYTE_B, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((t_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		// add len	
		coder_data (sz, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((t_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		
		while (sz)
			{
			coder_data (*s++, cd_dt);
			coder_sz += sizeof(cd_dt);
			if (coder_sz > c_alloc_size) break;
			*((t_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
			sz--;
			}
		if (!sz) 
			{
			uint16_t crc16 = coder_calc16 (src, calc_sz);
			
			// add crc low
			coder_data (crc16 & 0xFF, cd_dt);
			coder_sz += sizeof(cd_dt);
			*((t_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
			// add crc hi
			coder_data ((crc16>>8) & 0xFF, cd_dt);
			coder_sz += sizeof(cd_dt);
			*((t_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
			// add sleep
			coder_data (C_SLEEP_BYTE, cd_dt);
			coder_sz += sizeof(cd_dt);
			*((t_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
				
			rv = coder_sz;
			}
		}
return rv;
}


uint16_t TBITMODTX::coder_calc16 (void *src, uint16_t sz)
{
	uint16_t rv = 0;
	uint8_t *s = (uint8_t*)src;
	uint8_t dt8;
	while (sz)
		{
		dt8 = *s++;
		switch (dt8)
			{
			case 0: rv += 0x100; break;
			case 0xFF: rv += 0x01; break;
			default: rv += dt8; break;
			}
		rv += sz;
		sz--;
		}
	return rv;
}



// coding format: 1110 = 1, 1000 = 0
static const uint8_t codarr[4] = {0x88/*00*/,0x8E/*01*/,0xE8/*10*/,0xEE/*11*/};
void TBITMODTX::coder_data (uint8_t data, t_coder_t &dst)
{
uint8_t in_dmask = 128, dt8;
uint8_t out_dix = 0;
uint8_t byte_offs;
	uint8_t cnt = 4;
while (cnt)
	{
	dt8 = data & 0x3;
	dst.data[out_dix++] = codarr[dt8];
	data >>= 2;
	cnt--;
	}
}



bool TBITMODTX::set_bit_mncharr_array (void *d, uint32_t indxbit, bool v)
{
	bool rv = false;
	uint8_t *dst = (uint8_t*)d;
	uint32_t ixbyte = indxbit / 8;
	uint8_t ixbit = indxbit % 8;
	uint8_t mask_bt = 128 >> ixbit;
	if (c_alloc_size > ixbyte)
		{
		uint8_t data = *dst;
		if (v)
			{
			data |= mask_bt;
			}
		else
			{
			data &= (0xFF - mask_bt);
			}
		*dst = data;
		rv = true;
		}
	return rv;
}



bool TBITMODTX::add_mncharr_bit (bool v)
{
bool rv = set_bit_mncharr_array (buffer, add_mncharr_ix, v);
if (rv) add_mncharr_ix++;
return rv;
}



bool TBITMODTX::add_mncharr_preamble ()
{
	bool rv = add_mncharr_bit (true);
	do	{
			if (!add_mncharr_bit (true)) break;
			if (!add_mncharr_bit (true)) break;
			if (!add_mncharr_bit (false)) break;
		  rv = true;
			} while (false);
	return rv;
}



bool TBITMODTX::add_mncharr_data (uint8_t dat)
{
uint8_t cnt = 8, mask_rl = 128;
bool db, rv = false;
	while (cnt)
		{
		db = (dat & mask_rl)? false: true;
		if (!add_mncharr_bit (db)) break;
		if (!add_mncharr_bit (!db)) break;
		mask_rl >>= 1;
		cnt--;
		}
	if (!cnt) rv = true;
	return rv;
}



bool TBITMODTX::add_mncharr_arr (void *src, uint8_t sz)
{
bool rv = false;
	uint8_t *s = (uint8_t*)src;
	while (sz)
		{
		if (!add_mncharr_data (*s++)) break;
		sz--;
		}
	if (!sz) rv = true;
return rv;
}



uint16_t TBITMODTX::mncharr_data_coder (void *src, void *dst, uint8_t sz)
{
uint16_t rv = 0;
add_mncharr_ix = 0;
	t_frame_prefix_t prfix;
	t_frame_postfix_t postfix;
	do	{
			if (!add_mncharr_preamble ()) break;
			if (!add_mncharr_preamble ()) break;
			if (!add_mncharr_preamble ()) break;
			prfix.len = sz;
			prfix.preamble_a = C_PREAMBLE_BYTE_A;
		  prfix.preamble_b = C_PREAMBLE_BYTE_B;
			if (!add_mncharr_arr(&prfix, sizeof(prfix))) break;
			if (!add_mncharr_arr(src, sz)) break;
			postfix.crc16 = coder_calc16 (src, sz);
			if (!add_mncharr_arr(&prfix, sizeof(prfix))) break;
			rv = add_mncharr_ix / 8;
			if (add_mncharr_ix % 8) rv++;
			} while (false);
	
return rv;
}


#ifdef MODEM_RX
TBITMODRX::TBITMODRX (const S_GPIOPIN *p, uint32_t sz, const ESYSTIM t) : pinisr(p), c_alloc_size (sz), sys_tim (t)
{
	 ext_isr_obj = new TEXTINT_ISR (pinisr, EGPINTMOD_FALLING);
	 ext_isr_obj->set_cb (this);
	 timer_isr = new TTIM_MKS_ISR (sys_tim, 65535, 10000000);

}


		
uint32_t TBITMODRX::check_in ()
{
	uint32_t rv = 0;
	return rv;
}



uint32_t TBITMODRX::in (void *dst, uint32_t sz_max)
{
	uint32_t rv = 0;
	return rv;
}



void TBITMODRX::isr_gpio_cb_isr (uint8_t isr_n, bool pinstate)
{
}



void TBITMODRX::tim_comp_cb_user_isr (ESYSTIM t, EPWMCHNL ch)
{

}


#endif
