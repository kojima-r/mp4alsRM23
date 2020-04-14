/***************** MPEG-4 Audio Lossless Coding **************************

This software module was originally developed by

Choo Wee Boon (Institute for InfoComm Research)
Huang HaiBin
Yu Rong Shan

in the course of development of the MPEG-4 Audio standard ISO/IEC 14496-3
and associated amendments. This software module is an implementation of
a part of one or more MPEG-4 Audio lossless coding tools as specified
by the MPEG-4 Audio standard. ISO/IEC gives users of the MPEG-4 Audio
standards free license to this software module or modifications
thereof for use in hardware or software products claiming conformance
to the MPEG-4 Audio standards. Those intending to use this software
module in hardware or software products are advised that this use may
infringe existing patents. The original developer of this software
module, the subsequent editors and their companies, and ISO/IEC have
no liability for use of this software module or modifications thereof
in an implementation. Copyright is not released for non MPEG-4 Audio
conforming products. The original developer retains full right to use
the code for the developer's own purpose, assign or donate the code to
a third party and to inhibit third party from using the code for non
MPEG-4 Audio conforming products. This copyright notice must be included
in all copies or derivative works.

Copyright (c) 2004.

filename : lms.h
project  : MPEG-4 Audio Lossless Coding
author   : Choo Wee Boon (Institute for InfoComm Research)
date     : Dec 20, 2004
contents : Header file for lms.h

*************************************************************************/

/*************************************************************************
 *
 * Modifications:
 *
 * Dec 25, 2009 - Do not use the const block coding tool when the constant
 *                value cannot be represented on IntRes bits
 *
 * Jan 25, 2011 - Fix integer overflows for frame lengths >= 32768
 *
 *************************************************************************/
#include "mcc.h"

#ifndef LMS_DEFINE
#define LMS_DEFINE
#if defined(WIN32) || defined(WIN64)
	typedef __int64 INT64;
	typedef unsigned __int64 UINT64;
	#define JS_INIT_P 115292150460684 
#else
	#include <stdint.h>
	typedef int64_t INT64;
	#define _I64_MAX 9223372036854775807LL
	#define _I64_MIN (-9223372036854775807LL - 1LL)
	#define JS_INIT_P 115292150460684LL 
#endif

#define ROUND1(x)  ((int)(x+8)>>4) 
#define ROUND2(x)  ((INT64) ( (INT64) x + (INT64) 1 )>>1)
#define ENCODE 0
#define DECODE 1
#define MAX_MODE 5
#define MAX_STAGES 10
#define JS_LEN1 30
#define JS_LEN2	30
#define JS_LEN (JS_LEN1+JS_LEN2)
#define JS_LAMBDA 999         // 0.999


#define LMS_START 2

#define LMS_LEN 1024*8   
#define TOTAL_LMS_LEN LMS_LEN // depend on how many predictor stage
#define LMS_MU_INT  16777 // 7.24 format for 0.001

#define FRACTION (1L <<24)

#define RA_TRANS N/32
typedef int BUF_TYPE;
typedef int W_TYPE;
typedef INT64 P_TYPE;
struct mtable{
			short nstage;
			short filter_len[MAX_STAGES];
			short opt_mu[MAX_STAGES];
			short lambda[2];
			int step_size;
};

struct rlslms_buf_ptr {
	BUF_TYPE **pbuf;
	W_TYPE **weight;
	P_TYPE **Pmatrix;
    short channel; // which channel is currently processing		
};

void analyze(int *x, long N,  rlslms_buf_ptr *rlslms_ptr, short RA, short IntRes, MCC_ENC_BUFFER *mccbuf);
void synthesize(int *x, long N,  rlslms_buf_ptr *rlslms_ptr, short RA, MCC_DEC_BUFFER *mccbuf);
void analyze_joint(int *x0, int *x1, long N,  rlslms_buf_ptr *rlslms_ptr, short RA, short IntRes, short *mono, MCC_ENC_BUFFER *mccbuf);
void synthesize_joint(int *x0, int *x1, long N, rlslms_buf_ptr *rlslms_ptr, short RA, short mono, MCC_DEC_BUFFER *mccbuf);
void predict_init(rlslms_buf_ptr *ptr);
void initCoefTable(short mode, unsigned char CoefTable);


extern short BlockIsZero(int *x, long N);
extern int BlockIsConstant(int *x, long N, short IntRes);
extern short ShiftOutEmptyLSBs(int *x, long N);
extern short lookup_mu(short mu);
extern short lookup_table(short *,short mu);


extern mtable c_mode_table;
extern mtable safe_mode_table;
extern mtable *table_assigned[3];
extern char mu_table[32];
extern short lms_order_table[32]; 
#endif
