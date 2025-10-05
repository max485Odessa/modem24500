#include "ITIMINT.h"
#include "TGlobalISR.h"

static const uint32_t chanarr[4] = {TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4};
static const uint32_t ocmode[2] = {TIM_OCMODE_PWM1, TIM_OCMODE_PWM2};


#ifdef __cplusplus
 extern "C" {
#endif 

	 

static void TimIsr (TTIM_ISR *o)
{
	TTIM_ISR &obj = *o;
	obj.isr_tim ();
}	


#ifdef TIM1
void TIM1_IRQHandler ()
{
	void *cb = hard_timer_procedure_this_get (ESYSTIM_1);
	if (cb) TimIsr ((TTIM_ISR*)cb);
}
#endif

	 
#ifdef TIM2
void TIM2_IRQHandler ()
{
	void *cb = hard_timer_procedure_this_get (ESYSTIM_2);
	if (cb) TimIsr ((TTIM_ISR*)cb);
}
#endif


#ifdef TIM3
void TIM3_IRQHandler ()
{
	void *cb = hard_timer_procedure_this_get (ESYSTIM_3);
	if (cb) TimIsr ((TTIM_ISR*)cb);
}
#endif


#ifdef TIM4
void TIM4_IRQHandler ()
{
	void *cb = hard_timer_procedure_this (ESYSTIM_4);
	if (cb) TimIsr ((TTIM_MKS_ISR*)cb);
}
#endif


#ifdef TIM5
void TIM5_IRQHandler ()
{
	void *cb = hard_timer_procedure_this (ESYSTIM_5);
	if (cb) TimIsr ((TTIM_MKS_ISR*)cb);
}
#endif


#ifdef __cplusplus
}
#endif


TTIM_USER_A::TTIM_USER_A (ESYSTIM t, uint32_t prd, uint32_t fr) : c_value_period (prd), c_value_freq (fr)
{
tim_ix = t;
TimHandle.Instance = hard_get_tim (t, 0);
timer_init (c_value_period, c_value_freq);
}


ESYSTIM TTIM_USER_A::get_tim ()
{
	return tim_ix;
}



void TTIM_USER_A::timer_init (uint32_t period, uint32_t hz)
{
//TIM_OC_InitTypeDef   sConfig;
uint32_t uwPrescalerValue = (uint32_t) ((SystemCoreClock / hz) - 1);
hard_tim_clock_enable (tim_ix);
  
  TimHandle.Init.Period = period - 1;
  TimHandle.Init.Prescaler = uwPrescalerValue;
  TimHandle.Init.ClockDivision = 0;
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
	//HAL_TIM_Base_Start_IT(&TimHandle);
	HAL_TIM_OC_Init (&TimHandle);
	//HAL_TIM_OC_Init (&TimHandle);
	//HAL_TIM_Base_Start (&TimHandle);
}



uint32_t TTIM_USER_A::get_timer_counter ()
{
	return TimHandle.Instance->CNT;
}



void TTIM_USER_A::set_timer_counter (uint32_t v)
{
 TimHandle.Instance->CNT = v;
}




uint32_t TTIM_USER_A::get_delta (uint32_t prv, uint32_t cur)
{
uint32_t rv;
if (prv <= cur)
	{
	rv = cur - prv;
	}
else
	{
	rv = c_value_period - prv + cur;
	}
return rv;
}



uint32_t TTIM_USER_A::get_period ()
{
	return c_value_period;
}



uint32_t TTIM_USER_A::get_freq ()
{
	return c_value_freq;
}



void TTIM_USER_A::clr_tim ()
{
	TimHandle.Instance->CNT = 0;
}



TTIM_ISR::TTIM_ISR (ESYSTIM t, uint32_t prd, uint32_t fr) : TTIM_USER_A (t, prd, fr)
{
	hard_timer_procedure_this_set (t, this);
	uint8_t ix = 0;
	while (ix < EPWMCHNL_ENDENUM)
		{
		f_active_isr[ix] = false;
		a_pwmvalue[ix] = 0;
		ix++;
		}
}



void TTIM_ISR::set_tim_cb (EPWMCHNL c, ITIMCB *cb)
{
	if (c < EPWMCHNL_ENDENUM) callback_user[c] = cb;
}




