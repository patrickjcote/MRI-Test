/*
 * sampler.h
 */

#ifndef SAMPLER_SAMPLER_H_
#define SAMPLER_SAMPLER_H_

/*Defining input/output ports.bit for sampler functionality*/
#define PUMP	BIT3	//Define pump ON/OFF bit (P1.3)
#define PUMPD	BIT4	//Define pump direction bit (P1.4)
#define PUMPG	BIT5	//Define pump photogate bit (P1.5)
#define volscale  0.43	//Define number of clicks per milliliter of the pump
#define ARMR	BIT0	//Define arm movement direction Reverse (P2.0)
#define ARMF	BIT1	//Define arm movement direction Forward (P2.1)
#define ARMPG	BIT3	//Define arm photogate bit (P2.2)


/*External Variables Initialization:
 *pump_vol: pumping volume(in milliliters),	pump_dir: pumping direction(forward/reverse)
 *new-bottle: value entered by user(1-24),	cur_bottle: bottle where arm is placed
 *CS_val: ADC value of current sensor input voltage,	ARM_TO: arm timeout(eliminate false triggering of current sensor)
 *status: status of sampler(0:Ready, 1:pumping forward, 2: pumping reverse, 3:arm moving, 4: arm resetting, 5: ALL_STOP_FLAG triggered)
 *Flags: to activate desired operations and eliminate interference between functions
 *		arm_flag: activates arm function to move arm to desired bottle
 *		arm_reset_flag: resets arm position to bottle #1
 *		pump_flag: activates pump function to fill/drain desired volume
 *		ALL_STOP_FLAG: allows stopping of all operations by user
 */
extern volatile int pump_vol, pump_dir, pump_flag, ALL_STOP_FLAG, status;
extern volatile int new_bottle, cur_bottle, arm_flag, arm_reset_flag, CS_val;
extern unsigned int ARM_TO;

/*sampler.c Functions' Prototypes*/
void ADC_init(void);
int ADC_val(void);
void sampler_init(void);
void pump_water(int,int);
void arm_position(int);
void arm_reset(void);


#endif
