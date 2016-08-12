#ifndef STEPPER_H_
#define STEPPER_H_


#define STEPS_PER_CLICK 400  // 14,000/CLICKS_PER_WRAP
#define FORWARD 1
#define BACKWARD -1

void setDirection();
void step(int);
int findHome(void);
void initStepper(void);


extern volatile int stepper_pos, step_count, stepper_dir, stepper_en;



#endif
