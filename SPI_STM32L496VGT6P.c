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

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

/**
 * @brief SPI STM32L496VGT6P Operation
 *
 * @enum SPI_STM32L496VGT6P_OperationType_t
 */
typedef enum SPI_STM32L496VGT6P_OperationType
{
    SPI_STM32L496VGT6P_OperationType_None = 0,    ///< None
    SPI_STM32L496VGT6P_OperationType_Pending,     ///< Pending
    SPI_STM32L496VGT6P_OperationType_Commit,      ///< Commit
    SPI_STM32L496VGT6P_OperationType_Transmit,    ///< Transmit
    SPI_STM32L496VGT6P_OperationType_Receive,     ///< Receive
    SPI_STM32L496VGT6P_OperationType_Transaction, ///< Transaction
} SPI_STM32L496VGT6P_OperationType_t;

/**
 * @brief SPI STM32L496VGT6P Operation Handler
 */
typedef SPI_STM32L496VGT6P_Status_t ( *SPI_STM32L496VGT6P_OperationHandler_t )( SPI_STM32L496VGT6P_Instance_t * Instance );

/**
 * @brief SPI STM32L496VGT6P Operation Context
 *
 * @struct SPI_STM32L496VGT6P_OperationContext_t
 */
typedef struct SPI_STM32L496VGT6P_OperationContext
{
    SPI_STM32L496VGT6P_Data_t * DataTx;
    SPI_STM32L496VGT6P_DataLength_t DataTxLength;

    SPI_STM32L496VGT6P_Data_t * DataRx;
    SPI_STM32L496VGT6P_DataLength_t DataRxLength;
} SPI_STM32L496VGT6P_OperationContext_t;

/**
 * @brief SPI STM32L496VGT6P Operation
 *
 * @struct SPI_STM32L496VGT6P_Operation_t
 */
typedef struct SPI_STM32L496VGT6P_Operation
{
    SPI_STM32L496VGT6P_OperationType_t Type;       ///< Type
    SPI_STM32L496VGT6P_OperationHandler_t Handler; ///< Handler
    SPI_STM32L496VGT6P_Status_t Status;            ///< Status
    TIM_Timestamp_t Timeout;                       ///< Timeout
    SPI_STM32L496VGT6P_OperationContext_t Context; ///< Context
} SPI_STM32L496VGT6P_Operation_t;

/**
 * @brief SPI STM32L496VGT6P Process Type
 *
 * @enum SPI_STM32L496VGT6P_ProcessType_t
 */
typedef enum SPI_STM32L496VGT6P_ProcessType
{
    SPI_STM32L496VGT6P_ProcessType_None = 0,    ///< None
    SPI_STM32L496VGT6P_ProcessType_Initialize,  ///< Initialize
    SPI_STM32L496VGT6P_ProcessType_Transmit,    ///< Transmit
    SPI_STM32L496VGT6P_ProcessType_Receive,     ///< Receive
    SPI_STM32L496VGT6P_ProcessType_Transaction, ///< Transaction
} SPI_STM32L496VGT6P_ProcessType_t;

/**
 * @brief SPI STM32L496VGT6P Process Handler
 */
typedef SPI_STM32L496VGT6P_Status_t ( *SPI_STM32L496VGT6P_ProcessHandler_t )( SPI_STM32L496VGT6P_Instance_t * Instance );

/**
 * @brief SPI STM32L496VGT6P Process Context
 *
 * @struct SPI_STM32L496VGT6P_ProcessContext_t
 */
typedef struct SPI_STM32L496VGT6P_ProcessContext
{
    SPI_STM32L496VGT6P_Operation_t Operation; ///< Operation
} SPI_STM32L496VGT6P_ProcessContext_t;

/**
 * @brief SPI STM32L496VGT6P Process
 *
 * @struct SPI_STM32L496VGT6P_Process_t
 */
typedef struct SPI_STM32L496VGT6P_Process
{
    SPI_STM32L496VGT6P_ProcessType_t Type;       ///< Type
    SPI_STM32L496VGT6P_ProcessHandler_t Handler; ///< Handler
    SPI_STM32L496VGT6P_ProcessContext_t Context; ///< Context
} SPI_STM32L496VGT6P_Process_t;

