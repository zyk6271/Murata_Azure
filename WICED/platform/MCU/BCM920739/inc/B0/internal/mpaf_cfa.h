//==================================================================================================
//                        THIS INFORMATION IS PROPRIETARY TO BROADCOM CORP
//--------------------------------------------------------------------------------------------------
//                               Copyright (c) 2011, 2012 Broadcom Corp.
//                                      ALL RIGHTS RESERVED
//==================================================================================================
//! \file
//!
//! Iinterface file for the MPAF cfa layer . This files has the interface for all foundation wrapper functions. This file can be expanded
//! in future to address future foundation support requirements.
//
//==================================================================================================

#ifndef _MPAF_CFA_H_
#define _MPAF_CFA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "brcm_fw_types.h"

//==================================================================================================
// Definitions
//==================================================================================================

#ifndef NVRAM_SUPPORTED
#   define mpaf_cfa_ConfigVSRead                    mpaf_cfa_ConfigRambufRead
#   define mpaf_cfa_ConfigVSWrite                   mpaf_cfa_ConfigRambufWrite
#   define mpaf_cfa_ConfigVSDelete                  mpaf_cfa_ConfigRambufDelete
#   define mpaf_cfa_ConfigVSReadLongItem(a,b,c,d)   mpaf_cfa_ConfigRambufRead(a,c,d)
#   define mpaf_cfa_ConfigVSWriteLongItem(a,b,c,d)  mpaf_cfa_ConfigRambufWrite(a,c,d)
#endif

#define MPAF_CFGID_VS_ID_MAX_READ                  0x7FFE //(ID_ITEM_ID_MAX)

//! values 0x7FFD, 0x7FFE reserved for the vendor specific production test flag command
#define MPAF_CFGID_VS_ID_MAX_WRITE                 0x7FFC //(ID_ITEM_ID_MAX - 2)

//==================================================================================================
// Types and constants
//==================================================================================================
#ifdef UHE_PPK

typedef struct
{
    UINT8* Pk;
    UINT8* RNH;
    UINT8* Bd_addr;
}PPK_PIN_REQ_t;

typedef BOOL32 (*MPAF_CFA_PPK_PUBLIC_REQ_CB)(void);
typedef BOOL32 (*MPAF_CFA_PPK_PIN_REQ_CB)(void);

#endif

//! Function implemented by application.
//! This will be called by core during boot up sequence
void mpaf_cfa_init( void );

//--------------------------------------------------------------------------------------------------
// Memory Manager (MM)
//--------------------------------------------------------------------------------------------------

//! API to get the buffer from generic pool. This buffer can be used by application for its internal purpose.
void* mpaf_cfa_memAlloc( UINT32 size );

//! API to free the buffer allocated from generic buff pool
void mpaf_cfa_memFree( void *memoryBlock );

//! API to allocate memory for permanent usage.
//! The memory allocated using this method cannot be freed. If the allocation would result in
//! exceeding the memory configured for MPAF, then the system will spin indefinitely.
//! NOTE: This function will never return if the requested number of bytes cannot be allocated.
//! To avoid getting into this trap, either the memory usage has to be optimized or MPAF needs to be
//! configured to use more memory.
void *mpaf_cfa_permanentAlloc( UINT32 size );

//! Returns the number of free bytes of RAM left.
//UINT32 mpaf_cfa_freePremanentBuff(void);

//! API to returns the number of bytes free RAM.
UINT32 mpaf_cfa_mm_MemFreeBytes( void );

//--------------------------------------------------------------------------------------------------
// HCI Command, Event, and ACL Data Generation
//--------------------------------------------------------------------------------------------------

//! En-queues an HCI command for processing by the LM thread. If the command buffer passed to this function
//! was dynamically allocated, the caller is responsible for releasing it, and is free to do so after this function
//! returns.
void mpaf_cfa_executeHciCmd( const void* command );

//! En-queues an HCI command for processing by the LM thread. If the command buffer passed to this function
//! was dynamically allocated, the caller is responsible for releasing it, and is free to do so after this function
//! returns. The only difference between the other similarly named function, "mpaf_pfSapExecuteHciCmd" is
//! that this function rely on the caller to allocate memory.
void mpaf_cfa_executeHCICmd_No_Alloc( const void* command );

