/*
 * Header file for SHA1
 *
 * Dong Jin PARK (djpax.park@samsung.com)
 * 2009.11.13
 *
 */
 
#ifndef __ACE_FW_SHA1_H__
#define __ACE_FW_SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ace_sfr.h>

	
/*****************************************************************
	Functions
*****************************************************************/
int SHA1_digest (
	unsigned char*	pOut,
	unsigned char*	pBufAddr,
	unsigned int		bufLen
);


#ifdef __cplusplus
}
#endif

#endif 

