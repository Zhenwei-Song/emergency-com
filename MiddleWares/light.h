#ifndef __LIGHT_H
#define __LIGHT_H

#include <stdint.h>

void ledRB_toggle(void);
void ledB_toggle(void);
void ledG_toggle(void);
void ledR_toggle(void);
void ledY_toggle(void);

void ledB_LR_toggle(void);
void ledG_LR_toggle(void);
void ledR_LR_toggle(void);
void ledY_LR_toggle(void);

void ledB_Fast_toggle(void);
void ledG_Fast_toggle(void);
void ledR_Fast_toggle(void);
void ledY_Fast_toggle(void);

void ledGY_toggle(void);
void ledBY_toggle(void);
void ledRY_toggle(void);

void all_light_Off(void);
void all_power_Off(void);
void all_power_on(void);

void Select_light(uint8_t lightnum);


#endif
