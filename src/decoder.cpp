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

filename : decoder.cpp
project  : MPEG-4 Audio Lossless Coding
author   : Tilman Liebchen (Technical University of Berlin)
date     : June 23, 2003
contents : Implementation of the CLpacDecoder class

*************************************************************************/

/*************************************************************************
 * Modifications:
 *
 * 8/31/2003, Yuriy A. Reznik <yreznik@real.com>
 *  made set of changes to support BGMC coding mode:
 *      - added CLpacDecoder::GetBGMC() function;
 *      - added logic for using 0x10 bit (BGMC) in the "parameters" field
 *        and 0x40 bit in the version number in CLpacDecoder::DecodeHeader()
 *      - added calls to BGMC decoding routines in CLpacDecoder::DecodeBlock()
 *
 * 11/26/2003, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   added support for multichannel, 32-bit, and new file format:
 *     - substituted DecodeHeader() by AnalyseInputFile() and WriteHeader()
 *     - substituted DecodeRemains() by WriteTrailer()
 *     - substituted DecodeFile() by DecodeAll()
 *     - added decoding of new file format header in AnalyseInputFile()
 *     - added decoding of multichannel and 32-bit in DecodeFrame() and
 *       DecodeBlock()
 *
 * 12/16/2003, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - new size of tmpbuf[] array
 *
 * 03/22/2004, Koichi Sugiura <ksugiura@mitaka.ntt-at.co.jp>
 *   - supported floating point PCM.
 *
 * 03/24/2004, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - progressive prediction for random access frames
 *   - removed support for file format version < 8
 *
 * 5/17/2004, Yuriy Reznik <yreznik@real.com>
 *   changes to support improved coding of LPC coefficients:
 *   - modified CLpacDecoder::DecodeBlock(() to use new scheme
 *   - incremented version # and removed support for older file formats
 *
 * 07/29/2004, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - added support for higher orders (up to 255)
 *   - further improved entropy coding of coefficients (3 tables)
 *   - streamlined block header syntax
 *   - removed support for file format version < 11
 *
 * 10/26/2004, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - added use of bgmc_decode_blocks()
 *   - added support for up to 8 subblocks
 *   - added support for improved coding of code parameters
 *   - incremented version # and removed support for older file formats
 *   - removed block switching flag where not necessary
 *
 * 11/05/2004, Yutaka Kamamoto <kamamoto@hil.t.u-tokyo.ac.jp>
 *   - added multi-channel correlation method
 *   - divided DecodeBlock() into DecodeBlockParameter()
 *                            and DecodeBlockReconstruct()
 *
 * 11/17/2004, Koichi Sugiura <koichi.sugiura@ntt-at.co.jp>
 *   - modified floating point operations using CFloat class.
 *
 * 11/22/2004, Koichi Sugiura <koichi.sugiura@ntt-at.co.jp>
 *   - supported channel sort feature for floating point data.
 *
 * 11/25/2004, Koichi Sugiura <koichi.sugiura@ntt-at.co.jp>
 *   - added RandomAccess parameter to DecodeDiff().
 *
 * 02/06/2005, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - added support for extended block switching and higher orders
 *
 * 02/11/2005, Takehiro Moriya (NTT) <t.moriya.ieee.org>
 *   - add LTP modules for CE11
 *
 * 03/08/2005, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - integration of CE10 and CE11
 *
 * 04/27/2005, Yutaka Kamamoto <kamamoto@theory.brl.ntt.co.jp>
 *   - added MCCex(multi-tap MCC) modules for CE14
 *   - changed ALS header information of #Channel 1byte(256) to 2 bytes(65536)
 *
 * 04/27/2005, Takehiro Moriya (NTT) <t.moriya@ieee.org>
 *   - added Joint Stereo and MCC switching in EncodeFrame()
 *
 * 05/16/2005, Choo Wee Boon (I2R) <wbchoo@a-star.i2r.edu.sg>
 *	 - added RLSLMS mode 
 *
 * 06/08/2005, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - syntax changes according to N7134
 *
 * 07/07/2005, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - some bugfixes (RM15v2)
 *
 * 07/15/2005, Yutaka Kamamoto <kamamoto@theory.brl.ntt.co.jp>
 * 07/21/2005, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - some more bugfixes (RM15v3)
 *
 * 07/22/2005, Tilman Liebchen <liebchen@nue.tu-berlin.de>
 *   - revised format of ChanSort/ChanPos
 *   - support for auxiliary data
 *
 * 10/17/2005, Noboru Harada <n-harada@theory.brl.ntt.co.jp>
 *   - Merged bug fixed codes for RLS-LMS prepared from I2R.
 *
 * 05/23/2007, Koichi Sugiura <koichi.sugiura@ntt-at.co.jp>
 *   - supported 64-bit data size.
 *   - replaced FILE* with HALSSTREAM.
 *
 * 10/20/2008, Koichi Sugiura <koichi.sugiura@ntt-at.co.jp>
 *   - debugged the limitation of 2nd and 3rd rice parameters to 15 
 *     in case of Res<=16.
 *   - debugged DecodeBlockParameter() to handle IntRes=20 properly.
 *   - debugged maximum prediction order calculation.
 *
 * 10/23/2008, Koichi Sugiura <koichi.sugiura@ntt-at.co.jp>
 *   - replaced ceil( double ) with ceil( float ) to avoid g++ bug.
 *
 * 11/28/2008, Csaba Kos <csaba.kos@ex.ssh.ntt-at.co.jp>
 *   - rewritten the formula for calculating the bit-width of the prediction
 *     order using integer operations (instead of floating-point).
 *
 * 09/04/2009, Csaba Kos <csaba.kos@ex.ssh.ntt-at.co.jp>
 *   - added support for ALS profile conformance checking
 *
 * 11/16/2009, Csaba Kos <csaba.kos@as.ntt-at.co.jp>
 *   - fixed rounding bug when channel sorting is enabled
 *
 * 12/28/2009, Csaba Kos <csaba.kos@as.ntt-at.co.jp>
 *   - aligned NeedTdBit calculation with the ALS specification
 *   - fixed progressive coding of the residual for short frames
 *   - use IntRes consistently in the integer decoder
 *
 * 02/03/2010, Csaba Kos <csaba.kos@as.ntt-at.co.jp>
 *   - generate the difference signal lazily
 *   - do not assume c % 2 == 0 when block length switching is
 *     used together with joint stereo
 *
 * 01/25/2011, Csaba Kos <csaba.kos@as.ntt-at.co.jp>
 *   - fixed an integer overflow in DecodeBlockReconstructRLSLMS()
 *     for frame lenths >= 32768
 *
 ************************************************************************/

#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <assert.h>
#include <limits.h>

#if defined(WIN32) || defined(WIN64)
	#include <io.h>
	#include <fcntl.h>
#endif

#include "decoder.h"
#include "bitio.h"
#include "lpc.h"
#include "audiorw.h"
#include "crc.h"
#include "wave.h"
#include "floating.h"
#include "mcc.h"
#include "lms.h"

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

namespace {

int ilog2_ceil(unsigned int x) {
	if (x == 0) return -1;

	int l = 0;
	while ((l < sizeof(x)*CHAR_BIT) && ((1u << l) < x)) ++l;
	return l;
}

} /* namespace */

