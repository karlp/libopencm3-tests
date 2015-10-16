/* 
 * File:   adc-power.h
 * Author: karlp
 *
 * Created on October 17, 2015, 12:19 AM
 */

#ifndef ADC_POWER_H
#define	ADC_POWER_H

#ifdef	__cplusplus
extern "C" {
#endif

	void adc_power_init(void);
	void adc_power_task_up(void);
	void adc_power_task_down(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_POWER_H */

