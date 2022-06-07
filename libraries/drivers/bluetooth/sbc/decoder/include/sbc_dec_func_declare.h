/*
* Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
*/

#ifndef SBCFUNCDECLARE_H
#define SBCFUNCDECLARE_H

#include "sbc_decoder.h"
void Mult64(SINT32 s32In1, SINT32 s32In2, SINT32 *s32OutLow, SINT32 *s32OutHi);
void sbc_dec_bit_alloc_mono(SBC_DEC_PARAMS *CodecParams);
void sbc_dec_bit_alloc_ste(SBC_DEC_PARAMS *CodecParams);
SINT16 SBC_Decoder(SBC_DEC_PARAMS *strDecParams);
SINT16 SBC_Decoder_Init(SBC_DEC_PARAMS *);
/*void DecQuantizer(SINT16 blk, SBC_DEC_PARAMS *);*/
/*void SbcSynthesisFilter(SINT32 *, SINT32 *, SINT16 *, SINT32 *, SINT16);*/
void SbcSynthesisFilter(SBC_DEC_PARAMS *strDecParams);
SINT16 DecUnPacking(SBC_DEC_PARAMS *strDecParams);
void SbcSynthesisFilter4sb(SBC_DEC_PARAMS *strDecParams);
void SbcSynthesisFilter8sb(SBC_DEC_PARAMS *strDecParams);

#endif
