/***************** MPEG-4 Audio Lossless Coding **************************

This software module was originally developed by

Tilman Liebchen (Technical University of Berlin)

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

Copyright (c) 2003.

filename : lpc.h
project  : MPEG-4 Audio Lossless Coding
author   : Tilman Liebchen (Technical University of Berlin)
date     : June 25, 2003
contents : Header file for lpc.cpp

*************************************************************************/

/*************************************************************************
 *
 * Modifications:
 *
 * 03/24/2004, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - added GetResidualRA() and GetSignalRA()
 *
 * 02/06/2005, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - changed some parameters from short to long (e.g. block length N)
 *
 * 12/25/2009, Csaba Kos <csaba.kos@as.ntt-at.co.jp>
 *   - introduced the IntRes parameter to BlockIsConstant()
 *
 *************************************************************************/

void acf(double *x, long N, long k, short norm, double *rxx);
void hamming(int *x, double *xd, long N);
void hanning(int *x, double *xd, long N);
void blackman(int *x, double *xd, long N);
void rect(int *x, double *xd, long N);
short durbin(short ord, double *rxx, double *par);
short par2cof(int *cof, int *par, short ord, short Q);

short GetCof(int *x, long N, short P, short win, double *par);
void GetResidual(int *x, long N, short P, short Q, int *cof, int *d);
void GetSignal(int *x, long N, short P, short Q, int *cof, int *d);
short GetResidualRA(int *x, long N, short P, short Q, int *par, int *cof, int *d);
short GetSignalRA(int *x, long N, short P, short Q, int *par, int *cof, int *d);

short BlockIsZero(int *x, long N);
int BlockIsConstant(int *x, long N, short IntRes);
short ShiftOutEmptyLSBs(int *x, long N);
