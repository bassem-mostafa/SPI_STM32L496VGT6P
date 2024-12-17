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

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

#include "Platform.h"

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #undef DEBUG
#endif

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#ifdef STM32L496xx

// #############################################################################
// #### Include(s) #############################################################
// #############################################################################

    #include "../../SPI_Internal.h"
    #include "SPI_STM32L496VGT6P.h"

    #include "stm32l4xx.h"
    #include "stm32l4xx_hal_spi.h"

// #############################################################################
// #### Private Macro(s) #######################################################
// #############################################################################

    #define SPI_STM32L496VGT6P_TIMEOUT_BYTE_TX    ( 5 ) // in milliseconds
    #define SPI_STM32L496VGT6P_TIMEOUT_BYTE_RX    ( 5 ) // in milliseconds
    #define SPI_STM32L496VGT6P_TIMEOUT_BYTE_TX_RX ( SPI_STM32L496VGT6P_TIMEOUT_BYTE_TX + SPI_STM32L496VGT6P_TIMEOUT_BYTE_RX )

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

typedef enum SPI_STM32L496VGT6P_Event
{
    SPI_STM32L496VGT6P_Event_None = 0,
    SPI_STM32L496VGT6P_Event_TxComplete = UTIL_BIT( 0 ),
    SPI_STM32L496VGT6P_Event_RxComplete = UTIL_BIT( 1 ),
    SPI_STM32L496VGT6P_Event_Abort = UTIL_BIT( 2 ),
    SPI_STM32L496VGT6P_Event_Error = UTIL_BIT( 3 ),
} SPI_STM32L496VGT6P_Event_t;

typedef struct SPI_STM32L496VGT6P_Instance_Context
{
    SPI_HandleTypeDef SPIx;
    SPI_STM32L496VGT6P_Event_t Event;
} SPI_STM32L496VGT6P_Instance_Context_t;

typedef struct SPI_STM32L496VGT6P_Context
{
    TIM_Timestamp_t Timestamp;
    SPI_STM32L496VGT6P_Instance_Context_t Context[ SPI_STM32L496VGT6P_Count ];
} SPI_STM32L496VGT6P_Context_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

void HAL_SPI_TxCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_RxCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_TxRxCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_TxHalfCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_RxHalfCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_TxRxHalfCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_ErrorCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_AbortCpltCallback( SPI_HandleTypeDef * hspi );

void SPI1_IRQHandler( void );
void SPI2_IRQHandler( void );
void SPI3_IRQHandler( void );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Write( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Read( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Commit( SPI_STM32L496VGT6P_Instance_t * Instance );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Initialize( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Cycle( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_DeInitialize( SPI_STM32L496VGT6P_Instance_t * Instance );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Initialize( void );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Cycle( void );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_DeInitialize( void );

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

static SPI_STM32L496VGT6P_Context_t SPI_STM32L496VGT6P_Context;

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

void HAL_SPI_TxCpltCallback( SPI_HandleTypeDef * hspi )
{
    for ( SPI_STM32L496VGT6P_t SPI_x = SPI_STM32L496VGT6P_1; SPI_x < SPI_STM32L496VGT6P_Count; ++SPI_x )
    {
        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_x ];

        if ( &Context->SPIx == hspi )
        {
            Context->Event |= SPI_STM32L496VGT6P_Event_TxComplete;
            break;
        }
    }
}

void HAL_SPI_RxCpltCallback( SPI_HandleTypeDef * hspi )
{
    for ( SPI_STM32L496VGT6P_t SPI_x = SPI_STM32L496VGT6P_1; SPI_x < SPI_STM32L496VGT6P_Count; ++SPI_x )
    {
        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_x ];

        if ( &Context->SPIx == hspi )
        {
            Context->Event |= SPI_STM32L496VGT6P_Event_RxComplete;
            break;
        }
    }
}

void HAL_SPI_TxRxCpltCallback( SPI_HandleTypeDef * hspi )
{
    for ( SPI_STM32L496VGT6P_t SPI_x = SPI_STM32L496VGT6P_1; SPI_x < SPI_STM32L496VGT6P_Count; ++SPI_x )
    {
        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_x ];

        if ( &Context->SPIx == hspi )
        {
            Context->Event |= SPI_STM32L496VGT6P_Event_TxComplete | SPI_STM32L496VGT6P_Event_RxComplete;
            break;
        }
    }
}

void HAL_SPI_TxHalfCpltCallback( SPI_HandleTypeDef * hspi )
{
}

void HAL_SPI_RxHalfCpltCallback( SPI_HandleTypeDef * hspi )
{
}

void HAL_SPI_TxRxHalfCpltCallback( SPI_HandleTypeDef * hspi )
{
}

void HAL_SPI_ErrorCallback( SPI_HandleTypeDef * hspi )
{
    for ( SPI_STM32L496VGT6P_t SPI_x = SPI_STM32L496VGT6P_1; SPI_x < SPI_STM32L496VGT6P_Count; ++SPI_x )
    {
        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_x ];

        if ( &Context->SPIx == hspi )
        {
            Context->Event |= SPI_STM32L496VGT6P_Event_Error;
            break;
        }
    }
}

