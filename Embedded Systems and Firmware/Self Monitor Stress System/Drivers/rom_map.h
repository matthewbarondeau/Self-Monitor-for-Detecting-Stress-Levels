/*
 * rom_map.h
 *
 *  Created on: Jan 29, 2020
 *      Author: Matthew Barondeau
 */

#ifndef DRIVERS_ROM_MAP_H_
#define DRIVERS_ROM_MAP_H_

//*****************************************************************************
//
// Macros for the I2C API.
//
//*****************************************************************************
#ifdef ROM_I2C_initMaster
#define MAP_I2C_initMaster                                                    \
        ROM_I2C_initMaster
#else
#define MAP_I2C_initMaster                                                    \
        I2C_initMaster
#endif
#ifdef ROM_I2C_initSlave
#define MAP_I2C_initSlave                                                     \
        ROM_I2C_initSlave
#else
#define MAP_I2C_initSlave                                                     \
        I2C_initSlave
#endif
#ifdef ROM_I2C_enableModule
#define MAP_I2C_enableModule                                                  \
        ROM_I2C_enableModule
#else
#define MAP_I2C_enableModule                                                  \
        I2C_enableModule
#endif
#ifdef ROM_I2C_disableModule
#define MAP_I2C_disableModule                                                 \
        ROM_I2C_disableModule
#else
#define MAP_I2C_disableModule                                                 \
        I2C_disableModule
#endif
#ifdef ROM_I2C_setSlaveAddress
#define MAP_I2C_setSlaveAddress                                               \
        ROM_I2C_setSlaveAddress
#else
#define MAP_I2C_setSlaveAddress                                               \
        I2C_setSlaveAddress
#endif
#ifdef ROM_I2C_setMode
#define MAP_I2C_setMode                                                       \
        ROM_I2C_setMode
#else
#define MAP_I2C_setMode                                                       \
        I2C_setMode
#endif
#ifdef ROM_I2C_setTimeout
#define MAP_I2C_setTimeout                                                    \
        ROM_I2C_setTimeout
#else
#define MAP_I2C_setTimeout                                                    \
        I2C_setTimeout
#endif
#ifdef ROM_I2C_slavePutData
#define MAP_I2C_slavePutData                                                  \
        ROM_I2C_slavePutData
#else
#define MAP_I2C_slavePutData                                                  \
        I2C_slavePutData
#endif
#ifdef ROM_I2C_slaveGetData
#define MAP_I2C_slaveGetData                                                  \
        ROM_I2C_slaveGetData
#else
#define MAP_I2C_slaveGetData                                                  \
        I2C_slaveGetData
#endif
#ifdef ROM_I2C_isBusBusy
#define MAP_I2C_isBusBusy                                                     \
        ROM_I2C_isBusBusy
#else
#define MAP_I2C_isBusBusy                                                     \
        I2C_isBusBusy
#endif
#ifdef ROM_I2C_masterSendSingleByte
#define MAP_I2C_masterSendSingleByte                                          \
        ROM_I2C_masterSendSingleByte
#else
#define MAP_I2C_masterSendSingleByte                                          \
        I2C_masterSendSingleByte
#endif
#ifdef ROM_I2C_masterSendSingleByteWithTimeout
#define MAP_I2C_masterSendSingleByteWithTimeout                               \
        ROM_I2C_masterSendSingleByteWithTimeout
#else
#define MAP_I2C_masterSendSingleByteWithTimeout                               \
        I2C_masterSendSingleByteWithTimeout
#endif
#ifdef ROM_I2C_masterSendMultiByteStart
#define MAP_I2C_masterSendMultiByteStart                                      \
        ROM_I2C_masterSendMultiByteStart
#else
#define MAP_I2C_masterSendMultiByteStart                                      \
        I2C_masterSendMultiByteStart
#endif
#ifdef ROM_I2C_masterSendMultiByteStartWithTimeout
#define MAP_I2C_masterSendMultiByteStartWithTimeout                           \
        ROM_I2C_masterSendMultiByteStartWithTimeout
#else
#define MAP_I2C_masterSendMultiByteStartWithTimeout                           \
        I2C_masterSendMultiByteStartWithTimeout
#endif
#ifdef ROM_I2C_masterSendMultiByteNext
#define MAP_I2C_masterSendMultiByteNext                                       \
        ROM_I2C_masterSendMultiByteNext
