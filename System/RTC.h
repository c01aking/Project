#ifndef __RTC_H
#define __RTC_H

void RTC_Init(void);
void RTC_Record_Time(uint32_t *timestamp, uint16_t **p_time);
void RTC_ReadTime(uint16_t **p_time);

#endif /*__RTC_H*/