//! En-queues an HCI event to be sent to the host over the transport.  Although the event is sent to the host by the
//! same mechanism which would handle sending events generated by the core firmware, an installed HCI event filter
//! will not be invoked for msg en-queued by the MPAF. If the event buffer passed to this function was dynamically
//! allocated, the caller is responsible for releasing it, and is free to do so after this function returns.
void mpaf_cfa_sendHciEvent( const void* event );

//! API to get the buffer from acl down pool. This buffer should be used by application to send data to lm threads(ACL packet).
void * mpaf_cfa_allocACLDown( UINT32 payloadSize );

//! API to free the buffer allocated from acl pool
void mpaf_cfa_freeACLBuffer( void* mem_ptr );

//! En-queues an ACL data packet to be sent over the air. The ACL data block must have been allocated
//! using mpaf_pfSapAllocACLDown with the for Tx parameter having been TRUE.  Once this function has been called,
//! the caller no longer owns the acl data buffer block and may not use or release it.
void mpaf_cfa_sendACLTx( void* acl );

//--------------------------------------------------------------------------------------------------
// Thread dispatch functions
//--------------------------------------------------------------------------------------------------
//! API to send msg to active transport thread.
void mpaf_cfa_dispatchMsgToTransport( void* msg );

//! API to send msg to active lm thread.
//! Example - If you want to send a HCI cmd to Lm thread use code as MSG_BT_HCI_DEFERRED_COMMAND
void mpaf_cfa_dispatchMsgToLm( void* msg );

//--------------------------------------------------------------------------------------------------
// Transport
//--------------------------------------------------------------------------------------------------

//! API to get the active transport type
//! for return type refer bttansport.h
UINT8 mpaf_cfa_GetActiveTransportType( void );

//--------------------------------------------------------------------------------------------------
// BCSULP notification functions
//--------------------------------------------------------------------------------------------------
//! API to register for the ADV transmission notifications
/// Allows the application to register a callback that will be invoked just before an ADV is packet is about
/// to be sent out and again, immediately after.
/// \param clientCallback Pointer to a function that will be invoked in MPAF
///        thread context with BCSULP_ADV_NOTIFICATION_READY for before ADV and
///        BCSULP_ADV_NOTIFICATION_DONE after ADV packet is complete.
/// \param advanceNoticeInMicroSeconds Number of microseconds before the ADV the notification is
///        to be sent. Will be rounded down to the nearest 1.25mS. Has to be an even multiple of 625uS.
BOOL32 mpaf_cfa_notifyAdvPacketTransmissions( void (*clientCallback)( UINT8 ), UINT32 advanceNoticeInMicroSeconds );

//--------------------------------------------------------------------------------------------------
// Debug UART
//--------------------------------------------------------------------------------------------------

#if DEBUG_UART
//! initialize the debug uart (115200, no h/w flow control).\
//! if baud is zero then it takes the default value from config
void mpaf_cfa_debug_InitUART( UINT32 baud );

//! It works similar to normal printf operation.
//! Dont use \r along with \n. This appending of  \r is taken care by driver
//! To print param use %d, %x,%c inside string for respective type.
//! To print only sting pass param as Zero
void mpaf_cfa_debug_Printf( IN char* string, UINT32 param );

// To interface debuguart_Write()
BOOL32 mpaf_debuguart_Write( UINT8 dbyte );

// To interface dbguart_read()
extern UINT32 dbguart_read( BYTE* c );
#define mpaf_dbguart_read(c) dbguart_read(c)

#endif

//--------------------------------------------------------------------------------------------------
// TO DO - Currently not suported by new foundation. Will be implemented in need basis
//--------------------------------------------------------------------------------------------------

//! It works similar to normal scanf operation.
//void mpaf_cfa_debug_Scanf( IN char* format, ... );

//void mpaf_cfa_kickWatchdog(void);
void mpaf_config_rambuf_AllocateRamBuffer( UINT32 alloc_LengthInBytes );
UINT8* mpaf_cfa_ConfigGetRamBufferInfo( UINT16* allocated_len, UINT16* total_len );
UINT8 mpaf_cfa_ConfigRambufRead( UINT16 vsID, UINT8 itemLength, UINT8* payload );
UINT8 mpaf_cfa_ConfigRambufWrite( UINT16 vsID, UINT8 itemLength, UINT8 *payload );
BOOL32 mpaf_cfa_ConfigRambufDelete( UINT16 vsID );