#else
#define MAP_I2C_masterSendMultiByteNext                                       \
        I2C_masterSendMultiByteNext
#endif
#ifdef ROM_I2C_masterSendMultiByteNextWithTimeout
#define MAP_I2C_masterSendMultiByteNextWithTimeout                            \
        ROM_I2C_masterSendMultiByteNextWithTimeout
#else
#define MAP_I2C_masterSendMultiByteNextWithTimeout                            \
        I2C_masterSendMultiByteNextWithTimeout
#endif
#ifdef ROM_I2C_masterSendMultiByteFinish
#define MAP_I2C_masterSendMultiByteFinish                                     \
        ROM_I2C_masterSendMultiByteFinish
#else
#define MAP_I2C_masterSendMultiByteFinish                                     \
        I2C_masterSendMultiByteFinish
#endif
#ifdef ROM_I2C_masterSendMultiByteFinishWithTimeout
#define MAP_I2C_masterSendMultiByteFinishWithTimeout                          \
        ROM_I2C_masterSendMultiByteFinishWithTimeout
#else
#define MAP_I2C_masterSendMultiByteFinishWithTimeout                          \
        I2C_masterSendMultiByteFinishWithTimeout
#endif
#ifdef ROM_I2C_masterSendMultiByteStop
#define MAP_I2C_masterSendMultiByteStop                                       \
        ROM_I2C_masterSendMultiByteStop
#else
#define MAP_I2C_masterSendMultiByteStop                                       \
        I2C_masterSendMultiByteStop
#endif
#ifdef ROM_I2C_masterSendMultiByteStopWithTimeout
#define MAP_I2C_masterSendMultiByteStopWithTimeout                            \
        ROM_I2C_masterSendMultiByteStopWithTimeout
#else
#define MAP_I2C_masterSendMultiByteStopWithTimeout                            \
        I2C_masterSendMultiByteStopWithTimeout
#endif
#ifdef ROM_I2C_masterReceiveStart
#define MAP_I2C_masterReceiveStart                                            \
        ROM_I2C_masterReceiveStart
#else
#define MAP_I2C_masterReceiveStart                                            \
        I2C_masterReceiveStart
#endif
#ifdef ROM_I2C_masterReceiveMultiByteNext
#define MAP_I2C_masterReceiveMultiByteNext                                    \
        ROM_I2C_masterReceiveMultiByteNext
#else
#define MAP_I2C_masterReceiveMultiByteNext                                    \
        I2C_masterReceiveMultiByteNext
#endif
#ifdef ROM_I2C_masterReceiveMultiByteFinish
#define MAP_I2C_masterReceiveMultiByteFinish                                  \
        ROM_I2C_masterReceiveMultiByteFinish
#else
#define MAP_I2C_masterReceiveMultiByteFinish                                  \
        I2C_masterReceiveMultiByteFinish
#endif
#ifdef ROM_I2C_masterReceiveMultiByteFinishWithTimeout
#define MAP_I2C_masterReceiveMultiByteFinishWithTimeout                       \
        ROM_I2C_masterReceiveMultiByteFinishWithTimeout
#else
#define MAP_I2C_masterReceiveMultiByteFinishWithTimeout                       \
        I2C_masterReceiveMultiByteFinishWithTimeout
#endif
#ifdef ROM_I2C_masterReceiveMultiByteStop
#define MAP_I2C_masterReceiveMultiByteStop                                    \
        ROM_I2C_masterReceiveMultiByteStop
#else
#define MAP_I2C_masterReceiveMultiByteStop                                    \
        I2C_masterReceiveMultiByteStop
#endif
#ifdef ROM_I2C_masterReceiveSingleByte
#define MAP_I2C_masterReceiveSingleByte                                       \
        ROM_I2C_masterReceiveSingleByte
#else
#define MAP_I2C_masterReceiveSingleByte                                       \
        I2C_masterReceiveSingleByte
#endif
#ifdef ROM_I2C_masterReceiveSingle
#define MAP_I2C_masterReceiveSingle                                           \
        ROM_I2C_masterReceiveSingle
#else
#define MAP_I2C_masterReceiveSingle                                           \
        I2C_masterReceiveSingle
#endif
#ifdef ROM_I2C_getReceiveBufferAddressForDMA
#define MAP_I2C_getReceiveBufferAddressForDMA                                 \
        ROM_I2C_getReceiveBufferAddressForDMA
