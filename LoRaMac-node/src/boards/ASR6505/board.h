/*!
 * \file      board.h
 *
 * \brief     Target board general functions implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdbool.h>
#include <stdint.h>

#include "board-config.h"

extern bool is_controller;
extern bool is_terminal_emergency;
extern bool is_terminal_normal;

extern bool led_busy_flag;
/*!
 * Possible power sources
 */
enum BoardPowerSources {
    USB_POWER = 0,
    BATTERY_POWER
};

/*!
 * Board Version
 */
typedef union BoardVersion_u {
    struct BoardVersion_s {
        uint8_t Rfu;
        uint8_t Revision;
        uint8_t Minor;
        uint8_t Major;
    } Fields;
    uint32_t Value;
} BoardVersion_t;

void message_green_led(void);
void power_on_led(void);
void power_off_led(void);

    /*!
     * \brief Disable interrupts
     *
     * \remark IRQ nesting is managed
     */
    void BoardDisableIrq(void);

/*!
 * \brief Enable interrupts
 *
 * \remark IRQ nesting is managed
 */
void BoardEnableIrq(void);

/*!
 * \brief Initializes the mcu.
 */
void BoardInitMcu(void);

/*!
 * \brief Resets the mcu.
 */
void BoardResetMcu(void);

/*!
 * \brief Initializes the boards peripherals.
 */
void BoardInitPeriph(void);

/*!
 * \brief De-initializes the target board peripherals to decrease power
 *        consumption.
 */
void BoardDeInitMcu(void);

/*!
 * \brief Gets the current potentiometer level value
 *
 * \retval value  Potentiometer level ( value in percent )
 */
uint8_t BoardGetPotiLevel(void);

/*!
 * \brief Measure the Battery voltage
 *
 * \retval value  battery voltage in volts
 */
uint32_t BoardGetBatteryVoltage(void);

/*!
 * \brief Get the current battery level
 *
 * \retval value  battery level [  0: USB,
 *                                 1: Min level,
 *                                 x: level
 *                               254: fully charged,
 *                               255: Error]
 */
uint8_t BoardGetBatteryLevel(void);

/*!
 * Returns a pseudo random seed generated using the MCU Unique ID
 *
 * \retval seed Generated pseudo random seed
 */
uint32_t BoardGetRandomSeed(void);

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId(uint8_t *id);

/*!
 * \brief Get the board power source
 *
 * \retval value  power source [0: USB_POWER, 1: BATTERY_POWER]
 */
uint8_t GetBoardPowerSource(void);

/*!
 * \brief Get the board version
 *
 * \retval value  Version
 */
BoardVersion_t BoardGetVersion(void);

void BoardAllIoInit(void);
void BoardUnusedIoInit(void);
void DisableUnusedPeripherals(void);
void Output_Pin_Init(void);
void UartEnterLowPowerStopMode(void);
void UartLowPowerHandler(void);
void DisableLowPowerDuringTask(bool status);
void McuEnterLowPowerStopMode(void);
void RTC_Config(void);
 extern volatile bool McuStopFlag;

#endif // __BOARD_H__