// Constructor
CLpacDecoder::CLpacDecoder()
{
	fpInput = NULL;
	fpOutput = NULL;

	frames = 0;		// Number of frames
	fid = 0;		// No frame decoded yet
	CRC = 0;		// CRC initialization
	Q = 20;			// Quantizer value
	CloseInput = CloseOutput = false;
	ChanSort = 0;
	mp4file = false;
	ALSProfFillSet(ConformantProfiles);
}

CLpacDecoder::~CLpacDecoder()
{
	long i;

	if (frames > 0)
	{
		// Deallocate memory
		for (i = 0; i < Chan; i++)
			delete [] xp[i];
		delete [] xp;
		delete [] x;

		if (RLSLMS)
		{
			for (i = 0; i < Chan; i++)
			{
				delete [] rlslms_ptr.pbuf[i];
				delete [] rlslms_ptr.weight[i];
			}
			delete [] rlslms_ptr.pbuf;
			delete [] rlslms_ptr.weight;
		}

		delete [] tmpbuf;

		delete [] bbuf;
		delete [] d;
		delete [] cofQ;

		if (RA && (RAflag == 2))
			delete [] RAUsize;
	}

	if (ChanSort)
		delete [] ChPos;

	// Deallocate float buffer
	if ( SampleType == SAMPLE_TYPE_FLOAT ) Float.FreeBuffer();
	// Deallocate MCC buffer
	FreeMccDecBuffer( &MccBuf );

	// Close files
	CloseFiles();
}

short CLpacDecoder::CloseFiles()
{
	if ( fpInput ) {
		if ( CloseInput ) fclose( fpInput );
		fpInput = NULL;
		CloseInput = false;
	}

	if ( fpOutput ) {
		if ( CloseOutput ) fclose( fpOutput );
		fpOutput = NULL;
		CloseOutput = false;
	}
	return(0);
}

/*void CLpacDecoder::GetFilePositions(int *SizeIn, int *SizeOut)
{
	*SizeIn = ftell(fpInput);
	*SizeOut = ftell(fpOutput);
}*/

short CLpacDecoder::AnalyseInputFile(AUDIOINFO *ainfo, ENCINFO *encinfo, const MP4INFO& Mp4Info)
{
	BYTE tmp;
	long i;
	UINT als_id;

	// Read ALS header information ////////////////////////////////////////////////////////////////
	als_id = ReadUIntMSBfirst(fpInput);		// ALS identifier: 'ALS' + 0x00 (= 0x414C5300)
	if (als_id != 0x414C5300UL)
		return(1);

	Freq = ReadUIntMSBfirst(fpInput);			// sampling frequency
	Samples = ReadUIntMSBfirst(fpInput);		// samples
	if (Samples == 0xFFFFFFFF) {
		if ( !mp4file ) return (1);
		Samples = Mp4Info.m_Samples;
	}
	Chan = ReadUShortMSBfirst(fpInput) + 1;		// channels
	
	fread(&tmp, 1, 1, fpInput);
	FileType = tmp >> 5;						// file type			(XXXx xxxx)
	Res =  8 * (((tmp >> 2) & 0x07) + 1);		// resolution			(xxxX XXxx)
	SampleType = (tmp >> 1) & 0x01;				// floating-point		(xxxx xxXx)
	if (SampleType == 1)
		IntRes = IEEE754_PCM_RESOLUTION;
	else
		IntRes = Res;
	MSBfirst = tmp & 0x01;						// MSB/LSB first		(xxxx xxxX)

	N = ReadUShortMSBfirst(fpInput) + 1;		// frame length

	fread(&tmp, 1, 1, fpInput);
	RA = tmp;									// random access

	fread(&tmp, 1, 1, fpInput);
	RAflag = (tmp >> 6) & 0x03;					// RA location			(XXxx xxxx)
	Adapt = (tmp >> 5) & 0x01;					// adaptive order		(xxXx xxxx)
	CoefTable = (tmp >> 3) & 0x03;				// entropy coding table	(xxxX Xxxx)
	PITCH = (tmp >> 2) & 0x01;					// pitch Coding	(LTP)	(xxxx xXxx)
	P = tmp & 0x03;								// pred. order (MSBs)	(xxxx xxXX)

	fread(&tmp, 1, 1, fpInput);					// pred. order (LSBs)	(XXXX XXXX)
	P = (P << 8) | tmp;

	fread(&tmp, 1, 1, fpInput);
	Sub = (tmp >> 6) & 0x03;					// block switching		(XXxx xxxx)
	if (Sub)
		Sub += 2;
	BGMC = (tmp >> 5) & 0x01;					// entropy coding		(xxXx xxxx)
	SBpart = (tmp >> 4) & 0x01;					// subblock partition	(xxxX xxxx)
	Joint = (tmp >> 3) & 0x01;					// joint stereo			(xxxx Xxxx)
	MCC = (tmp >> 2) & 0x01;					// multi-channel coding	(xxxx xXxx)
	ChanConfig = (tmp >> 1) & 0x01;				// channel config.		(xxxx xxXx)
	ChanSort = tmp & 0x01;						// new channel sorting	(xxxx xxxX)

	fread(&tmp, 1, 1, fpInput);
	CRCenabled = (tmp >> 7) & 0x01;				// CRC enabled			(Xxxx xxxx)
	RLSLMS = (tmp >> 6) & 0x01;					// RLSLMS mode			(xXxx xxxx)
	AUXenabled = tmp & 0x01;					// AUX data present		(xxxx xxxX)

	if (ChanConfig)
		ReadUShortMSBfirst(fpInput);			// channel configuration data (not used)

	if (ChanSort)
	{
		CBitIO in;
		unsigned int u;
		ChPos = new unsigned short[Chan];

		i = (Chan > 1) ? (Chan-1) : 1;
		NeedPuchBit = 0;
		while(i) {i /= 2; NeedPuchBit++;}

		long size = (NeedPuchBit*Chan+7)/8;
		unsigned char* buff = new unsigned char[size];
		fread(buff, 1, size, fpInput);
		in.InitBitRead(buff);

		for (i = 0; i < Chan; i++)
		{
			in.ReadBits(&u, NeedPuchBit);
			ChPos[i] = u;
		}
		in.EndBitRead();
		delete [] buff;
	}

	HeaderSize = ReadUIntMSBfirst(fpInput);	// header size
	TrailerSize = ReadUIntMSBfirst(fpInput);	// trailer size
	TrailerOffset = 0;
	
	// End of header information //////////////////////////////////////////////////////////////////

	// Set parameter structure
	ainfo->FileType = (unsigned char)FileType;
	ainfo->MSBfirst = (unsigned char)MSBfirst;
	ainfo->Chan = Chan;
	ainfo->Res = Res;
	ainfo->IntRes = IntRes;
	ainfo->SampleType = SampleType;
	ainfo->Samples = Samples;
	ainfo->Freq = Freq;
	ainfo->HeaderSize = ( HeaderSize == 0xffffffff ) ? Mp4Info.m_HeaderSize : HeaderSize;
	ainfo->TrailerSize = ( TrailerSize == 0xffffffff ) ? Mp4Info.m_TrailerSize : TrailerSize;

	encinfo->FrameLength = N;
	encinfo->AdaptOrder = Adapt;
	encinfo->JointCoding = Joint;
	encinfo->SubBlocks = Sub;
	encinfo->RandomAccess = RA;
	encinfo->BGMC = BGMC;
	encinfo->MaxOrder = P;
	encinfo->MCC = MCC;
	encinfo->PITCH = PITCH;
	encinfo->RAflag = RAflag;
	encinfo->CRCenabled = CRCenabled;
	encinfo->RLSLMS = RLSLMS;

	CheckAlsProfiles_Header(ConformantProfiles, ainfo, encinfo);

	return(0);
}