/**
 * @brief SPI STM32L496VGT6P Event
 *
 * @enum SPI_STM32L496VGT6P_Event_t
 */
typedef enum SPI_STM32L496VGT6P_Event
{
    SPI_STM32L496VGT6P_Event_None = 0,
    SPI_STM32L496VGT6P_Event_Interrupt = UTIL_BIT( 0 ),
    SPI_STM32L496VGT6P_Event_TxComplete = UTIL_BIT( 1 ),
    SPI_STM32L496VGT6P_Event_RxComplete = UTIL_BIT( 2 ),
    SPI_STM32L496VGT6P_Event_Abort = UTIL_BIT( 3 ),
    SPI_STM32L496VGT6P_Event_Error = UTIL_BIT( 4 ),
} SPI_STM32L496VGT6P_Event_t;

typedef struct SPI_STM32L496VGT6P_Instance_Context
{
    SPI_HandleTypeDef SPIx;

    SPI_STM32L496VGT6P_Event_t Event;

    SPI_STM32L496VGT6P_Process_t Process;
} SPI_STM32L496VGT6P_Instance_Context_t;

typedef struct SPI_STM32L496VGT6P_Context
{
    TIM_Timestamp_t Timestamp;
    SPI_STM32L496VGT6P_Instance_Context_t Context[ SPI_STM32L496VGT6P_Count ];
} SPI_STM32L496VGT6P_Context_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

void SPI1_IRQHandler( void );
void SPI2_IRQHandler( void );
void SPI3_IRQHandler( void );

