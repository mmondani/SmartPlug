/**
 * @file TCPCommands.h
 * @author Mariano Mondani
 *
 * @brief Comandos para las conexiones TCP usados por el Smart Plug y por la App móvil.
 */


#ifndef TCP_COMMANDS_H_
#define TCP_COMMANDS_H_

// Comandos
#define CMD_GET     				0x01
#define CMD_SET						0x02
#define CMD_NODE_ON					0x10
#define CMD_NODE_OFF				0x11
#define CMD_RESET					0x20
#define CMD_RESP_GET				0x30
#define CMD_RESP_SET				0x31
#define CMD_RESP_RESET				0x32
#define CMD_RESP_NODE_ON			0x33
#define CMD_RESP_NODE_OFF			0x34


// Registros
#define REG_V_RMS						0x01
#define REG_I_RMS						0x02
#define REG_POWER_FACTOR				0x03
#define REG_FREQUENCY					0x04
#define REG_ACTIVE_POWER				0x05
#define REG_TOTAL_ENERGY				0x06
#define REG_CURRENT_HOUR_ENERGY			0x07
#define REG_CURRENT_MEASUREMENTS		0x08
#define REG_DEVICE_ID					0x10
#define REG_LOAD_STATE					0x15
#define REG_SYNCH_TIME					0x1A
#define REG_DATE_TIME					0x1B
#define REG_MONDAY_LOAD_ON_TIME			0x20
#define REG_MONDAY_LOAD_OFF_TIME		0x21
#define REG_TUESDAY_LOAD_ON_TIME		0x22
#define REG_TUESDAY_LOAD_OFF_TIME		0x23
#define REG_WEDNESDAY_LOAD_ON_TIME		0x24
#define REG_WEDNESDAY_LOAD_OFF_TIME		0x25
#define REG_THURSDAY_LOAD_ON_TIME		0x26
#define REG_THURSDAY_LOAD_OFF_TIME		0x27
#define REG_FRIDAY_LOAD_ON_TIME			0x28
#define REG_FRIDAY_LOAD_OFF_TIME		0x29
#define REG_SATURDAY_LOAD_ON_TIME		0x2A
#define REG_SATURDAY_LOAD_OFF_TIME		0x2B
#define REG_SUNDAY_LOAD_ON_TIME			0x2C
#define REG_SUNDAY_LOAD_OFF_TIME		0x2D
#define REG_ENABLE_ONOFF_TIME			0x2E
#define REG_ONOFF_TIME_TIMES			0x2F
#define REG_PER_HOUR_ENERGY				0x30
#define REG_PER_HOUR_ACTIVE_POWER		0x31
#define REG_ALL_REGISTERS				0x70




#endif