ALS_INT64 CLpacDecoder::WriteHeader( const MP4INFO& Mp4Info )
{
	long i, j, rest;

	// Copy header
	if ( HeaderSize == 0xffffffff ) {
		if ( !mp4file ) return ( frames = -2 );
		if ( !CopyData( Mp4Info.m_pOriginalFile, Mp4Info.m_HeaderOffset, Mp4Info.m_HeaderSize, fpOutput ) ) return ( frames = -2 );
	} else {
		if ( HeaderSize >> 32 ) return ( frames = -2 );
		if ( !CopyData( fpInput, HeaderSize, fpOutput ) ) return ( frames = -2 );
	}
	
	// Read trailer
	if ( TrailerSize != 0xffffffff ) {
		TrailerOffset = ftell( fpInput );
		fseek( fpInput, TrailerSize, SEEK_CUR );
	}

	if (CRCenabled)
	{
		CRCorg = ReadUIntMSBfirst(fpInput);
		// Initialize CRC
		BuildCRCTable();
		CRC = CRC_MASK;
	}

	// Number of frames
	frames = Samples / N;
	rest = static_cast<long>( Samples % N );
	if (rest)
		frames++;

	if (RA)
	{
		// number of random acess units
		if ( frames / RA > 0x7fffffff ) return ( frames = -2 );
		RAUnits = static_cast<long>( frames / RA );
		if (frames % RA)
			RAUnits++;
		RAUid = 0;			// RAU index (0..RAUnits-1)

		if (RAflag == 2)		// read random access info from header
		{
			RAUsize = new unsigned int[RAUnits];
			for (long r = 0; r < RAUnits; r++)
				RAUsize[r] = ReadUIntMSBfirst(fpInput);
		}
	}

	if (AUXenabled)	// aux data present
	{
		unsigned int size = ReadUIntMSBfirst(fpInput);	// size of aux data
		if (size == 0xFFFFFFFF)
			size = 0;
		fseek(fpInput, size, SEEK_CUR);				// skip aux data
	}

	if (Chan == 1)
		Joint = 0;

	// Allocate memory
	xp = new int*[Chan];
	x = new int*[Chan];
	for (i = 0; i < Chan; i++)
	{
		xp[i] = new int[N+P];
		x[i] = xp[i] + P;
		memset(xp[i], 0, sizeof(int)*P);
	}
	
	// following buffer size is enough if only forward predictor is used.
	//	tmpbuf = new unsigned char[((long)((IntRes+7)/8)+1)*N + (4*P + 128)*(1+(1<<Sub))];
	// for RLS-LMS
	tmpbuf = new unsigned char[long(IntRes/8+10)*N + MAXODR * 4];

	if (RLSLMS)
	{
		rlslms_ptr.pbuf = new BUF_TYPE*[Chan];
		rlslms_ptr.weight  = new W_TYPE*[Chan];
		rlslms_ptr.Pmatrix  = new P_TYPE*[Chan];
		for (i = 0; i < Chan; i++)
		{
			rlslms_ptr.pbuf[i] = new BUF_TYPE[TOTAL_LMS_LEN];
			rlslms_ptr.weight[i]=new W_TYPE[TOTAL_LMS_LEN];
			rlslms_ptr.Pmatrix[i] = new P_TYPE[JS_LEN*JS_LEN];
			for(j=0;j<TOTAL_LMS_LEN;j++) rlslms_ptr.pbuf[i][j]=0;
		}
	}

	// following buffer size is enough if only forward predictor is used.
	//	bbuf = new unsigned char[(((long)((IntRes+7)/8)+1)*N + (4*P + 128)*(1+(1<<Sub))) * Chan];	// Input buffer
	// for RLS-LMS
	bbuf = new unsigned char[long(IntRes/8+10)*Chan*N];	// Input buffer

	// Allocate float buffer
	if ( SampleType == SAMPLE_TYPE_FLOAT ) Float.AllocateBuffer( Chan, N, IntRes );
	// Allocate MCC buffer
	if (Freq >= 192000L)
		NeedTdBit = 7;
	else if (Freq >= 96000L)
		NeedTdBit = 6;
	else
		NeedTdBit = 5;

	AllocateMccDecBuffer( &MccBuf, Chan, N);

	// #bits/channel: NeedPuchBit = max(1,ceil(log2(Chan)))
	i = (Chan > 1) ? (Chan-1) : 1;
	NeedPuchBit = 0;
	while(i){
		i /= 2;
		NeedPuchBit++;
	}

	d = new int[N];								// Prediction residual
	cofQ = new int[P];								// Quantized coefficients

	// Length of last frame
	if (rest)
		N0 = rest;
	else
		N0 = N;

	return(frames);
}

ALS_INT64 CLpacDecoder::WriteTrailer( const MP4INFO& Mp4Info )
{
	if ( TrailerSize == 0xffffffff ) {
		if ( !mp4file ) return -1;
		if ( !CopyData( Mp4Info.m_pOriginalFile, Mp4Info.m_TrailerOffset, Mp4Info.m_TrailerSize, fpOutput ) ) return -1;
	} else {
		if ( TrailerSize >> 32 ) return -1;
		fseek( fpInput, TrailerOffset, SEEK_SET );
		if ( !CopyData( fpInput, TrailerSize, fpOutput ) ) return -1;
	}

	CRC ^= CRC_MASK;
	CRC -= CRCorg;		// CRC = 0 if decoding was successful

	return ( TrailerSize == 0xffffffff ) ? Mp4Info.m_TrailerSize : TrailerSize;
}

short CLpacDecoder::DecodeAll( const MP4INFO& Mp4Info )
{
	long f;

	if ((frames = WriteHeader( Mp4Info )) < 1)
		return static_cast<short>( frames );

	// Main loop for all frames
	for (f = 0; f < frames; f++)
	{
		// Decode one frame
		if (DecodeFrame())
			return(-2);
	}

	if (WriteTrailer( Mp4Info ) < 0)
		return(-2);

	return(CRC != 0);	// Return CRC status
}

unsigned int CLpacDecoder::GetCRC()
{
	return(CRC);
}

/*short CLpacDecoder::GetFrameSize()
{
	return(N * Chan * (IntRes / 8));
}

long CLpacDecoder::GetOrgFileSize()
{
	return(FileSize);
}

long CLpacDecoder::GetPacFileSize()
{
	long pos, size;

	pos = ftell(fpInput);
	fseek(fpInput, 0, SEEK_END);
	size = ftell(fpInput);
	fseek(fpInput, pos, SEEK_SET);

	return(size);
}*/