void HAL_SPI_AbortCpltCallback( SPI_HandleTypeDef * hspi )
{
    for ( SPI_STM32L496VGT6P_t SPI_x = SPI_STM32L496VGT6P_1; SPI_x < SPI_STM32L496VGT6P_Count; ++SPI_x )
    {
        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_x ];

        if ( &Context->SPIx == hspi )
        {
            Context->Event |= SPI_STM32L496VGT6P_Event_Abort;
            break;
        }
    }
}

void SPI1_IRQHandler( void )
{
    HAL_SPI_IRQHandler( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_1 ].SPIx );
}

void SPI2_IRQHandler( void )
{
    HAL_SPI_IRQHandler( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ].SPIx );
}

void SPI3_IRQHandler( void )
{
    HAL_SPI_IRQHandler( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ].SPIx );
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Write( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;

    do
    {
        SPI_Trace( "%s( Instance=%p, Data=%p, Length=%d )", __FUNCTION__, Instance, Data, DataLength );

        if ( Instance == NULL || Data == NULL || DataLength < 1 )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        // TODO Make Use Of DMA
    #if 0
    while ( ( HAL_Status = HAL_SPI_Transmit_DMA( &Instance->Context->SPIx, Data, DataLength ) ) != HAL_OK )
    {
//      Status = SPI_STM32L496VGT6P_Status_Error;
//      break;
    }
    #else
        if ( ( HAL_Status = HAL_SPI_Transmit( &Instance->Context->SPIx, Data, DataLength, SPI_STM32L496VGT6P_TIMEOUT_BYTE_TX * DataLength ) ) != HAL_OK )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    #endif

        Status = SPI_STM32L496VGT6P_Status_Success;
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Read( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p, Data=%p, Length=%d )", __FUNCTION__, Instance, Data, DataLength );

        if ( Instance == NULL || Data == NULL || DataLength < 1 )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        // TODO Make Use Of DMA
    #if 0
    while ( ( HAL_Status = HAL_SPI_Receive_DMA( &Instance->Context->SPIx, Data, DataLength ) ) != HAL_OK )
    {
//      Status = SPI_STM32L496VGT6P_Status_Error;
//      break;
    }
    #else
        if ( ( HAL_Status = HAL_SPI_Receive( &Instance->Context->SPIx, Data, DataLength, SPI_STM32L496VGT6P_TIMEOUT_BYTE_RX * DataLength ) ) != HAL_OK )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    #endif

        Status = SPI_STM32L496VGT6P_Status_Success;
    }
    while ( 0 );
    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Transaction( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * DataTx, SPI_STM32L496VGT6P_DataLength_t DataTxLength, SPI_STM32L496VGT6P_Data_t * DataRx, SPI_STM32L496VGT6P_DataLength_t DataRxLength )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;

    do
    {
        SPI_Trace( "%s( Instance=%p, TX={Data=%p, Length=%d} RX={Data=%p, Length=%d} )", __FUNCTION__, Instance, DataTx, DataTxLength, DataRx, DataRxLength );

        if ( Instance == NULL || DataTx == NULL || DataTxLength < 1 || DataRx == NULL || DataRxLength < 1 )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        // TODO Make Use Of DMA
    #if 0
    while ( ( HAL_Status = HAL_SPI_TransmitReceive_DMA( &Instance->Context->SPIx, DataTx, DataRx, UTIL_Max( DataTxLength, DataRxLength ) ) ) != HAL_OK )
    {
//      Status = SPI_STM32L496VGT6P_Status_Error;
//      break;
    }
    #else
        if ( ( HAL_Status = HAL_SPI_TransmitReceive( &Instance->Context->SPIx, DataTx, DataRx, UTIL_Max( DataTxLength, DataRxLength ), SPI_STM32L496VGT6P_TIMEOUT_BYTE_TX_RX * UTIL_Max( DataTxLength, DataRxLength ) ) ) != HAL_OK )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    #endif

        Status = SPI_STM32L496VGT6P_Status_Success;
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Commit( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        Status = SPI_STM32L496VGT6P_Status_Success;

        // FIXME Keep CubeMX generated configurations as is for now
    #if 0
    HAL_StatusTypeDef HAL_Status = HAL_ERROR;
    if ( ( HAL_Status = HAL_SPI_Init( &Instance->Context->SPIx ) ) != HAL_OK )
    {
      Status = SPI_STM32L496VGT6P_Status_Error;
      break;
    }
    #endif
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Initialize( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];

        Status = SPI_STM32L496VGT6P_Status_Success;

        // FIXME Keep CubeMX generated configurations as is for now
        switch ( Instance->SPIx )
        {
            case SPI_STM32L496VGT6P_1:
                extern SPI_HandleTypeDef hspi1;
                Context->SPIx = hspi1;
                break;

            case SPI_STM32L496VGT6P_2:
                extern SPI_HandleTypeDef hspi2;
                Context->SPIx = hspi2;
                break;

            case SPI_STM32L496VGT6P_3:
                extern SPI_HandleTypeDef hspi3;
                Context->SPIx = hspi3;
                break;

            default:
                Status = SPI_STM32L496VGT6P_Status_NotSupported;
                break;
        }
        if ( Status != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Instance->Context = Context;

        if ( ( Status = SPI_STM32L496VGT6P_Instance_Commit( Instance ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Cycle( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        Status = SPI_STM32L496VGT6P_Status_Success;
    }
    while ( 0 );
    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_DeInitialize( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        // TODO De-Initialize & Disable SPI
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Initialize( void )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;

    do
    {
        SPI_Trace( "%s( void )", __FUNCTION__ );

        // FIXME Remove the usage of `MX_SPIx_Init()`
        // FIXME Keep CubeMX generated configurations as is for now
    #if 1
        extern SPI_HandleTypeDef hspi1;
        extern DMA_HandleTypeDef hdma_spi1_tx;
        extern void MX_SPI1_Init( void );
        MX_SPI1_Init( );
        SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_1 ].SPIx = hspi1;
            // TODO Make Use Of DMA
        #if 0
    __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_1 ].SPIx, hdmatx, hdma_spi1_tx );
        #endif

        extern SPI_HandleTypeDef hspi2;
        extern DMA_HandleTypeDef hdma_spi2_tx;
        extern DMA_HandleTypeDef hdma_spi2_rx;
        extern void MX_SPI2_Init( void );
        MX_SPI2_Init( );
        SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ].SPIx = hspi2;
            // TODO Make Use Of DMA
        #if 0
    __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ].SPIx, hdmatx, hdma_spi2_tx );
    __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ].SPIx, hdmarx, hdma_spi2_rx );
        #endif

        extern SPI_HandleTypeDef hspi3;
        extern DMA_HandleTypeDef hdma_spi3_tx;
        extern DMA_HandleTypeDef hdma_spi3_rx;
        extern void MX_SPI3_Init( void );
        MX_SPI3_Init( );
        SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ].SPIx = hspi3;
            // TODO Make Use Of DMA
        #if 0
    __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ].SPIx, hdmatx, hdma_spi3_tx );
    __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ].SPIx, hdmarx, hdma_spi3_rx );
        #endif
    #endif

        Status = SPI_STM32L496VGT6P_Status_Success;
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Cycle( void )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( void )", __FUNCTION__ );
        UTIL_UNUSED( SPI_STM32L496VGT6P_Context ); // FIXME Skip Warning
        // Nothing to be done
        Status = SPI_STM32L496VGT6P_Status_Success;
    }
    while ( 0 );
    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_DeInitialize( void )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( void )", __FUNCTION__ );
        UTIL_UNUSED( SPI_STM32L496VGT6P_Context ); // FIXME Skip Warning
        // Nothing to be done
        Status = SPI_STM32L496VGT6P_Status_Success;
    }
    while ( 0 );
    return Status;
}

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Initialize( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = SPI_STM32L496VGT6P_Context_Initialize( ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }
        Status = SPI_STM32L496VGT6P_Instance_Initialize( Instance );
    }
    while ( 0 );
    return Status;
}

SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Cycle( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = SPI_STM32L496VGT6P_Context_Cycle( ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }
        Status = SPI_STM32L496VGT6P_Instance_Cycle( Instance );
    }
    while ( 0 );
    return Status;
}

SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_DeInitialize( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = SPI_STM32L496VGT6P_Instance_DeInitialize( Instance ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }
        Status = SPI_STM32L496VGT6P_Context_DeInitialize( );
    }
    while ( 0 );
    return Status;
}

SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Write( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Status = SPI_STM32L496VGT6P_Instance_Write( Instance, Data, DataLength );
    }
    while ( 0 );
    return Status;
}

SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Read( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * Data, SPI_STM32L496VGT6P_DataLength_t DataLength )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Status = SPI_STM32L496VGT6P_Instance_Read( Instance, Data, DataLength );
    }
    while ( 0 );
    return Status;
}

SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Transaction( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_Data_t * DataTx, SPI_STM32L496VGT6P_DataLength_t DataTxLength, SPI_STM32L496VGT6P_Data_t * DataRx, SPI_STM32L496VGT6P_DataLength_t DataRxLength )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Error;
    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Status = SPI_STM32L496VGT6P_Instance_Transaction( Instance, DataTx, DataTxLength, DataRx, DataRxLength );
    }
    while ( 0 );
    return Status;
}

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#endif /* STM32L496xx */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