// isr context executed
void TTIM_ISR::isr_tim ()
{
	//TGLOBISR::disable ();
	uint32_t sr = TimHandle.Instance->SR;
	//if(__HAL_TIM_GET_FLAG (&TimHandle, TIM_IT_CC1) !=RESET)
	if(sr & TIM_IT_CC1)
    {
			if (f_one_short[EPWMCHNL_PWM1]) enable_timer_oc (EPWMCHNL_PWM1, false);
			if (callback_user[EPWMCHNL_PWM1]) callback_user[EPWMCHNL_PWM1]->tim_comp_cb_user_isr (tim_ix, EPWMCHNL_PWM1);
		 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC1);
		}
	if(sr & TIM_IT_CC2)
    {
			if (f_one_short[EPWMCHNL_PWM2]) enable_timer_oc (EPWMCHNL_PWM2, false);
			if (callback_user[EPWMCHNL_PWM2])callback_user[EPWMCHNL_PWM2]->tim_comp_cb_user_isr (tim_ix, EPWMCHNL_PWM2);
		 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC2);
		}
	if(sr & TIM_IT_CC3)
    {
			if (f_one_short[EPWMCHNL_PWM3]) enable_timer_oc (EPWMCHNL_PWM3, false);
			if (callback_user[EPWMCHNL_PWM3])callback_user[EPWMCHNL_PWM3]->tim_comp_cb_user_isr (tim_ix, EPWMCHNL_PWM3);
		 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC3);
		}
	if(sr & TIM_IT_CC4)
    {
			if (f_one_short[EPWMCHNL_PWM4]) enable_timer_oc (EPWMCHNL_PWM4, false);
			if (callback_user[EPWMCHNL_PWM4])callback_user[EPWMCHNL_PWM4]->tim_comp_cb_user_isr (tim_ix, EPWMCHNL_PWM4);
		 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC4);
		}
	if(sr & TIM_IT_UPDATE)
    {
			if (callback_user[EPWMCHNL_UPDATE])callback_user[EPWMCHNL_UPDATE]->tim_comp_cb_user_isr (tim_ix, EPWMCHNL_UPDATE);
		 __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_UPDATE);
		}
	//TGLOBISR::enable ();
}




void TTIM_ISR::enable_timer_isr (bool st)
{
	IRQn_Type tp = (IRQn_Type)(hard_tim_irq_type (tim_ix));
	if (st)
		{
		if (tp)HAL_NVIC_EnableIRQ (tp);
		}
	else
		{
		if (tp)HAL_NVIC_DisableIRQ (tp);
		}
}



void TTIM_ISR::set_timer_oc_value (EPWMCHNL c, uint32_t v)
{
	if (c < EPWMCHNL_ENDENUM) 
		{
		a_pwmvalue[c] = v;
		__HAL_TIM_SET_COMPARE (&TimHandle, chanarr[c], v);
		}
}



void TTIM_ISR::start_one_short (EPWMCHNL c, uint32_t dly_mks)
{
	if (c < EPWMCHNL_ENDENUM)
		{
		f_one_short[c] = true;
		uint32_t nw = get_timer_counter () + dly_mks;
		set_timer_oc_value (c, nw);
		enable_timer_oc (c, true);
		}
}



void TTIM_ISR::enable_timer_oc (EPWMCHNL c, bool state)
{
	if (c < EPWMCHNL_ENDENUM)
		{
		TIM_OC_InitTypeDef sConfig;
		if (c < EPWMCHNL_UPDATE)
			{
			if (state)
				{

				sConfig.OCMode  = TIM_OCMODE_ACTIVE;//TIM_OCMODE_FORCED_ACTIVE;//TIM_OCMODE_ACTIVE;//ocmode[c]; //TIM_OCMODE_PWM1;//TIM_OCMODE_ACTIVE;
				sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;

				sConfig.Pulse = a_pwmvalue[c];  
				HAL_TIM_OC_ConfigChannel (&TimHandle, &sConfig, chanarr[c]);
				HAL_TIM_OC_Start_IT (&TimHandle, chanarr[c]);
				}
			else
				{
				HAL_TIM_OC_Stop_IT (&TimHandle, chanarr[c]);
				}
			f_active_isr[c] = state;
			}
		else
			{
			if (c == EPWMCHNL_UPDATE)
				{
				f_active_isr[c] = state;
					
				HAL_TIM_Base_Start_IT(&TimHandle);
				//__HAL_TIM_ENABLE_IT (&TimHandle, TIM_IT_UPDATE);
				}
			}
		}
}