#else
#define MAP_I2C_getReceiveBufferAddressForDMA                                 \
        I2C_getReceiveBufferAddressForDMA
#endif
#ifdef ROM_I2C_getTransmitBufferAddressForDMA
#define MAP_I2C_getTransmitBufferAddressForDMA                                \
        ROM_I2C_getTransmitBufferAddressForDMA
#else
#define MAP_I2C_getTransmitBufferAddressForDMA                                \
        I2C_getTransmitBufferAddressForDMA
#endif
#ifdef ROM_I2C_masterIsStopSent
#define MAP_I2C_masterIsStopSent                                              \
        ROM_I2C_masterIsStopSent
#else
#define MAP_I2C_masterIsStopSent                                              \
        I2C_masterIsStopSent
#endif
#ifdef ROM_I2C_masterIsStartSent
#define MAP_I2C_masterIsStartSent                                             \
        ROM_I2C_masterIsStartSent
#else
#define MAP_I2C_masterIsStartSent                                             \
        I2C_masterIsStartSent
#endif
#ifdef ROM_I2C_masterSendStart
#define MAP_I2C_masterSendStart                                               \
        ROM_I2C_masterSendStart
#else
#define MAP_I2C_masterSendStart                                               \
        I2C_masterSendStart
#endif
#ifdef ROM_I2C_enableMultiMasterMode
#define MAP_I2C_enableMultiMasterMode                                         \
        ROM_I2C_enableMultiMasterMode
#else
#define MAP_I2C_enableMultiMasterMode                                         \
        I2C_enableMultiMasterMode
#endif
#ifdef ROM_I2C_disableMultiMasterMode
#define MAP_I2C_disableMultiMasterMode                                        \
        ROM_I2C_disableMultiMasterMode
#else
#define MAP_I2C_disableMultiMasterMode                                        \
        I2C_disableMultiMasterMode
#endif
#ifdef ROM_I2C_enableInterrupt
#define MAP_I2C_enableInterrupt                                               \
        ROM_I2C_enableInterrupt
#else
#define MAP_I2C_enableInterrupt                                               \
        I2C_enableInterrupt
#endif
#ifdef ROM_I2C_disableInterrupt
#define MAP_I2C_disableInterrupt                                              \
        ROM_I2C_disableInterrupt
#else
#define MAP_I2C_disableInterrupt                                              \
        I2C_disableInterrupt
#endif
#ifdef ROM_I2C_clearInterruptFlag
#define MAP_I2C_clearInterruptFlag                                            \
        ROM_I2C_clearInterruptFlag
#else
#define MAP_I2C_clearInterruptFlag                                            \
        I2C_clearInterruptFlag
#endif
#ifdef ROM_I2C_getInterruptStatus
#define MAP_I2C_getInterruptStatus                                            \
        ROM_I2C_getInterruptStatus
#else
#define MAP_I2C_getInterruptStatus                                            \
        I2C_getInterruptStatus
#endif
#ifdef ROM_I2C_getEnabledInterruptStatus
#define MAP_I2C_getEnabledInterruptStatus                                     \
        ROM_I2C_getEnabledInterruptStatus
#else
#define MAP_I2C_getEnabledInterruptStatus                                     \
        I2C_getEnabledInterruptStatus
#endif
#ifdef ROM_I2C_getMode
#define MAP_I2C_getMode                                                       \
        ROM_I2C_getMode
#else
#define MAP_I2C_getMode                                                       \
        I2C_getMode
#endif
#ifdef ROM_I2C_registerInterrupt
#define MAP_I2C_registerInterrupt                                             \
        ROM_I2C_registerInterrupt
#else
#define MAP_I2C_registerInterrupt                                             \
        I2C_registerInterrupt
#endif
#ifdef ROM_I2C_unregisterInterrupt
#define MAP_I2C_unregisterInterrupt                                           \
        ROM_I2C_unregisterInterrupt
#else
#define MAP_I2C_unregisterInterrupt                                           \
        I2C_unregisterInterrupt
#endif
#ifdef ROM_I2C_slaveSendNAK
#define MAP_I2C_slaveSendNAK                                                  \
        ROM_I2C_slaveSendNAK
#else
#define MAP_I2C_slaveSendNAK                                                  \
        I2C_slaveSendNAK
#endif

#endif /* DRIVERS_ROM_MAP_H_ */
