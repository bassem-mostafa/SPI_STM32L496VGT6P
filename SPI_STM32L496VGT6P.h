// #############################################################################
// #### Copyright ##############################################################
// #############################################################################

/*
 * Copyright 2024 BaSSeM
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// #############################################################################
// #### Description ############################################################
// #############################################################################

/**
 *  @file
 *
 *  @brief Platform SPI STM32L496VGT6P Driver
 */

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

/**
 *  @addtogroup Platform_SPI_Driver
 *
 *  @{
 */

/**
 *  @defgroup Platform_SPI_STM32L496VGT6P STM32L496VGT6P
 *
 *  @{
 */

#ifndef SPI_STM32L496VGT6P_H_
    #define SPI_STM32L496VGT6P_H_

    #ifdef __cplusplus
extern "C"
{
    #endif /* __cplusplus */

    // #############################################################################
    // #### Include(s) #############################################################
    // #############################################################################

    // #############################################################################
    // #### Public Macro(s) ########################################################
    // #############################################################################

    // #############################################################################
    // #### Public Type(s) #########################################################
    // #############################################################################

    /**
     *  @brief SPI STM32L496VGT6P Operation Status
     *
     *  @enum SPI_STM32L496VGT6P_Status_t
     */
    typedef enum SPI_STM32L496VGT6P_Status
    {
        SPI_STM32L496VGT6P_Status_Success = 0,     ///< Success
        SPI_STM32L496VGT6P_Status_ArgumentInvalid, ///< Argument Invalid
        SPI_STM32L496VGT6P_Status_NotSupported,    ///< Not Supported
        SPI_STM32L496VGT6P_Status_Error,           ///< General Error
        SPI_STM32L496VGT6P_Status_Busy,            ///< Busy
        SPI_STM32L496VGT6P_Status_Timeout,         ///< Timeout
    } SPI_STM32L496VGT6P_Status_t;

    /**
     *  @brief SPI STM32L496VGT6P
     *
     *  @enum SPI_STM32L496VGT6P_t
     */
    typedef enum SPI_STM32L496VGT6P
    {
        SPI_STM32L496VGT6P_1 = 0, ///< SPI 1 (Minimum)
        SPI_STM32L496VGT6P_2,     ///<
        SPI_STM32L496VGT6P_3,     ///< SPI 3 (Maximum)
        SPI_STM32L496VGT6P_Count, ///< Count
    } SPI_STM32L496VGT6P_t;

    /**
     *  @brief SPI STM32L496VGT6P Instance (Forward Declaration)
     */
    typedef struct SPI_STM32L496VGT6P_Instance SPI_STM32L496VGT6P_Instance_t;

    /**
     *  @brief SPI STM32L496VGT6P Callback On Complete
     */
    typedef void ( *SPI_STM32L496VGT6P_CallbackOnComplete_t )( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Status_t Status );

    /**
     *  @brief SPI STM32L496VGT6P Instance Context
     *
     *  @struct SPI_STM32L496VGT6P_InstanceContext_t
     */
    typedef struct SPI_STM32L496VGT6P_InstanceContext SPI_STM32L496VGT6P_InstanceContext_t;

    /**
     *  @brief SPI STM32L496VGT6P Instance
     *
     *  @struct SPI_STM32L496VGT6P_Instance_t
     */
    typedef struct SPI_STM32L496VGT6P_Instance
    {
        SPI_STM32L496VGT6P_t SPIx;

        GPIO_t SCK;
        GPIO_t MOSI;
        GPIO_t MISO;

        SPI_STM32L496VGT6P_CallbackOnComplete_t OnComplete;

        SPI_STM32L496VGT6P_InstanceContext_t * Context;
    } SPI_STM32L496VGT6P_Instance_t;

    /**
     *  @brief SPI STM32L496VGT6P Data
     */
    typedef uint8_t SPI_STM32L496VGT6P_Data_t;

    /**
     *  @brief SPI STM32L496VGT6P Data Length
     */
    typedef uint32_t SPI_STM32L496VGT6P_DataLength_t;

    // #############################################################################
    // #### Public Method(s) #######################################################
    // #############################################################################

    /**
     *  @brief Initializes specified SPI STM32L496VGT6P Instance
     *
     *  @param[in] Instance Instance
     *
     *  @return SPI_STM32L496VGT6P_Status_t
     */
    SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Initialize( SPI_STM32L496VGT6P_Instance_t * Instance );

    /**
     *  @brief Cycles specified SPI STM32L496VGT6P Instance
     *
     *  @param[in] Instance Instance
     *
     *  @return SPI_STM32L496VGT6P_Status_t
     */
    SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Cycle( SPI_STM32L496VGT6P_Instance_t * Instance );

    /**
     *  @brief De-initializes specified SPI STM32L496VGT6P Instance
     *
     *  @param[in] Instance Instance
     *
     *  @return SPI_STM32L496VGT6P_Status_t
     */
    SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_DeInitialize( SPI_STM32L496VGT6P_Instance_t * Instance );

    /**
     *  @brief Writes data to specified SPI STM32L496VGT6P Instance
     *
     *  @param[in] Instance   Instance
     *  @param[in] Data       Data buffer
     *  @param[in] DataLength Length of data buffer
     *
     *  @return SPI_STM32L496VGT6P_Status_t
     */
    SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Write( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength );

    /**
     *  @brief Reads data from specified SPI STM32L496VGT6P Instance
     *
     *  @param[in] Instance   Instance
     *  @param[in] Data       Data buffer
     *  @param[in] DataLength Length of data buffer
     *
     *  @return SPI_STM32L496VGT6P_Status_t
     */
    SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Read( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength );

    /**
     *  @brief Initializes specified SPI STM32L496VGT6P Instance
     *
     *  @param[in] Instance     Instance
     *  @param[in] DataTx       Transmit data buffer
     *  @param[in] DataTxLength Length of transmit data buffer
     *  @param[in] DataRx       Receive data buffer
     *  @param[in] DataRxLength Length of receive data buffer
     *
     *  @return SPI_STM32L496VGT6P_Status_t
     */
    SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Transaction( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * DataTx, SPI_STM32L496VGT6P_DataLength_t DataTxLength, SPI_STM32L496VGT6P_Data_t * DataRx, SPI_STM32L496VGT6P_DataLength_t DataRxLength );

    // #############################################################################
    // #### Public Variable(s) #####################################################
    // #############################################################################

    /**
     *  @brief Version
     */
    extern const char SPI_STM32L496VGT6P_VERSION[];

    // #############################################################################
    // #### File Guard #############################################################
    // #############################################################################

    #ifdef __cplusplus
} /* extern "C" */
    #endif /* __cplusplus */

#endif /* SPI_STM32L496VGT6P_H_ */

/**
 *  @}
 *
 *  @}
 */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
