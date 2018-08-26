/*****************************************************************************
 *
 *  XVID MPEG-4 VIDEO CODEC
 *  - Global Motion Estimation -
 *
 *  Copyright(C) 2003 Christoph Lampert <gruel@web.de>
 *
 *  This program is free software ; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation ; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY ; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program ; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: estimation_gmc.c,v 1.5 2004/12/05 04:53:01 syskin Exp $
 *
 ****************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../encoder.h"
#include "../prediction/mbprediction.h"
#include "estimation.h"
#include "motion.h"
#include "sad.h"
#include "gmc.h"
#include "../utils/emms.h"
#include "motion_inlines.h"

static void
CheckCandidate16I(const int x, const int y, SearchData * const data, const unsigned int Direction)
{
	int sad;
	const uint8_t * Reference;

	if ( (x > data->max_dx) || ( x < data->min_dx)
		|| (y > data->max_dy) || (y < data->min_dy) ) return;

	Reference = GetReference(x, y, data);

	sad = sad16(data->Cur, Reference, data->iEdgedWidth, 256*4096);

	if (sad < data->iMinSAD[0]) {
		data->iMinSAD[0] = sad;
		data->currentMV[0].x = x; data->currentMV[0].y = y;
		data->dir = Direction;
	}
}

static __inline void
GMEanalyzeMB (	const uint8_t * const pCur,
				const uint8_t * const pRef,
				const uint8_t * const pRefH,
				const uint8_t * const pRefV,
				const uint8_t * const pRefHV,
				const int x,
				const int y,
				const MBParam * const pParam,
				MACROBLOCK * const pMBs,
				SearchData * const Data)
{

	MACROBLOCK * const pMB = &pMBs[x + y * pParam->mb_width];

	Data->iMinSAD[0] = MV_MAX_ERROR;

	Data->predMV = get_pmv2(pMBs, pParam->mb_width, 0, x, y, 0);

	get_range(&Data->min_dx, &Data->max_dx, &Data->min_dy, &Data->max_dy, x, y, 4,
				pParam->width, pParam->height, 16, 1);

	Data->Cur = pCur + 16*(x + y * pParam->edged_width);
	Data->RefP[0] = pRef + 16*(x + y * pParam->edged_width);
	Data->RefP[1] = pRefV + 16*(x + y * pParam->edged_width);
	Data->RefP[2] = pRefH + 16*(x + y * pParam->edged_width);
	Data->RefP[3] = pRefHV + 16*(x + y * pParam->edged_width);

	Data->currentMV[0].x = Data->currentMV[0].y = 0;
	CheckCandidate16I(0, 0, Data, 255);

	if ( (Data->predMV.x !=0) || (Data->predMV.y != 0) )
		CheckCandidate16I(Data->predMV.x, Data->predMV.y, Data, 255);

	xvid_me_DiamondSearch(Data->currentMV[0].x, Data->currentMV[0].y, Data, 255, CheckCandidate16I);

	xvid_me_SubpelRefine(Data->currentMV[0], Data, CheckCandidate16I, 0);


	/* for QPel halfpel positions are worse than in halfpel mode :( */
/*	if (Data->qpel) {
		Data->currentQMV->x = 2*Data->currentMV->x;
		Data->currentQMV->y = 2*Data->currentMV->y;
		Data->qpel_precision = 1;
		get_range(&Data->min_dx, &Data->max_dx, &Data->min_dy, &Data->max_dy, x, y, 4,
					pParam->width, pParam->height, iFcode, 2, 0);
		SubpelRefine(Data);
	}
*/

	pMB->mvs[0] = pMB->mvs[1] = pMB->mvs[2] = pMB->mvs[3] = Data->currentMV[0];
	pMB->sad16 = Data->iMinSAD[0];
	pMB->mode = MODE_INTER;
	pMB->sad16 += 10*d_mv_bits(pMB->mvs[0].x, pMB->mvs[0].y, Data->predMV, Data->iFcode, 0);
	return;
}


void
GMEanalysis(const MBParam * const pParam,
			const FRAMEINFO * const current,
			const FRAMEINFO * const reference,
			const IMAGE * const pRefH,
			const IMAGE * const pRefV,
			const IMAGE * const pRefHV)
{
	uint32_t x, y;
	MACROBLOCK * const pMBs = current->mbs;
	const IMAGE * const pCurrent = &current->image;
	const IMAGE * const pReference = &reference->image;

	SearchData Data;
	memset(&Data, 0, sizeof(SearchData));

	Data.iEdgedWidth = pParam->edged_width;
	Data.rounding = pParam->m_rounding_type;

	Data.iFcode = current->fcode;

	if (sadInit) (*sadInit) ();

	for (y = 0; y < pParam->mb_height; y ++) {
		for (x = 0; x < pParam->mb_width; x ++) {
			GMEanalyzeMB(pCurrent->y, pReference->y, pRefH->y, pRefV->y, pRefHV->y, x, y, pParam, pMBs, &Data);
		}
	}
	return;
}