//! Function to enable the BT sleep/BCS sleep and to stop the transport detection machanism.
//! This could be used to enable for A4WP in SA mode.
//! sleep_enable         : Enables the sleep if TRUE, else disables
//! trans_detect_disable : TRUE will disable the transport detection, FALSE will enable the
//!                        transport detection when sleep is disabled.
void mpaf_cfa_bt_sleep_enable( BOOL32 sleep_enable, BOOL32 trans_detect_disable );

#ifdef MPAF_CFA
void updateTotalSleepTime( UINT64 sleepTime );
#else
#define updateTotalSleepTime(sleepTime)
#endif
#define updateBtClockSleepTime(sleepTime)
#define updateBtShortSleepTime(sleepTime)
extern void (*mpaf_updateTotalSleepTime)( UINT64 sleepTime );

#ifdef NVRAM_SUPPORTED
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// 10.2     Configuration Data VS
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//------------------------------------------------------------------------------
// 10.2.1       Types
//------------------------------------------------------------------------------
typedef BOOL32 (*MPAF_CFA_VS_READ_CB)( UINT16 vsID, UINT8 itemLength, UINT8* payload );
// The MPAF_CFA_VS_READ_CB type is a function pointer for a callback from the
// mpaf_cfa_ConfigVSReadCB function.

//------------------------------------------------------------------------------
// 10.2.2       Configuration Data VS Access Functions
//------------------------------------------------------------------------------
UINT8 mpaf_cfa_ConfigVSRead( UINT16 vsID, UINT8 itemLength, UINT8 *payload );
// Reads an item from the configuration data volatile section.  Returns the
// number of bytes actually read, or 0 if the item was not present.  The maximum
// valid vsID is 0x7D, with 0x7E and 0x7F being reserved.

//------------------------------------------------------------------------------
UINT8 mpaf_cfa_ConfigVSReadCB( MPAF_CFA_VS_READ_CB callback, UINT8 maxItemLength, BYTE* payloadBuffer );
// Reads all valid items with a length less than or equal to maxItemLength from
// the configuration data volatile section.  For each valid item, the callback
// function is invoked indicting the item's ID tag, valid payload length, and
// payload.  For any given invocation of the callback, if the callback function
// returns TRUE, reading the VS is terminated and the item length is returned
// from cfa_ConfigVSReadCB, with the payloadBuffer still containing the item's
// payload.  If the callback function returned FALSE to every item,
// mpaf_cfa_ConfigVSReadCB returns 0.

//------------------------------------------------------------------------------
UINT8 mpaf_cfa_ConfigVSWrite( UINT16 vsID, UINT8 itemLength, UINT8 *payload );
// Writes an item to the configuration data volatile section.  This function may
// not be called prior to when the post-VS init function is called, or would
// have been called if that init function is not installed (see Table 2).
// Returns the number of bytes actually written.  If the number of bytes written
// is less than what was requested, the volatile section is full and needs to be
// compacted.  Compacting the volatile section is performed in the idle thread,
// once all connections have exited sniff.  In the event that the volatile
// section needs to be compacted, the CSA should attempt to write the VS item
// again after a timeout period of a few seconds.  If memory is available,
// mpaf_cfa_ConfigVSWrite can buffer up to four writes in RAM in the event that
// compaction is necessary.  In that case, cfa_ConfigVSWrite will return an
// indication that the number of bytes written was equal to itemLength.  The
// maximum valid vsID is 0x7B, with 0x7C, 0x7D, 0x7E and 0x7F being reserved.

//------------------------------------------------------------------------------
BOOL32 mpaf_cfa_ConfigVSDelete( UINT16 vsID );
// Deletes an item in the configuration data volatile section.  Returns TRUE on
// success, FALSE on failure.  A FALSE indication generally indicates that the
// item was not present.  Typically, an item will be overwritten using
// mpaf_cfa_ConfigVSWrite rather than deleted, and deleting an item is not necessary
// prior to overwriting it: the old item is implicitly deleted.

