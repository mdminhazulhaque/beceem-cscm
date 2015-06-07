/* ---------------------------------------------------- */
/*	Client-Server Connection Manager                    */
/*	Copyright 2010 Beceem Communications, Inc.          */
/*                                                      */
/*	December 2010                                           */
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "BeceemAPI.h"


BOOL BeceemCscmOEM_GetCAPLParams (ST_CAPL_BASED_SEARCH_PARAMS *lpsmParams)
{

/****************************************************************************/
//2 This is Beceem Example code. 
//2 OEM/VENDOR to replace this with their proprietary implementation.
/*****************************************************************************/

//3 ### EXAMPLE CODE START ###
	UINT32 cfList[] = {2520250, 2540250, 2560250, 2580250, 2600250, 
					   2620250, 2640250, 2660250, 2680250, 2340250, 2360250, 3500250, 3580250};
	UINT32 i;

    memset(lpsmParams, 0, sizeof(ST_CAPL_BASED_SEARCH_PARAMS));
	lpsmParams->u32Duration = 0;
	//Keep ReportType as 1, else Response handler function has to be modified.
	lpsmParams->u32ReportType = 1;
	lpsmParams->u32NumNapID = 0;
	lpsmParams->u32Flags = 0;
	lpsmParams->iCinrThreshold = 6;
	lpsmParams->iRssiThreshold = -93;
	lpsmParams->stChannelSpec.u32NumChannels = 9;
	for(i=0;i<lpsmParams->stChannelSpec.u32NumChannels;i++)
	{
		lpsmParams->stChannelSpec.stChannelList[i].au32Preamble[0] = 0xFF;
		lpsmParams->stChannelSpec.stChannelList[i].au32Preamble[1] = 0xFF;
		lpsmParams->stChannelSpec.stChannelList[i].au32Preamble[2] = 0xFF;
		lpsmParams->stChannelSpec.stChannelList[i].au32Preamble[3] = 0xFF;
		lpsmParams->stChannelSpec.stChannelList[i].u32Bandwidth = 10000;
		lpsmParams->stChannelSpec.stChannelList[i].u32CenterFreq = cfList[i];
	}
	return TRUE;
//3 ### EXAMPLE CODE START ###	
}