int
GlobalMotionEstRefine(
				WARPPOINTS *const startwp,
				MACROBLOCK * const pMBs,
				const MBParam * const pParam,
				const FRAMEINFO * const current,
				const FRAMEINFO * const reference,
				const IMAGE * const pCurr,
				const IMAGE * const pRef,
				const IMAGE * const pRefH,
				const IMAGE * const pRefV,
				const IMAGE * const pRefHV)
{
	uint8_t* GMCblock = (uint8_t*)malloc(16*pParam->edged_width);
	WARPPOINTS bestwp=*startwp;
	WARPPOINTS centerwp,currwp;
	int gmcminSAD=0;
	int gmcSAD=0;
	int direction;
#if 0
	int mx,my;
#endif

#if 0
	/* use many blocks... */
	for (my = 0; my < (uint32_t)pParam->mb_height; my++) {
		for (mx = 0; mx < (uint32_t)pParam->mb_width; mx++) {
			const int mbnum = mx + my * pParam->mb_width;
			pMBs[mbnum].mcsel=1;
		}
	}
#endif

#if 0
	/* or rather don't use too many blocks... */
	for (my = 1; my < (uint32_t)MBh-1; my++) {
		for (mx = 1; mx < (uint32_t)MBw-1; mx++) {
			const int mbnum = mx + my * MBw;
			if (MBmask[mbnum-1])
				MBmask[mbnum-1]=0;
			else
				if (MBmask[mbnum-MBw])
					MBmask[mbnum-1]=0;

		}
	}
#endif

	gmcminSAD = globalSAD(&bestwp, pParam, pMBs, current, pRef, pCurr, GMCblock);

	if ( (reference->coding_type == S_VOP)
		 && ( (reference->warp.duv[1].x != bestwp.duv[1].x)
			  || (reference->warp.duv[1].y != bestwp.duv[1].y)
			  || (reference->warp.duv[0].x != bestwp.duv[0].x)
			  || (reference->warp.duv[0].y != bestwp.duv[0].y)
			  || (reference->warp.duv[2].x != bestwp.duv[2].x)
			  || (reference->warp.duv[2].y != bestwp.duv[2].y) ) )
	{
		gmcSAD = globalSAD(&reference->warp, pParam, pMBs,
						   current, pRef, pCurr, GMCblock);

			if (gmcSAD < gmcminSAD)
			{	bestwp = reference->warp;
				gmcminSAD = gmcSAD;
			}
	}

	do {
		direction = 0;
		centerwp = bestwp;

		currwp = centerwp;

		currwp.duv[0].x--;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 1;
		}
		else
		{
		currwp = centerwp; currwp.duv[0].x++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 2;
		}
		}
		if (direction) continue;

		currwp = centerwp; currwp.duv[0].y--;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 4;
		}
		else
		{
		currwp = centerwp; currwp.duv[0].y++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 8;
		}
		}
		if (direction) continue;

		currwp = centerwp; currwp.duv[1].x++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 32;
		}
		currwp.duv[2].y++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 1024;
		}

		currwp = centerwp; currwp.duv[1].x--;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 16;
		}
		else
		{
		currwp = centerwp; currwp.duv[1].x++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 32;
		}
		}
		if (direction) continue;


		currwp = centerwp; currwp.duv[1].y--;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 64;
		}
		else
		{
		currwp = centerwp; currwp.duv[1].y++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 128;
		}
		}
		if (direction) continue;

		currwp = centerwp; currwp.duv[2].x--;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 256;
		}
		else
		{
		currwp = centerwp; currwp.duv[2].x++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 512;
		}
		}
		if (direction) continue;

		currwp = centerwp; currwp.duv[2].y--;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 1024;
		}
		else
		{
		currwp = centerwp; currwp.duv[2].y++;
		gmcSAD = globalSAD(&currwp, pParam, pMBs, current, pRef, pCurr, GMCblock);
		if (gmcSAD < gmcminSAD)
		{	bestwp = currwp;
			gmcminSAD = gmcSAD;
			direction = 2048;
		}
		}
	} while (direction);
	free(GMCblock);

	*startwp = bestwp;

	return gmcminSAD;
}

int
globalSAD(const WARPPOINTS *const wp,
		  const MBParam * const pParam,
		  const MACROBLOCK * const pMBs,
		  const FRAMEINFO * const current,
		  const IMAGE * const pRef,
		  const IMAGE * const pCurr,
		  uint8_t *const GMCblock)
{
	NEW_GMC_DATA gmc_data;
	int iSAD, gmcSAD=0;
	int num=0;
	unsigned int mx, my;

	generate_GMCparameters(	3, 3, wp, pParam->width, pParam->height, &gmc_data);

	for (my = 0; my < (uint32_t)pParam->mb_height; my++)
		for (mx = 0; mx < (uint32_t)pParam->mb_width; mx++) {

		const int mbnum = mx + my * pParam->mb_width;
		const int iEdgedWidth = pParam->edged_width;

		if (!pMBs[mbnum].mcsel)
			continue;

		gmc_data.predict_16x16(&gmc_data, GMCblock,
						pRef->y,
						iEdgedWidth,
						iEdgedWidth,
						mx, my,
						pParam->m_rounding_type);

		iSAD = sad16 ( pCurr->y + 16*(my*iEdgedWidth + mx),
						GMCblock , iEdgedWidth, 65536);
		iSAD -= pMBs[mbnum].sad16;

		if (iSAD<0)
			gmcSAD += iSAD;
		num++;
	}
	return gmcSAD;
}