//------------------------------------------------------------------------------
UINT16 mpaf_cfa_ConfigVSReadLongItem( UINT16 controlID, UINT16 numReservedIDs, UINT16 itemLength, UINT8* payload );
// Reads a long item from the configuration data volatile section.  The item
// is actually stored in multiple vsIDs immediately after controlID, which must
// be reserved for the purpose.  The maximum length is (numReservedIDs-1)*255.
// For example if controlID is 5 and numReservedIDs is 3, vsIDs 5, 6 and 7 must
// be reserved for this long item and the maximum itemLength is 510.  In the
// event of success, returns the number of bytes read, or 0 in the event of
// failure or if the controlID or a required fragment is absent.

//------------------------------------------------------------------------------
UINT16 mpaf_cfa_ConfigVSWriteLongItem( UINT16 controlID, UINT16 numReservedIDs, UINT16 itemLength, UINT8* payload );
// Writes a long item to the configuration data volatile section.  The item
// is actually stored in multiple vsIDs immediately after controlID, which must
// be reserved for the purpose.  The maximum length is (numReservedIDs-1)*255.
// For example if controlID is 5 and numReservedIDs is 3, vsIDs 5, 6 and 7 must
// be reserved for this long item and the maximum itemLength is 510.  In the
// event of success, returns the number of bytes written, or 0 in the event of
// failure.
#endif

#ifdef UHE_ENABLE
void mpaf_cfa_TurnRadio(BOOL32);
#endif //UHE_ENABLE

#ifdef IT_MANAGER
void mpaf_cfa_app_stack_shutdown_complete(void);
#endif

#ifdef TBFC
BOOL32 mpaf_cfa_tbfc_startTbfcScan( BOOL32 onForEver );
BOOL32 mpaf_cfa_tbfc_stopTbfcScan( void );
#endif

#ifdef UHE_PPK

void mpaf_cfa_ppk_SetPublicCb(MPAF_CFA_PPK_PUBLIC_REQ_CB callback);
//To register callback function for lm
// Input/Output:
//   callback - The callback function to call
//
// Return: None

void mpaf_cfa_ppk_PublicRequest(void);
// To request Public key and random number generation
// Input/Output: None
// Return: None

UINT8* mpaf_cfa_ppk_PublicGet(void);
//To retrieve Pk and RNT from lm
// Input/Output: None
//
// Return:
//   Pk - pointer of Public Key

UINT8* mpaf_cfa_ppk_RNTGet(void);
//To retrieve Pk and RNT from lm
// Input/Output: None
//
// Return:
//   RNT - pointer of Random number
//
// RNT is deleted after calling mpaf_cfa_ppk_PinRequest()

void mpaf_cfa_ppk_SetPinCb(MPAF_CFA_PPK_PIN_REQ_CB callback);
//To register callback function for lm
// Input/Output:
//   callback - The callback function to call
//
// Return: None

void mpaf_cfa_ppk_PinRequest(UINT8 *Pk, UINT8* RNH, UINT8* Bd_addr);
// To request PIN Code generation
// Input/Output:
//   Pk - Pointer of the pulic key received from hid
//   RNH - Pointer of the random number received from hid
//   Bd_addr - Pointer of reversed form of Bd Addr
//
// Return: None
// Pk, RNH, Bd_addr are passed as pointer,
// these pointer indicated location should be protected until callback function is called.

UINT8* mpaf_cfa_ppk_PinGet(void);
//To retrieve Pin from lm
// Input/Output: None
//
// Return:
//   Pin - pointer of Pin

#endif
#ifdef UHE_ENABLE
/*******************************************************************************
 * Function: mpaf_cfa_bdaddr_from_conhandle
 *
 * Abstract: To retireve bd_addr from connection handle
 *
 * Input/Output:
 *   conhandle - Connection Handle
 *
 * Return:
 *   bd_addr - pointer of UINT8 bd_addr[6];
 *******************************************************************************/
UINT8* mpaf_cfa_bdaddr_from_conhandle(UINT32 conhandle);
#endif

#ifdef __cplusplus
}
#endif

#endif// -MPAF_CFA