short GetBlockSequence(unsigned long BSflags, long N, long *Nb)
{
	short B = 0;

	if (!(BSflags & 0x40000000))
		Nb[B++] = N;
	else						// N/2
	{
		for (short b = 0; b < 2; b++)
		{
			if (!(BSflags & (0x20000000 >> b)))
				Nb[B++] = N >> 1;			
			else						// N/4
			{
				for (short bb = 0; bb < 2; bb++)
				{
					if (!(BSflags & (0x08000000 >> ((b<<1)+bb) )))
						Nb[B++] = N >> 2;
					else						// N/8
					{
						for (short bbb = 0; bbb < 2; bbb++)
						{
							if (!(BSflags & (0x00800000 >> ((b<<2)+(bb<<1)+bbb) )))
								Nb[B++] = N >> 3;
							else						// N/16
							{
								for (short bbbb = 0; bbbb < 2; bbbb++)
								{
									if (!(BSflags & (0x00008000 >> ((b<<3)+(bb<<2)+(bbb<<1)+bbbb) )))
										Nb[B++] = N >> 4;
									else						// N/32
									{
										Nb[B++] = N >> 5;
										Nb[B++] = N >> 5;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return(B);
}

// Decode frame
short CLpacDecoder::DecodeFrame()
{
	short RAframe;
	BYTE h1;
	long c, c1, b, B, oaa;
	long i, Nb[32], NN, Nsum, mtp;
	UINT BSflags;
	short CBS;
	BYTE h, typ, flag;

	int **xsave, **xtmp;
	xsave = new int*[Chan];
	xtmp = new int*[Chan];
	
	fid++;						// Number of current frame

	NN = N;

	// Random Access
	RAframe = RA;
	if (RA)
	{
		if (((fid - 1) % RA))	// Not first frame of RA unit
			RAframe = 0;		// Turn off RA for current frame
		else if (RAflag == 1)
			ReadUIntMSBfirst(fpInput);		// read size of RAU
	}

	MCCflag=0;
	if(MCC)
	{
		MCCflag=1;
		if(Joint)
		{
			unsigned char uu;
			fread(&uu, 1, 1, fpInput);
			if(uu) MCCflag=0;
			else MCCflag=1;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Normal coding (no multi-channel correlation method) ////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(!MCCflag && !RLSLMS)
	{
		// Save original pointers
		for (c = 0; c < Chan; c++)
			xsave[c] = x[c];

		// Channels ///////////////////////////////////////////////////////////////////////////////////
		for (c = 0; c < Chan; c++)
		{
			CBS = (Joint && (c < Chan - 1) && (c % 2 == 0 || Sub));	// Joint coding, and not the last channel

			if (Sub)	// block switching enabled
			{
				// read block switching info
				fread(&h1, 1, 1, fpInput);
				if (h1 & 0x80)	// if independent block switching is indicated...
					CBS = 0;	// ...turn off channel coupling
				BSflags = h1 << 24;
				if (Sub > 3)
				{
					fread(&h1, 1, 1, fpInput);
					BSflags |= h1 << 16;
				}
				if (Sub > 4)
				{
					fread(&h1, 1, 1, fpInput);
					BSflags |= h1 << 8;
					fread(&h1, 1, 1, fpInput);
					BSflags |= h1;
				}

				// get #blocks B and block lengths Nb[]
				B = GetBlockSequence(BSflags, NN, Nb);
			}
			else		// fixed block length (= frame length)
			{
				B = 1;
				Nb[0] = N;
			}

			if (fid == frames)	// last frame needs some recalculation
			{
				Nsum = 0;
				B = 0;
				while (Nsum < N0)
					Nsum += Nb[B++];
				Nb[B-1] -= (Nsum - N0);
				N = N0;

				/*fprintf(stderr, "\nBSflags = %X\n", BSflags);
				for (b = 0; b < B; b++)
					fprintf(stderr, "\nNb[%d] = %d\n", b, Nb[b]);*/
			}

			if (CBS)	// Decode two coupled channels
			{
				c1 = c + 1;		// index of second channel

				for (b = 0; b < B; b++)
				{
					// Difference method
					fread(&h, 1, 1, fpInput);
					typ = h >> 6;
					flag = h & 0x20;
					fseek(fpInput, -1, SEEK_CUR);

					if ((typ == 0x03) || ((typ < 0x02) && flag))	// Channel 1 = difference signal
					{
						if (!(RAframe && (b == 0))) {
							/* Prepare P samples from the previous block */
							for (i = -P; i < 0; i++)
								x[c][i] = x[c1][i] - x[c][i];
						}

						DecodeBlock(x[c],  Nb[b], RAframe && (b == 0));
						DecodeBlock(x[c1], Nb[b], RAframe && (b == 0));

						if (!(RAframe && (b == 0))) {
							/* Restore P samples from the previous block */
							for (i = -P; i < 0; i++)
								x[c][i] = x[c1][i] - x[c][i];
						}

						/* Restore signal from difference */
						for (i = 0; i < Nb[b]; i++)
							x[c][i] = x[c1][i] - x[c][i];
					}
					else											// Channel 1 = normal signal
					{
						DecodeBlock(x[c], Nb[b], RAframe && (b == 0));

						fread(&h, 1, 1, fpInput);
						typ = h >> 6;
						flag = h & 0x20;
						fseek(fpInput, -1, SEEK_CUR);

						if ((typ == 0x03) || ((typ < 0x02) && flag))	// Channel 2 = difference signal
						{
							if (!(RAframe && (b == 0))) {
								/* Prepare P samples from the previous block */
								for (i = -P; i < 0; i++)
									x[c1][i] -= x[c][i];
							}
							DecodeBlock(x[c1], Nb[b], RAframe && (b == 0));

							if (!(RAframe && (b == 0))) {
								/* Restore P samples from the previous block */
								for (i = -P; i < 0; i++)
									x[c1][i] += x[c][i];
							}

							/* Restore signal from difference */
							for (i = 0; i < Nb[b]; i++)
								x[c1][i] += x[c][i];
						}
						else											// Channel 2 = normal signal
						{
							DecodeBlock(x[c1], Nb[b], RAframe && (b == 0));
						}
					}
					// Increment pointers (except for last subblock)
					if (b < B - 1)
					{
						x[c] = x[c] + Nb[b];
						x[c1] = x[c1] + Nb[b];
					}
				}
				// increment channel index
				c++;
			}
			else	// Decode one independent channel
			{
				for (b = 0; b < B; b++)
				{
					DecodeBlock(x[c], Nb[b], RAframe && (b == 0));

					// Increment pointers (except for last subblock)
					if (b < B - 1)
						x[c] = x[c] + Nb[b];
				}
			}
		}
		// End of Channels ////////////////////////////////////////////////////////////////////////////

		// Restore original pointers
		for (c = 0; c < Chan; c++){
			
			x[c] = xsave[c];
			//if (fid == frames) fprintf(stderr,"%d=%d\n",c,x[c][0]);
		}

		
	}
	else if (RLSLMS)//   RLSLMS -mode
	{
			short cpe,c0,c1,sce;
			//MCCflag = 0;
			if (fid == frames)	// last frame needs some recalculation
			{
				N = N0;
			}
			// Channel Pair Elements

			for (cpe = 0; cpe < Chan/2; cpe++)
			{
				// Channel numbers for this CPE
				c0 = 2 * cpe;
				c1 = c0 + 1;	

				if (!Joint)			// Independent Stereo
				{
					if (RLSLMS)
					{
						rlslms_ptr.channel=c0;
						DecodeBlockParameter( &MccBuf, c0, N, RAframe);
						DecodeBlockReconstructRLSLMS( &MccBuf, c0, x[c0]);
						synthesize(x[c0], N, &rlslms_ptr, RAframe || RLSLMS_ext==7, &MccBuf);

						rlslms_ptr.channel=c1;
						DecodeBlockParameter( &MccBuf, c1, N, RAframe);
						DecodeBlockReconstructRLSLMS( &MccBuf,  c1, x[c1]);

						synthesize(x[c1], N, &rlslms_ptr, RAframe || RLSLMS_ext==7, &MccBuf);
					}
				}
				else	// Joint Stereo
				{
					if (RLSLMS)
					{
						// Difference method
						char *xpr0=&MccBuf.m_xpara[c0];
						char *xpr1=&MccBuf.m_xpara[c1];
						
						DecodeBlockParameter( &MccBuf, c0, N, RAframe);
						DecodeBlockReconstructRLSLMS( &MccBuf, c0, x[c0]);
						DecodeBlockParameter( &MccBuf, c1, N, RAframe);
						DecodeBlockReconstructRLSLMS( &MccBuf, c1, x[c1]);
						rlslms_ptr.channel=c0;
						synthesize_joint(x[c0],x[c1], N, &rlslms_ptr, RAframe || RLSLMS_ext==7, mono_frame, &MccBuf);
					}
				}
			}

			// Single Channel Elements
			for (sce = 0; sce < Chan%2; sce++)
			{
				c0 = sce+2*(Chan/2);
				DecodeBlockParameter( &MccBuf, c0, N, RAframe);
				DecodeBlockReconstructRLSLMS( &MccBuf, c0, x[c0]);
				rlslms_ptr.channel=c0;
				synthesize(x[c0], N, &rlslms_ptr, RAframe || RLSLMS_ext==7, &MccBuf);
			}
			RLSLMS_ext=0; // reset for next frame
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of normal coding - Multi-channel correlation method ////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else
	{

		// Save original pointers
		for (c = 0; c < Chan; c++)
			xsave[c] = x[c];

		if (Sub)	// block switching enabled
		{
			// read block switching info
			fread(&h1, 1, 1, fpInput);
			BSflags = h1 << 24;
			if (Sub > 3)
			{
				fread(&h1, 1, 1, fpInput);
				BSflags |= h1 << 16;
			}
			if (Sub > 4)
			{
				fread(&h1, 1, 1, fpInput);
				BSflags |= h1 << 8;
				fread(&h1, 1, 1, fpInput);
				BSflags |= h1;
			}

			// get #blocks B and block lengths Nb[]
			B = GetBlockSequence(BSflags, NN, Nb);
		}
		else		// fixed block length (= frame length)
		{
			B = 1;
			Nb[0] = N;
		}

		if (fid == frames)	// last frame needs some recalculation
		{
			Nsum = 0;
			B = 0;
			while (Nsum < N0)
				Nsum += Nb[B++];
			Nb[B-1] -= (Nsum - N0);
			N = N0;
		}
			
		for (b = 0; b < B; b++)
		{
			InitMccDecBuffer( &MccBuf );

			for(c = 0; c < Chan; c++)
				DecodeBlockParameter( &MccBuf, c, Nb[b], RAframe && (b == 0)); // Get residual and parameter

			for(oaa = 0; oaa < OAA; oaa++)
			{
				for(c = 0; c < Chan; c++)
				{
					MccBuf.m_tmppuchan[c] = MccBuf.m_puchan[c][OAA-1-oaa];
					MccBuf.m_tmptdtau[c] = MccBuf.m_tdtau[c][OAA-1-oaa];
					MccBuf.m_tmpMM[c] = MccBuf.m_MccMode[c][OAA-1-oaa];
					for(mtp = 0; mtp < Mtap; mtp++) MccBuf.m_mtgmm[c][mtp]=MccBuf.m_cubgmm[c][OAA-1-oaa][mtp];
				}
				ReconstructResidualTD( &MccBuf, Chan, Nb[b] ); // d[] = (subtracted d[]) + gamma*(reference d[])
			}

			for(c = 0; c < Chan; c++)
			{
				DecodeBlockReconstruct( &MccBuf, c, x[c], Nb[b], RAframe && (b == 0)); // Residual --> Original signal

				// Increment pointers (except for last subblock)
				if (b < B - 1)
					x[c] = x[c] + Nb[b];
			}
		}

		// Restore original pointers
		for (c = 0; c < Chan; c++)
			x[c] = xsave[c];
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// End of MCC /////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Copy last P samples of each frame in front of it
	for (c = 0; c < Chan; c++)
		memcpy(x[c] - P, x[c] + (N - P), P * sizeof(int));

	// Write PCM audio data
	if ( SampleType == SAMPLE_TYPE_INT )
	{
		if (ChanSort)
		{
			// Restore original channel pointers
			for (c = 0; c < Chan; c++)
				xtmp[c] = x[c];
			for (c = 0; c < Chan; c++)
				x[ChPos[c]] = xtmp[c];
		}

		if (Res == 16)
		{
			if ((Write16BitNM(x, Chan, N, MSBfirst, bbuf, fpOutput) != 2L*Chan*N) && (fpOutput != NULL)) return(-1);
			CRC = CalculateBlockCRC32(2L*Chan*N, CRC, (void*)bbuf);
		}
		else if (Res == 8)
		{
			if ((Write8BitOffsetNM(x, Chan, N, bbuf, fpOutput) != (long)Chan*N) && (fpOutput != NULL)) return(-1);
			CRC = CalculateBlockCRC32((long)Chan*N, CRC, (void*)bbuf);
		}
		else if (Res == 24)
		{
			if ((Write24BitNM(x, Chan, N, MSBfirst, bbuf, fpOutput) != 3L*Chan*N) && (fpOutput != NULL)) return(-1);
			CRC = CalculateBlockCRC32(3L*Chan*N, CRC, (void*)bbuf);
		}
		else	// Res == 32
		{
			if ((Write32BitNM(x, Chan, N, MSBfirst, bbuf, fpOutput) != 4L*Chan*N) && (fpOutput != NULL)) return(-1);
			CRC = CalculateBlockCRC32(4L*Chan*N, CRC, (void*)bbuf);
		}

		if (ChanSort)
		{
			// Rearrange channel pointers
			for (c = 0; c < Chan; c++)
				xtmp[c] = x[c];
			for (c = 0; c < Chan; c++)
				x[c] = xtmp[ChPos[c]];
		}

	}
	else	// Floating-point PCM
		Float.ReformatData( x, N );

	if ( SampleType == SAMPLE_TYPE_FLOAT ) {
		if ( !Float.DecodeDiff( fpInput, N, RAframe != 0 ) ) return -1;
		if ( !Float.AddIEEEDiff( N ) ) return -1;
		if ( ChanSort ) Float.ChannelSort( ChPos, false );
		Float.ConvertFloatToRBuff( bbuf, N );
		if ( ChanSort ) Float.ChannelSort( ChPos, true );

		// Write floating point data into output file
		if ( fwrite( bbuf, 1, N * Chan * IEEE754_BYTES_PER_SAMPLE, fpOutput ) != N * Chan * IEEE754_BYTES_PER_SAMPLE ) {
			// Write error
			return -1;
		}
		CRC = CalculateBlockCRC32( Chan * N * IEEE754_BYTES_PER_SAMPLE, CRC, static_cast<void*>( bbuf ) );
	}

	delete [] xsave;
	delete [] xtmp;

	return(0);
}

// Decode block (Normal)
short CLpacDecoder::DecodeBlock(int *x, long Nb, short ra)
{
	DecodeBlockParameter(&MccBuf, 0, Nb, ra);
	DecodeBlockReconstruct(&MccBuf, 0, x, Nb, ra);
	return (0);
}

// Decode block parameter
void CLpacDecoder::DecodeBlockParameter(MCC_DEC_BUFFER *pBuffer, long Channel, long Nb, short ra)
{
	char *xpra = &pBuffer->m_xpara[Channel];
	int *d = pBuffer->m_dmat[Channel];
	int *puch = pBuffer->m_puchan[Channel];
	int *mccparq = pBuffer->m_parqmat[Channel];
	short *sft = &pBuffer->m_shift[Channel];
	short *oP = &pBuffer->m_optP[Channel];
	long oaa;
	//MCC-ex
	int *tdtauval = pBuffer->m_tdtau[Channel];
	short *MccMode = pBuffer->m_MccMode[Channel];
	int **mtgmm=pBuffer->m_cubgmm[Channel];

	BYTE h, hl[4];
	short BlockType, optP, shift = 0;
	int c;
	long count, bytes, i, Ns;
    int asi[1023];
    int parq[1023];
	UINT u;
	short s[16], sx[16], sub;
    // Bit-oriented input
    CBitIO in;
    /* reconstruction levels for 1st and 2nd coefficients: */
    static int pc12_tbl[128] = {
        -1048544, -1048288, -1047776, -1047008, -1045984, -1044704, -1043168, -1041376, -1039328,
        -1037024, -1034464, -1031648, -1028576, -1025248, -1021664, -1017824, -1013728, -1009376,
        -1004768, -999904, -994784, -989408, -983776, -977888, -971744, -965344, -958688, -951776,
        -944608, -937184, -929504, -921568, -913376, -904928, -896224, -887264, -878048, -868576,
        -858848, -848864, -838624, -828128, -817376, -806368, -795104, -783584, -771808, -759776,
        -747488, -734944, -722144, -709088, -695776, -682208, -668384, -654304, -639968, -625376,
        -610528, -595424, -580064, -564448, -548576, -532448, -516064, -499424, -482528, -465376,
        -447968, -430304, -412384, -394208, -375776, -357088, -338144, -318944, -299488, -279776,
        -259808, -239584, -219104, -198368, -177376, -156128, -134624, -112864, -90848, -68576,
        -46048, -23264, -224, 23072, 46624, 70432, 94496, 118816, 143392, 168224, 193312, 218656,
        244256, 270112, 296224, 322592, 349216, 376096, 403232, 430624, 458272, 486176, 514336,
        542752, 571424, 600352, 629536, 658976, 688672, 718624, 748832, 779296, 810016, 840992,
        872224, 903712, 935456, 967456, 999712, 1032224 };

    /* rice code parameters for each coeff: */
	struct pv {int m,s;} *parcor_vars;

	// 48kHz
	static struct pv parcor_vars_0[20] = {
        {-52, 4}, {-29, 5}, {-31, 4}, { 19, 4}, {-16, 4}, { 12, 3}, { -7, 3}, {  9, 3}, { -5, 3}, {  6, 3}, { -4, 3}, {  3, 3}, { -3, 2}, {  3, 2}, { -2, 2}, {  3, 2}, { -1, 2}, {  2, 2}, { -1, 2}, {  2, 2}  // i&1, 2
    };

    // 96kHz
	static struct pv parcor_vars_1[20] = {
        {-58, 3}, {-42, 4}, {-46, 4}, { 37, 5}, {-36, 4}, { 29, 4}, {-29, 4}, { 25, 4}, {-23, 4}, { 20, 4}, {-17, 4}, { 16, 4}, {-12, 4}, { 12, 3}, {-10, 4}, {  7, 3}, { -4, 4}, {  3, 3}, { -1, 3}, {  1, 3}  // i&1, 2
    };

    // 192 kHz
	static struct pv parcor_vars_2[20] = {
        {-59, 3}, {-45, 5}, {-50, 4}, { 38, 4}, {-39, 4}, { 32, 4}, {-30, 4}, { 25, 3}, {-23, 3}, { 20, 3}, {-20, 3}, { 16, 3}, {-13, 3}, { 10, 3}, { -7, 3}, {  3, 3}, {  0, 3}, { -1, 3}, {  2, 3}, { -1, 2}  // i&1, 2
    };

	if (CoefTable == 0)
		parcor_vars = parcor_vars_0;
	else if (CoefTable == 1)
		parcor_vars = parcor_vars_1;
	else if (CoefTable == 2)
		parcor_vars = parcor_vars_2;

    optP = 10;

	// Read block header
	fread(&h, 1, 1, fpInput);
	BlockType = h >> 6;			// Type of block

	// ZERO BLOCK
	if (BlockType == 0)
	{
		*xpra=1;
	}
	// CONSTANT BLOCK
	else if (BlockType == 1)
	{
		fread( hl, 1, ( IntRes + 7 ) / 8, fpInput );
		if ( IntRes <= 8 ) {
			c = static_cast<int>( hl[0] ) - 128;
		}
		else if (IntRes <= 16)
		{
			c = short((hl[0] << 8) + hl[1]);
		}
		else if (IntRes <= 24)
		{
			c = hl[2] | (hl[1] << 8) | (hl[0] << 16);
			if (c & 0x00800000)
				c = c | 0xFF000000;
		}
		else	// IntRes > 24
		{
			c = hl[3] | (hl[2] << 8) | (hl[1] << 16) | (hl[0] << 24);
		}
		
		d[0] = c;
		*xpra=2;
	}
	// NORMAL BLOCK
	else if (BlockType > 1)
	{
		fseek(fpInput, -1, SEEK_CUR);	// Go back one byte which was already read
		// following buffer size is enough if only forward predictor is used.
		//		count = fread(tmpbuf, 1, (((long)((IntRes+7)/8)+1)*Nb + 4*P + 128), fpInput);   // Fill input buffer
        // for RLS-LMS
		count = fread(tmpbuf, 1, long(IntRes/8+10) * Nb + P + 16 + 255*4, fpInput);   // Fill input buffer

        in.InitBitRead(tmpbuf);
		
		in.ReadBits(&u, 2);		// 1J

		if (!BGMC)
		{
			if (SBpart)
			{
				in.ReadBits(&u, 1);
				if (u)
					sub = 4;
				else
					sub = 1;
			}
			else
				sub = 1;
		}
		else
		{
			if (SBpart)
			{
				in.ReadBits(&u, 2);
				sub = 1 << u;
			}
			else
			{
				in.ReadBits(&u, 1);
				if (u)
					sub = 4;
				else
					sub = 1;
			}
		}

		// Code parameters for EC blocks
		if (!BGMC)
		{
			short sbits = IntRes <= 16 ? 4 : 5;
			in.ReadBits(&u, sbits);
			s[0] = u;
			for (i = 1; i < sub; i++)
			{
				in.ReadRice(&c, 0, 1);
				s[i] = s[i-1] + c;
			}
		}
		else
		{
			short sbits = IntRes <= 16 ? 8 : 9;
			UINT S[16];
			in.ReadBits(&S[0], sbits);
			for (i = 1; i < sub; i++)
			{
				in.ReadRice(&c, 2, 1);
				S[i] = S[i-1] + c;
			}
			// Separate BGMC code parameters
			for (i = 0; i < sub; i++)
			{
				s[i] = S[i] >> 4;
				sx[i] = S[i] & 0x0F;
			}
		}

		// LSB shift
		shift = 0;
		in.ReadBits(&u, 1);
		if (u)
		{
			in.ReadBits(&u, 4);
			shift = short(u + 1);
		}
		if (!RLSLMS)
		{
			if (Adapt)
			{
				// Limit maximum predictor order
				h = (Nb < 8) ? 1 : min(ilog2_ceil(P+1), max(ilog2_ceil(Nb >> 3), 1));
				in.ReadBits(&u, h);
				optP = (short)u;
			}
			else
				optP = P;

			/* decode quantized coefficients: */
			if (CoefTable == 3)
			{
				for (i = 0; i < optP; i++)
				{
					in.ReadBits(&u, 7);
					asi[i] = (int)u - 64;
				}
			}
			else
			{
				for (i = 0; i < min(optP,20); i++)
					asi[i] = parcor_vars[i].m + rice_decode (parcor_vars[i].s, &in.bio);
				for (; i < min(optP,127); i++)
					asi[i] = (i&1) + rice_decode (2, &in.bio);
				for (; i < optP; i++)
					asi[i] = rice_decode (1, &in.bio);
			}
			
			/* reconstruct parcor coefficients: */
			if (optP > 0)
				parq[0] = pc12_tbl[asi[0] + 64];
			if (optP > 1)
				parq[1] = -pc12_tbl[asi[1] + 64];
			for (i = 2; i < optP; i++)
				parq[i] = (asi[i] << (Q -6)) + (1 << (Q-7));
		}

		if(PITCH) pBuffer->m_Ltp.Decode( Channel, Nb, Freq, &in );

		Ns = Nb / sub;
		if (!ra)	// No random access
		{
			if (!BGMC)	// Use Rice codes
			{
				for (i = 0; i < sub; i++)
					in.ReadRice(d+(i*Ns), (char)s[i], Ns);
			}
			else		// Use block Gilbert-Moore codes for central region
				bgmc_decode_blocks(d, 0, s, sx, Nb, sub, &in.bio);
		}
		else		// Random access
		{
			// Number of separately coded samples
			short num = min(optP, min(Nb, 3));

			// Progressive prediction: Separate entropy coding of the first 3 samples
			short RiceLimit = ( IntRes <= 16 ) ? 15 : 31;
			if (num > 0)
				in.ReadRice(d, IntRes-4, 1);
			if (num > 1)
				in.ReadRice(d+1, min(s[0]+3, RiceLimit), 1);
			if (num > 2)
				in.ReadRice(d+2, min(s[0]+1, RiceLimit), 1);

	        if (!BGMC)
			{
				in.ReadRice(d + num, (char)s[0], Ns - num);	// First EC block is 'num' samples shorter
				for (i = 1; i < sub; i++)						// Remaining 3 EC blocks
					in.ReadRice(d + i*Ns, (char)s[i], Ns);
			}
			else
			{
				bgmc_decode_blocks(d, num, s, sx, Nb, sub, &in.bio);
			}
		}

		for(i = 0; i < optP; i++)
			mccparq[i] = parq[i];
		*sft = shift;
		*oP = optP;
		*xpra = 0;

		if (!RLSLMS && !MCCflag)
		{
			bytes = in.EndBitRead();					// Number of bytes read
			fseek(fpInput, bytes - count, SEEK_CUR);	// Set working pointer to current position
		}
	}

	if (RLSLMS)
	{
		if (BlockType<=1)
		{
			count = fread(tmpbuf, 1, 400, fpInput);   // Fill input buffer
			in.InitBitRead(tmpbuf);
		}
		in.ReadBits(&u,1);
		mono_frame = u;
		in.ReadBits(&u,1);
		if (u!=0)
		{
			in.ReadBits(&u,3);
			RLSLMS_ext = u;
			//printf("%d %d ",RLSLMS_ext,optP);
			if (RLSLMS_ext&0x01) 
			{
				c_mode_table.filter_len[0]=1;
				in.ReadBits(&u,4);
				c_mode_table.filter_len[1]=(u)<<1;
				in.ReadBits(&u,3);
				c_mode_table.nstage=u+2;
				for(i=2;i<c_mode_table.nstage;i++)
				{
					in.ReadBits(&u,5);
					c_mode_table.filter_len[i]=lms_order_table[u];						
				}
			}
			if (RLSLMS_ext&0x02)
			{
				if (c_mode_table.filter_len[1]){
					in.ReadBits(&u,10);
					c_mode_table.lambda[0]=u;
					in.ReadBits(&u,10);
					c_mode_table.lambda[1]=u;
				}
			}
			if (RLSLMS_ext&0x04)
			{
				for(i=2;i<c_mode_table.nstage;i++)
				{
					in.ReadBits(&u,5);
					c_mode_table.opt_mu[i]=mu_table[u];
				}
				in.ReadBits(&u,3);
				c_mode_table.step_size=u*LMS_MU_INT;
			}
		}
		if(!MCCflag)
		{
			bytes = in.EndBitRead();					// Number of bytes read
			fseek(fpInput, bytes - count, SEEK_CUR);	// Set working pointer to current position
		}
	}

	// MCC Coefficients (Get referene channels and weighting factor)
	if(MCCflag)
	{
		if ( (BlockType<=1) && !RLSLMS)
		{
			count = fread(tmpbuf, 1, 256, fpInput);   // Fill input buffer
			in.InitBitRead(tmpbuf);
		}

		for (oaa = 0; oaa < OAA+1; oaa++)
		{
			in.ReadBits(&u,1); // Endflag
			if(u)
			{
				break;
			}
		
			in.ReadBits(&u,NeedPuchBit); // Reference Channel
			puch[oaa]=(int) u;
		
			if(puch[oaa] == Channel)
			{
				// not need weighting factor
			}
			else
			{
				in.ReadBits(&u,1);

				if(!u)
				{
					MccMode[oaa]=1;

					mtgmm[oaa][0]=rice_decode (1, &in.bio); // -1
					mtgmm[oaa][1]=rice_decode (2, &in.bio); //  0
					mtgmm[oaa][2]=rice_decode (1, &in.bio); // +1
					mtgmm[oaa][1] -= 2;
				}
				else
				{
					MccMode[oaa]=2;

					mtgmm[oaa][0]=rice_decode (1, &in.bio); // -1
					mtgmm[oaa][1]=rice_decode (2, &in.bio); //  0
					mtgmm[oaa][2]=rice_decode (1, &in.bio); // +1
					mtgmm[oaa][3]=rice_decode (1, &in.bio); // Tau-1
					mtgmm[oaa][4]=rice_decode (1, &in.bio); // Tau
					mtgmm[oaa][5]=rice_decode (1, &in.bio); // Tau+1
					mtgmm[oaa][1] -= 2;

					UINT v;
					in.ReadBits(&v,1);
					in.ReadBits(&u,NeedTdBit);
					tdtauval[oaa]=(int)u+3;
					if(v) tdtauval[oaa] *=-1;
				}
				
				for(i = 0; i < Mtap; i++)
						mtgmm[oaa][i] += 16;
			}
		}
		CheckAlsProfiles_MCCStages(ConformantProfiles, oaa);
		bytes = in.EndBitRead();					// Number of bytes read
		fseek(fpInput, bytes - count, SEEK_CUR);	// Set working pointer to current position
	}
}

short	CLpacDecoder::DecodeBlockReconstruct( MCC_DEC_BUFFER* pBuffer, long Channel, int* x, long Nb, short ra )
{
	CLtpBuffer*	pLtpBuf = pBuffer->m_Ltp.m_pBuffer + Channel;
	int*	parq = pBuffer->m_parqmat[Channel];
	int*	d = pBuffer->m_dmat[Channel];
	short	shift = pBuffer->m_shift[Channel];
	short	optP = pBuffer->m_optP[Channel];
	short	xpara = pBuffer->m_xpara[Channel];
	long	i;
	int*	xtmp;

	if ( xpara == 1 ) {
		memset( x, 0, sizeof(int) * Nb );

	} else if ( xpara == 2 ) {
		for( i=0; i<Nb; i++ ) x[i] = d[0];

	} else {
		if ( shift ) {
			xtmp = new int [optP];
			// "Shift" last P samples of previous block
			for( i=-optP; i<0; i++ ) {
				xtmp[optP+i] = x[i];		// buffer original values
				x[i] >>= shift;
			}
		}

		if ( PITCH ) {
			int*	dd = pLtpBuf->m_ltpmat + 2048;
			if ( pLtpBuf->m_ltp ) {
				memcpy( dd, d, Nb * sizeof(int) );
				memset( dd-2048, 0, 2048 * sizeof(int) );
				pBuffer->m_Ltp.PitchReconstruct( dd, Nb, optP, Channel, Freq );
				memcpy( dd-2048, dd+Nb-2048, 2048 * sizeof(int) );
				memcpy( d, dd, Nb * sizeof(int) );
			} else {
				memcpy( dd, d, Nb * sizeof(int) );
				memcpy( dd-2048, dd+Nb-2048, 2048 * sizeof(int) );
			}
		}

		if ( !ra ) {
			// Conversion from parcor to direct form coefficients
			par2cof( cofQ, parq, optP, Q );
			// Calculate original signal
			GetSignal( x, Nb, optP, Q, cofQ, d );
		} else {
			// Progressive prediction (internal conversion)
			GetSignalRA( x, Nb, optP, Q, parq, cofQ, d );
		}

		if ( shift ) {
			// Undo "shift" of whole block (and restore optP samples of the previous block)
			for( i=-optP; i<0; i++ ) x[i] = xtmp[optP+i];
			for( i=0; i<Nb; i++ ) x[i] <<= shift;
			delete[] xtmp;
		}
	}
	return 0;
}

// Decode block reconstruct
short CLpacDecoder::DecodeBlockReconstructRLSLMS(MCC_DEC_BUFFER *pBuffer, long Channel, int *x)
{
	CLtpBuffer*	pLtpBuf = pBuffer->m_Ltp.m_pBuffer + Channel;
	int *parq = pBuffer->m_parqmat[Channel];
	int *d = pBuffer->m_dmat[Channel];
	short shift = pBuffer->m_shift[Channel];
	short optP = pBuffer->m_optP[Channel];
	short xpara = pBuffer->m_xpara[Channel];

	long i;

	if(xpara == 1)
		memset(x, 0, sizeof(int) * N);
	else if(xpara == 2)
		for(i = 0; i < N; i++) x[i]=d[0];
	else
	{
		if (shift)
		{
			// "Shift" last P samples of previous block
			for (i = -optP; i < 0; i++)
				x[i] >>= shift;
		}
		// rescontruct the pitch information
		if ( PITCH ) {
			int*	dd = pLtpBuf->m_ltpmat + 2048;
			if ( pLtpBuf->m_ltp ) {
				memcpy( dd, d, N * sizeof(int) );
				memset( dd-2048, 0, 2048 * sizeof(int) );
				pBuffer->m_Ltp.PitchReconstruct( dd, N, 10, Channel, Freq );
				memcpy( dd-2048, dd+N-2048, 2048 * sizeof(int) );
				memcpy( d, dd, N * sizeof(int) );
			} else {
				memcpy( dd, d, N * sizeof(int) );
				memcpy( dd-2048, dd+N-2048, 2048 * sizeof(int) );
			}
		}
		// restore the sample from d array
		for(i=0;i<N;i++) x[i]=d[i];
		if (shift)
		{
			// Undo "shift" of whole block (and P samples of the previous block)
			for (i = -optP; i < N; i++)
				x[i] <<= shift;
		}
	}

	// Copy last P samples of this block in front of it
	memcpy(x - P, x + (N - P), P * sizeof(int));

	return(0);
}

bool	CLpacDecoder::CopyData( const char* pFilename, ALS_UINT64 Offset, ALS_UINT64 Size, HALSSTREAM hOutFile )
{
	HALSSTREAM	hInFile = NULL;
	bool		Result = false;
	if ( OpenFileReader( pFilename, &hInFile ) == 0 ) {
		fseek( hInFile, Offset, SEEK_SET );
		Result = CopyData( hInFile, Size, hOutFile );
		fclose( hInFile );
	}
	return Result;
}

bool	CLpacDecoder::CopyData( HALSSTREAM hInFile, ALS_UINT64 Size, HALSSTREAM hOutFile )
{
	const ALS_UINT32	COPYDATA_BUFSIZE = 1048576;
	char*				pBuffer = NULL;
	ALS_UINT32			CopySize;
    
	// Allocate buffer.
	pBuffer = new char [ COPYDATA_BUFSIZE ];
	if ( pBuffer == NULL ) return false;

	// Copy loop.
	while( Size > 0 ) {
		CopySize = ( Size < COPYDATA_BUFSIZE ) ? static_cast<ALS_UINT32>( Size ) : COPYDATA_BUFSIZE;
		if ( fread( pBuffer, 1, CopySize, hInFile ) != CopySize ) break;
		if ( fwrite( pBuffer, 1, CopySize, hOutFile ) != CopySize ) break;
		Size -= CopySize;
	}
	delete[] pBuffer;
	return ( Size == 0 );
}
