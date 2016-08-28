/**
 * @file EEPROMMap.h
 * @author Mariano Mondani
 *
 * @brief Mapa de la memoria EEPROM externa
 */


#ifndef EEPROM_MAP_H_
#define EEPROM_MAP_H_

#define EE_DEVICE_ID						0
#define EE_MONDAY_LOAD_ON_TIME				33
#define EE_TUESDAY_LOAD_ON_TIME				35
#define EE_WEDNESDAY_LOAD_ON_TIME			37
#define EE_THURSDAY_LOAD_ON_TIME			39
#define EE_FRIDAY_LOAD_ON_TIME				41
#define EE_SATURDAY_LOAD_ON_TIME			43
#define EE_SUNDAY_LOAD_ON_TIME				45
#define EE_MONDAY_LOAD_OFF_TIME				47
#define EE_TUESDAY_LOAD_OFF_TIME			49
#define EE_WEDNESDAY_LOAD_OFF_TIME			51
#define EE_THURSDAY_LOAD_OFF_TIME			53
#define EE_FRIDAY_LOAD_OFF_TIME				55
#define EE_SATURDAY_LOAD_OFF_TIME			57
#define EE_SUNDAY_LOAD_OFF_TIME				59
#define EE_ENABLE_ONOFF_TIME				61
#define EE_BLOCK_PTR						62
#define EE_ACTIVE_POWER_DATE				64
#define EE_ACTIVE_POWER_HOUR_00				67
#define EE_ACTIVE_POWER_HOUR_12				128
#define EE_ENERGY_DATE						1088
#define EE_ENERGY_HOUR_00					1091
#define EE_ENERGY_HOUR_12					1152
#define EE_ACUM_ENERGY						2240
#define EE_VDC_OFFSET						5760
#define EE_IDC_OFFSET						5764
#define EE_VAC_GAIN							5768
#define EE_IAC_GAIN							5772
#define EE_INIT_FLAG						6400



#endif