void HAL_SPI_TxCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_RxCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_TxRxCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_TxHalfCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_RxHalfCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_TxRxHalfCpltCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_ErrorCallback( SPI_HandleTypeDef * hspi );
void HAL_SPI_AbortCpltCallback( SPI_HandleTypeDef * hspi );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Initialize( void );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Cycle( void );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_DeInitialize( void );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Initialize( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Cycle( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_DeInitialize( SPI_STM32L496VGT6P_Instance_t * Instance );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_SetProcess( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_ProcessType_t ProcessType );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessInitialize( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessTransmit( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessReceive( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessTransaction( SPI_STM32L496VGT6P_Instance_t * Instance );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationCommitExecute( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationCommitResolve( SPI_STM32L496VGT6P_Instance_t * Instance );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransmitExecute( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransmitResolve( SPI_STM32L496VGT6P_Instance_t * Instance );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationReceiveExecute( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationReceiveResolve( SPI_STM32L496VGT6P_Instance_t * Instance );

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransactionExecute( SPI_STM32L496VGT6P_Instance_t * Instance );
static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransactionResolve( SPI_STM32L496VGT6P_Instance_t * Instance );

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

static SPI_STM32L496VGT6P_Context_t SPI_STM32L496VGT6P_Context;

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

void SPI1_IRQHandler( void )
{
    SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_1 ];

    Context->Event |= SPI_STM32L496VGT6P_Event_Interrupt;

    HAL_SPI_IRQHandler( &Context->SPIx );
}

void SPI2_IRQHandler( void )
{
    SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ];

    Context->Event |= SPI_STM32L496VGT6P_Event_Interrupt;

    HAL_SPI_IRQHandler( &Context->SPIx );
}

void SPI3_IRQHandler( void )
{
    SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ];

    Context->Event |= SPI_STM32L496VGT6P_Event_Interrupt;

    HAL_SPI_IRQHandler( &Context->SPIx );
}

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

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Initialize( void )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

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
        __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_1 ].SPIx, hdmatx, hdma_spi1_tx );

        extern SPI_HandleTypeDef hspi2;
        extern DMA_HandleTypeDef hdma_spi2_tx;
        extern DMA_HandleTypeDef hdma_spi2_rx;
        extern void MX_SPI2_Init( void );
        MX_SPI2_Init( );
        SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ].SPIx = hspi2;
        __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ].SPIx, hdmatx, hdma_spi2_tx );
        __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_2 ].SPIx, hdmarx, hdma_spi2_rx );

        extern SPI_HandleTypeDef hspi3;
        extern DMA_HandleTypeDef hdma_spi3_tx;
        extern DMA_HandleTypeDef hdma_spi3_rx;
        extern void MX_SPI3_Init( void );
        MX_SPI3_Init( );
        SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ].SPIx = hspi3;
        __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ].SPIx, hdmatx, hdma_spi3_tx );
        __HAL_LINKDMA( &SPI_STM32L496VGT6P_Context.Context[ SPI_STM32L496VGT6P_3 ].SPIx, hdmarx, hdma_spi3_rx );
    #endif
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_Cycle( void )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( void )", __FUNCTION__ );

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_GetTimestamp( SPI_TIM, &SPI_STM32L496VGT6P_Context.Timestamp ) ) != TIM_Status_Success )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Context_DeInitialize( void )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( void )", __FUNCTION__ );
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Initialize( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];

        // TODO GPIOs Configurations

        Context->Event = SPI_STM32L496VGT6P_Event_None;

        Instance->Context = Context;

        Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_Initialize );
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_Instance_Cycle( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;
        SPI_STM32L496VGT6P_Event_t Event = Context->Event; // CAUTION: Has to copy events occurred at the early start of the cycle, so as to be cleared at the end of the cycle,
                                                           //          which let events occurs after that for the next cycle call

        if ( Operation->Handler != NULL )
        {
            SPI_STM32L496VGT6P_Status_t STM32L496VGT6P_Status = SPI_STM32L496VGT6P_Status_Error;
            if ( ( STM32L496VGT6P_Status = Operation->Handler( Instance ) ) != SPI_STM32L496VGT6P_Status_Success )
            {
                Status = STM32L496VGT6P_Status;
                // FIXME Operation reported non success status, is there any action ?
            }
        }

        if ( Process->Handler != NULL )
        {
            SPI_STM32L496VGT6P_Status_t STM32L496VGT6P_Status = SPI_STM32L496VGT6P_Status_Error;
            if ( ( STM32L496VGT6P_Status = Process->Handler( Instance ) ) != SPI_STM32L496VGT6P_Status_Success )
            {
                Status = STM32L496VGT6P_Status;
                // FIXME Process reported non success status, is there any action ?
            }
        }

        if ( ( Event & SPI_STM32L496VGT6P_Event_Interrupt ) == SPI_STM32L496VGT6P_Event_Interrupt )
        {
            Context->Event &= ~SPI_STM32L496VGT6P_Event_Interrupt;
            SPI_Trace( "Interrupt: Instance=%p, SPIx=%d", Instance, Instance->SPIx );
            // TODO Invoke Callback
        }

        if ( ( Event & SPI_STM32L496VGT6P_Event_TxComplete ) == SPI_STM32L496VGT6P_Event_TxComplete )
        {
            Context->Event &= ~SPI_STM32L496VGT6P_Event_TxComplete;
            SPI_Debug( "TX Complete: Instance=%p, SPIx=%d", Instance, Instance->SPIx );
            // TODO Invoke Callback
        }

        if ( ( Event & SPI_STM32L496VGT6P_Event_RxComplete ) == SPI_STM32L496VGT6P_Event_RxComplete )
        {
            Context->Event &= ~SPI_STM32L496VGT6P_Event_RxComplete;
            SPI_Debug( "RX Complete: Instance=%p, SPIx=%d", Instance, Instance->SPIx );
            // TODO Invoke Callback
        }

        if ( ( Event & SPI_STM32L496VGT6P_Event_Abort ) == SPI_STM32L496VGT6P_Event_Abort )
        {
            Context->Event &= ~SPI_STM32L496VGT6P_Event_Abort;
            SPI_Debug( "Abort: Instance=%p, SPIx=%d", Instance, Instance->SPIx );
            // TODO Invoke Callback
        }

        if ( ( Event & SPI_STM32L496VGT6P_Event_Error ) == SPI_STM32L496VGT6P_Event_Error )
        {
            Context->Event &= ~SPI_STM32L496VGT6P_Event_Error;
            SPI_Debug( "Error: Instance=%p, SPIx=%d", Instance, Instance->SPIx );
            // TODO Invoke Callback
        }
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

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_SetProcess( SPI_STM32L496VGT6P_Instance_t * Instance, SPI_STM32L496VGT6P_ProcessType_t ProcessType )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p, ProcessType=%d )", __FUNCTION__, Instance, ProcessType );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        switch ( ProcessType )
        {
            case SPI_STM32L496VGT6P_ProcessType_None:
                Process->Handler = NULL;
                break;

            case SPI_STM32L496VGT6P_ProcessType_Initialize:
                Process->Handler = SPI_STM32L496VGT6P_ProcessInitialize;
                break;

            case SPI_STM32L496VGT6P_ProcessType_Transmit:
                Process->Handler = SPI_STM32L496VGT6P_ProcessTransmit;
                break;

            case SPI_STM32L496VGT6P_ProcessType_Receive:
                Process->Handler = SPI_STM32L496VGT6P_ProcessReceive;
                break;

            case SPI_STM32L496VGT6P_ProcessType_Transaction:
                Process->Handler = SPI_STM32L496VGT6P_ProcessTransaction;
                break;

            default:
                SPI_Warning( "%s Not Handled Type %d", __FUNCTION__, ProcessType );
                Status = SPI_STM32L496VGT6P_Status_NotSupported;
                break;
        }
        if ( Status != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        Process->Type = ProcessType;

        Operation->Handler = NULL;
        Operation->Status = SPI_STM32L496VGT6P_Status_Success;
        Operation->Timeout = SPI_STM32L496VGT6P_Context.Timestamp;

        switch ( ProcessType )
        {
            case SPI_STM32L496VGT6P_ProcessType_None:
                Operation->Type = SPI_STM32L496VGT6P_OperationType_None;
                break;

            default:
                Operation->Type = SPI_STM32L496VGT6P_OperationType_Pending;
                break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessInitialize( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != SPI_STM32L496VGT6P_ProcessType_Initialize )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Process->Type, SPI_STM32L496VGT6P_ProcessType_Initialize );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        if ( Operation->Handler != NULL )
        {
            // Operation In-progress
            break;
        }

        if ( Operation->Status != SPI_STM32L496VGT6P_Status_Success )
        {
            Operation->Type = SPI_STM32L496VGT6P_OperationType_None;
        }

        switch ( Operation->Type )
        {
            case SPI_STM32L496VGT6P_OperationType_Pending:
                Operation->Status = SPI_STM32L496VGT6P_OperationCommitExecute( Instance );
                break;

            case SPI_STM32L496VGT6P_OperationType_Commit:
            default:
                if ( Instance->OnComplete != NULL )
                {
                    Instance->OnComplete( Instance, Operation->Status );
                }
                Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_None );
                break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessTransmit( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != SPI_STM32L496VGT6P_ProcessType_Transmit )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Process->Type, SPI_STM32L496VGT6P_ProcessType_Transmit );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        if ( Operation->Handler != NULL )
        {
            // Operation In-progress
            break;
        }

        if ( Operation->Status != SPI_STM32L496VGT6P_Status_Success )
        {
            Operation->Type = SPI_STM32L496VGT6P_OperationType_None;
        }

        switch ( Operation->Type )
        {
            case SPI_STM32L496VGT6P_OperationType_Pending:
                Operation->Status = SPI_STM32L496VGT6P_OperationTransmitExecute( Instance );
                break;

            case SPI_STM32L496VGT6P_OperationType_Transmit:
            default:
                if ( Instance->OnComplete != NULL )
                {
                    Instance->OnComplete( Instance, Operation->Status );
                }
                Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_None );
                break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessReceive( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != SPI_STM32L496VGT6P_ProcessType_Receive )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Process->Type, SPI_STM32L496VGT6P_ProcessType_Receive );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        if ( Operation->Handler != NULL )
        {
            // Operation In-progress
            break;
        }

        if ( Operation->Status != SPI_STM32L496VGT6P_Status_Success )
        {
            Operation->Type = SPI_STM32L496VGT6P_OperationType_None;
        }

        switch ( Operation->Type )
        {
            case SPI_STM32L496VGT6P_OperationType_Pending:
                Operation->Status = SPI_STM32L496VGT6P_OperationReceiveExecute( Instance );
                break;

            case SPI_STM32L496VGT6P_OperationType_Receive:
            default:
                if ( Instance->OnComplete != NULL )
                {
                    Instance->OnComplete( Instance, Operation->Status );
                }
                Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_None );
                break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_ProcessTransaction( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != SPI_STM32L496VGT6P_ProcessType_Transaction )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Process->Type, SPI_STM32L496VGT6P_ProcessType_Transaction );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        if ( Operation->Handler != NULL )
        {
            // Operation In-progress
            break;
        }

        if ( Operation->Status != SPI_STM32L496VGT6P_Status_Success )
        {
            Operation->Type = SPI_STM32L496VGT6P_OperationType_None;
        }

        switch ( Operation->Type )
        {
            case SPI_STM32L496VGT6P_OperationType_Pending:
                Operation->Status = SPI_STM32L496VGT6P_OperationTransactionExecute( Instance );
                break;

            case SPI_STM32L496VGT6P_OperationType_Transaction:
            default:
                if ( Instance->OnComplete != NULL )
                {
                    Instance->OnComplete( Instance, Operation->Status );
                }
                Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_None );
                break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationCommitExecute( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

    // FIXME Keep CubeMX generated configurations as is for now
    #if 0
    HAL_StatusTypeDef HAL_Status = HAL_ERROR;
    if ( ( HAL_Status = HAL_SPI_Init( &Instance->Context->SPIx ) ) != HAL_OK )
    {
      Status = SPI_STM32L496VGT6P_Status_Error;
      break;
    }
    #endif

        Operation->Type = SPI_STM32L496VGT6P_OperationType_Commit;
        Operation->Handler = SPI_STM32L496VGT6P_OperationCommitResolve;
        Operation->Status = SPI_STM32L496VGT6P_Status_Success;
        Operation->Timeout = SPI_STM32L496VGT6P_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 0 ) ) != TIM_Status_Success )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationCommitResolve( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != SPI_STM32L496VGT6P_OperationType_Commit )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, SPI_STM32L496VGT6P_OperationType_Commit );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( SPI_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = SPI_STM32L496VGT6P_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransmitExecute( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        if ( ( HAL_Status = HAL_SPI_Transmit_DMA( &Context->SPIx, Operation->Context.DataTx, Operation->Context.DataTxLength ) ) != HAL_OK )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        Operation->Type = SPI_STM32L496VGT6P_OperationType_Transmit;
        Operation->Handler = SPI_STM32L496VGT6P_OperationTransmitResolve;
        Operation->Status = SPI_STM32L496VGT6P_Status_Success;
        Operation->Timeout = SPI_STM32L496VGT6P_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 1000 ) ) != TIM_Status_Success )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransmitResolve( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != SPI_STM32L496VGT6P_OperationType_Transmit )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, SPI_STM32L496VGT6P_OperationType_Transmit );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( SPI_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = SPI_STM32L496VGT6P_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & SPI_STM32L496VGT6P_Event_TxComplete ) == SPI_STM32L496VGT6P_Event_TxComplete )
        {
            Operation->Status = SPI_STM32L496VGT6P_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationReceiveExecute( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        if ( ( HAL_Status = HAL_SPI_Receive_DMA( &Context->SPIx, Operation->Context.DataRx, Operation->Context.DataRxLength ) ) != HAL_OK )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        Operation->Type = SPI_STM32L496VGT6P_OperationType_Receive;
        Operation->Handler = SPI_STM32L496VGT6P_OperationReceiveResolve;
        Operation->Status = SPI_STM32L496VGT6P_Status_Success;
        Operation->Timeout = SPI_STM32L496VGT6P_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 1000 ) ) != TIM_Status_Success )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationReceiveResolve( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != SPI_STM32L496VGT6P_OperationType_Receive )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, SPI_STM32L496VGT6P_OperationType_Receive );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( SPI_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = SPI_STM32L496VGT6P_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & SPI_STM32L496VGT6P_Event_RxComplete ) != 0 )
        {
            Operation->Status = SPI_STM32L496VGT6P_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransactionExecute( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        HAL_StatusTypeDef HAL_Status = HAL_ERROR;
        if ( ( HAL_Status = HAL_SPI_TransmitReceive_DMA( &Context->SPIx, Operation->Context.DataTx, Operation->Context.DataRx, UTIL_Max( Operation->Context.DataTxLength, Operation->Context.DataRxLength ) ) ) != HAL_OK )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
        Operation->Type = SPI_STM32L496VGT6P_OperationType_Transaction;
        Operation->Handler = SPI_STM32L496VGT6P_OperationTransactionResolve;
        Operation->Status = SPI_STM32L496VGT6P_Status_Success;
        Operation->Timeout = SPI_STM32L496VGT6P_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 1000 ) ) != TIM_Status_Success )
        {
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static SPI_STM32L496VGT6P_Status_t SPI_STM32L496VGT6P_OperationTransactionResolve( SPI_STM32L496VGT6P_Instance_t * Instance )
{
    SPI_STM32L496VGT6P_Status_t Status = SPI_STM32L496VGT6P_Status_Success;

    do
    {
        SPI_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != SPI_STM32L496VGT6P_OperationType_Transaction )
        {
            SPI_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, SPI_STM32L496VGT6P_OperationType_Transaction );
            Status = SPI_STM32L496VGT6P_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( SPI_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = SPI_STM32L496VGT6P_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & ( SPI_STM32L496VGT6P_Event_TxComplete | SPI_STM32L496VGT6P_Event_RxComplete ) ) == ( SPI_STM32L496VGT6P_Event_TxComplete | SPI_STM32L496VGT6P_Event_RxComplete ) )
        {
            Operation->Status = SPI_STM32L496VGT6P_Status_Success;
            Operation->Handler = NULL;
        }
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

        if ( Instance == NULL )
        {
            Status = SPI_STM32L496VGT6P_Status_ArgumentInvalid;
            break;
        }

        if ( ( Status = SPI_STM32L496VGT6P_Context_Initialize( ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        if ( ( Status = SPI_STM32L496VGT6P_Instance_Initialize( Instance ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }
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

        if ( ( Status = SPI_STM32L496VGT6P_Context_Cycle( ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        if ( ( Status = SPI_STM32L496VGT6P_Instance_Cycle( Instance ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }
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

        if ( ( Status = SPI_STM32L496VGT6P_Instance_DeInitialize( Instance ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }

        if ( ( Status = SPI_STM32L496VGT6P_Context_DeInitialize( ) ) != SPI_STM32L496VGT6P_Status_Success )
        {
            break;
        }
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

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != SPI_STM32L496VGT6P_ProcessType_None
             || Operation->Type != SPI_STM32L496VGT6P_OperationType_None )
        {
            Status = SPI_STM32L496VGT6P_Status_Busy;
            break;
        }

        Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_Transmit );

        Operation->Context.DataTx = Data;
        Operation->Context.DataTxLength = DataLength;
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

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != SPI_STM32L496VGT6P_ProcessType_None
             || Operation->Type != SPI_STM32L496VGT6P_OperationType_None )
        {
            Status = SPI_STM32L496VGT6P_Status_Busy;
            break;
        }

        Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_Receive );

        Operation->Context.DataRx = Data;
        Operation->Context.DataRxLength = DataLength;
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

        SPI_STM32L496VGT6P_Instance_Context_t * Context = &SPI_STM32L496VGT6P_Context.Context[ Instance->SPIx ];
        SPI_STM32L496VGT6P_Process_t * Process = &Context->Process;
        SPI_STM32L496VGT6P_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != SPI_STM32L496VGT6P_ProcessType_None
             || Operation->Type != SPI_STM32L496VGT6P_OperationType_None )
        {
            Status = SPI_STM32L496VGT6P_Status_Busy;
            break;
        }

        Status = SPI_STM32L496VGT6P_SetProcess( Instance, SPI_STM32L496VGT6P_ProcessType_Transaction );

        Operation->Context.DataTx = DataTx;
        Operation->Context.DataTxLength = DataTxLength;
        Operation->Context.DataRx = DataRx;
        Operation->Context.DataRxLength = DataRxLength;
    }
    while ( 0 );

    return Status;
}

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

const char SPI_STM32L496VGT6P_VERSION[] = "0.0.0.v20260125-0138";

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

#endif /* STM32L496xx */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
