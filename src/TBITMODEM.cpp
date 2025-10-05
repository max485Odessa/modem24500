#include "TBITMODEM.h"
#include "rutine.hpp"



uint16_t TBITRUTCOMMON::coder_calc16 (void *src, uint16_t sz)
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



#ifdef MODEM_TX
TBITMODTX::TBITMODTX (const S_GPIOPIN *p, uint32_t sz) : pin (p), c_alloc_size (sz*4 + sizeof(s_frame_prefix_t) + sizeof(s_frame_postfix_t))
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
		s_coder_t cd_dt;
			
		// add wakeup
		coder_data (C_WAKEUP_BYTE, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((s_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		// add preamble 1
		coder_data (C_PREAMBLE_BYTE_A, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((s_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		// add preamble 2	
		coder_data (C_PREAMBLE_BYTE_B, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((s_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		// add len	
		coder_data (sz, cd_dt);
		coder_sz += sizeof(cd_dt);
		*((s_coder_t*)d) = cd_dt;
		d += sizeof(cd_dt);
		
		while (sz)
			{
			coder_data (*s++, cd_dt);
			coder_sz += sizeof(cd_dt);
			if (coder_sz > c_alloc_size) break;
			*((s_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
			sz--;
			}
		if (!sz) 
			{
			uint16_t crc16 = coder_calc16 (src, calc_sz);
			
			// add crc low
			coder_data (crc16 & 0xFF, cd_dt);
			coder_sz += sizeof(cd_dt);
			*((s_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
			// add crc hi
			coder_data ((crc16>>8) & 0xFF, cd_dt);
			coder_sz += sizeof(cd_dt);
			*((s_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
			// add sleep
			coder_data (C_SLEEP_BYTE, cd_dt);
			coder_sz += sizeof(cd_dt);
			*((s_coder_t*)d) = cd_dt;
			d += sizeof(cd_dt);
				
			rv = coder_sz;
			}
		}
return rv;
}



// coding format: 1110 = 1, 1000 = 0
static const uint8_t codarr[4] = {0x88/*00*/,0x8E/*01*/,0xE8/*10*/,0xEE/*11*/};
void TBITMODTX::coder_data (uint8_t data, s_coder_t &dst)
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
	s_frame_prefix_t prfix;
	s_frame_postfix_t postfix;
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
#endif


#ifdef MODEM_RX

TRXINBIT::TRXINBIT (uint32_t size) : c_alloc_size (size)
{
	buffer = new uint8_t[c_alloc_size];
	start ();
}



TRXINBIT::EBITSTATE TRXINBIT::state ()
{
	return state_sw;
}



void TRXINBIT::start ()
{
seq_sync_cnt = 0;
pulses_tmp_cnt = 0;
state_sw = EBITSTATE_SYNC_START;
rx_bit_cnt = 0;
last_error_bit_seqcnt = -1;
}



void TRXINBIT::stop ()
{
bool f_ok = false;
do	{
		if (state_sw != EBITSTATE_RXDATA) break;
		uint32_t sz_byte = rx_bit_cnt / 8;
		if (sz_byte <= sizeof(s_prepost_size_t)) break;
		uint8_t *lp = buffer;
		s_frame_prefix_t *pre = (s_frame_prefix_t*)lp;
		if (pre->preamble_a != C_PREAMBLE_BYTE_A || pre->preamble_b != C_PREAMBLE_BYTE_B) break;
		uint32_t sz_data = sz_byte - sizeof(s_prepost_size_t);
		if (sz_data != pre->len) break;
		lp += sizeof(s_frame_prefix_t);
		uint16_t crc16 = coder_calc16 (lp, pre->len);
		lp += pre->len;
		s_frame_postfix_t *post = (s_frame_postfix_t*)lp;
		if (crc16 != post->crc16) break;
		f_ok = true;
		state_sw = EBITSTATE_RX_COMPLETE;
		} while (false);
	
if (f_ok == false) start ();
}



bool TRXINBIT::bit_insert (bool val)
{
	bool rv = false;
	uint32_t byte_ofs = rx_bit_cnt / 8; 
	if (byte_ofs < c_alloc_size)
		{
		uint8_t bit_ofs = rx_bit_cnt % 8;
		uint8_t mask = 128 >> bit_ofs;
		uint8_t dat = buffer[byte_ofs];
		if (val)
			{
			dat |= mask;
			}
		else
			{
			dat &= (0xFF ^ mask);
			}
		buffer[byte_ofs] = dat;
		rv = true;
		}
	return rv;
}



bool TRXINBIT::add_data_bit (bool val)
{
	bool rv = false;
	switch (state_sw)
		{
		case EBITSTATE_SYNC_START:
			{
			if (!val)
				{
				if (pulses_tmp_cnt >= 3)
					{
					seq_sync_cnt++;
					}
				else
					{
					if (pulses_tmp_cnt == 1)
						{
						prev_rx_bit = true;
						bit_insert (prev_rx_bit);
						state_sw = EBITSTATE_RXDATA;
						}
					}
				pulses_tmp_cnt = 0;
				}
			else
				{
				pulses_tmp_cnt++;
				}
			break;
			}
		case EBITSTATE_RXDATA:
			{
			if (!pulses_tmp_cnt)
				{
				prev_rx_bit = val;
				pulses_tmp_cnt++;
				bit_insert (prev_rx_bit);
				}
			else
				{
				if (prev_rx_bit != rv)
					{
					pulses_tmp_cnt = 0;
					}
				else
					{
					// error - need resync
					last_error_bit_seqcnt = rx_bit_cnt;
					start ();
					}
				}
			break;
			}
		case EBITSTATE_RX_COMPLETE:
			{
			break;
			}
		case EBITSTATE_NONE:
		default: 
			start ();
			break;
		}
	return rv;
}



TBITMODRX::TBITMODRX (const S_GPIOPIN *p, uint32_t sz, const ESYSTIM t) : pinisr(p), sys_tim (t), c_data_period (100/*10 mks*/)
{
	 rx_state = ERXSTATE_NONE;
	 bits = new TRXINBIT (sz);
	 //buffer = new uint8_t[c_alloc_size];
	 ext_isr_obj = new TEXTINT_ISR (pinisr, EGPINTMOD_FALLING);
	 ext_isr_obj->set_cb (this);
	 f_inp_data_await = false;
	 uint32_t jitperiod = c_data_period/10;
	 c_wakeup_period = c_data_period - jitperiod;
	 c_empty_check_period = c_data_period + jitperiod;
	 timer_isr = new TTIM_ISR (sys_tim, c_data_period * 3, 10000000/*10 mhz*/);
}



void TBITMODRX::timer_sync ()
{
	timer_isr->set_timer_counter (0);
	timer_isr->set_timer_oc_value ((EPWMCHNL)EPWMTYPE_WAKEUP, c_wakeup_period);
	timer_isr->set_timer_oc_value ((EPWMCHNL)EPWMTYPE_EMPTY, c_empty_check_period);
	timer_isr->enable_timer_oc ((EPWMCHNL)EPWMTYPE_WAKEUP, true);
	timer_isr->enable_timer_oc ((EPWMCHNL)EPWMTYPE_EMPTY, true);
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
timer_sync ();
f_inp_data_await = false;
bits->add_data_bit (true);
}



void TBITMODRX::gpio_isr_enable ()
{
}



void TBITMODRX::gpio_isr_disable ()
{
}



void TBITMODRX::tim_comp_cb_user_isr (ESYSTIM t, EPWMCHNL ch)
{
if (sys_tim == t)
	{
	switch (ch)
		{
		case (EPWMCHNL)EPWMTYPE_WAKEUP:
			{
			//TRXINBIT::EBITSTATE stbit = bits->state ();
			f_inp_data_await = true;
			gpio_isr_enable ();
			break;
			}
		case (EPWMCHNL)EPWMTYPE_EMPTY:
			{
			if (f_inp_data_await) 
				{
				bits->add_data_bit (false);
				gpio_isr_disable ();
				f_inp_data_await = false;
				}
			break;
			}
		case EPWMCHNL_UPDATE:
			{
			timer_isr->enable_timer_oc ((EPWMCHNL)EPWMTYPE_WAKEUP, false);
			timer_isr->enable_timer_oc ((EPWMCHNL)EPWMTYPE_EMPTY, false);
			bits->stop ();
			break;
			}
		default: break;
		}
	}
}



#endif
