-----------------------------------------------------------------
MPEG-4 Audio Lossless Coding - RM23
-----------------------------------------------------------------
ftp://ftlabsrv.nue.tu-berlin.de/mp4lossless/refsoft
-----------------------------------------------------------------
Noboru Harada (NTT)
Tilman Liebchen (Technical University of Berlin / LG Electronics)
-----------------------------------------------------------------
Last modified: January 25, 2011
-----------------------------------------------------------------

Files and Directories
---------------------
/bin/win            - codec binary for Windows (/Release/mp4alsRM23.exe)
/lib                - object files required for adaptive prediction order
/src                - reference model 23 source code
Makefile            - Makefile for Linux/Mac (GCC)
mp4als_vc6.dsp      - MSVC 6.0 project file
mp4als_vc6.dsw      - MSVC 6.0 workspace file
mp4als_vc7.vcproj   - Visual Studio .NET 2003 VC++ project file
mp4als_vc7.sln      - Visual Studio .NET 2003 solution file
mp4als_vc8.vcproj   - Visual Studio 2005 VC++ project file
mp4als_vc8.sln      - Visual Studio 2005 solution file
mp4als_vc9.vcproj   - Visual Studio 2008 VC++ project file
mp4als_vc9.sln      - Visual Studio 2008 solution file

RM23 Notes (Changes from RM22)
-------------------------------
- decoder: check the bit-stream and report the profile levels it conforms to
- decoder: report the profile levels indicated in MP4 files
- decoder: fix a rounding error when channel sorting is enabled
- decoder: fixed a limitation when joint stereo coding is used together
           with block length switching
- encoder: can enforce ALS Simple Profile Level 1 ("-sp1")
- encoder: automatically set the profile level indicator in MP4 files unless
           given the "-npi" option
- encoder: force sample resolution = 32 for floating-point RAW files
- encoder: support for samples rates higher than or equal to 288kHz
- encoder: do not use the const block coding tool if the constant value
           cannot be represented on IntRes bits
- encoder: fixed vulnerability in Rice parameter estimation
- encoder: correctly set the bs_info flags when joint stereo coding and
           block length switching is used together
- encoder: do not allow shorter block length than the fixed prediction order
- use IntRes consistently in the integer coder
- fixed memory leak in ImfBox.cpp, CBox::ReadString()
- fixed memory leak in Mp4aFile.cpp, CMp4aReader::Open()
- aligned the order of shift_amp[c] and partA_flag[c] flags with the
  specification for floating-point audio
- aligned the bit endianness of MLZ codewords with the specification (MSB
  first instead of LSB first)
- aligned NeedTdBit calculation with the ALS specification
- fixed progressive coding of the residual for short frames
- added workaround for MP4 files that contain multiple zero string
  terminators in the 'hdlr' box
- added support for FreeBSD (not well tested)
---- Changes from RM23_20100305 ----
- updated support for FreeBSD
- encoder: do not allow the combination of -z# and any of the -g#, -s# or -t#
           options
- fixed stack overflows for frame lengths > 8192 in the RLS-LMS tool
- fixed integer overflows for frame lengths >= 32768 in the RLS-LMS tool

RM22 Notes (Changes from RM21)
-------------------------------
- debugged the bit location and values of js_switch in frame_data.
- debugged the limitation of 2nd and 3rd rice parameters to 15
  in case of Res<=16.
- Bug fix for fractional-bit-depth input signals (16 < x < 24bit)
- 'meta' box is debugged to be derived from FullBox.
- use ilog2_ceil(...) for the maximum prediction order calculation.
- modified process exit code.

- support for native 64-bit platforms
- added note about 32-bit "int" assumption to the readme
- better OSTYPE detection in the Makefile
- moved LPC_ADAPT symbol definition to project file/Makefile.
- lpc_adapt is disabled by default in the Makefile
- the Makefile displays some explanation when lpc_adapt is not explicitly
  specified on the command line
- renamed VC project files to include version number
- Added Visual Studio 2005 project files
- added VC9 project file
- replaced /lib/mac/lpc_adapt.o with Universal Binary
  (ppc, ppc64, i386, x86_64)
- replaced /lib/linux/lpc_adapt.o with lpc_adapt_i386.o
  and lpc_adapt_x86_64.o
- replaced /lib/win/lpc_adapt.o with object files compiled
  with each VC version (VC6, VC7, VC8, and VC9)
- added linux_i386 and linux_x86_64 targets to the Makefile

General Notes
-------------
- The ALS reference software is not optimized, particularly not in terms
  of encoder speed.
- The algorithm for an adaptive choice of the prediction order is not
  supplied as source code, but provided as an object file (Win/Linux/Mac).
- Please report problems or bugs to T. Liebchen (liebchen@nue.tu-berlin.de)
  and N. Harada (harada.noboru@lab.ntt.co.jp).

Instructions
------------
- Windows: Use Visual Studio 6 workspace file 'mp4als_vc6.dsw' or one of the
  Visual Studio solution files 'mp4als_vc7.sln', 'mp4als_vc8.sln' or
  'mp4als_vc9.sln'.
- Linux/Mac: Use 'Makefile', i.e. type 'make all'. For further instructions,
  see the help that is printed by the 'make all' command.
- If the provided Visual Studio files or makefiles are used, the object file
  for adaptive prediction order is automatically included.
- Uncomment '#define LPC_ADAPT' in encoder.cpp if you do not wish to use
  the object files on Windows.
- The "int" data type is assumed to be 32-bit. If this is not true for your
  platform, you will have to replace "int" with the appropriate 32-bit
  data type where necessary.
- In order to avoid inconsistent results, integer implementations should be
  applied to some logarithmic arithmetic calculations such as 'ceil(log2())'.
  For detailes, see the code.

Further Resources
-----------------
- The technical specification of MPEG-4 ALS is available at http://www.iso.org
  in the following document: ISO/IEC 14496-3:2005/Amd 2:2006,
  "Audio Lossless Coding (ALS), new audio profiles and BSAC extensions"
- The identical text is also available for MPEG members at http://wg11.sc29.org/
  in document N7364, "Text of 14496-3:2001/FDAM 4, Audio Lossless Coding (ALS), 
  new audio profiles and BSAC extensions", Poznan, Poland, July 2005
- The reference software also implements the corrigenda specified in document
  N8300, "ISO/IEC 14496-3:2005/AMD2:2006/Cor.2, ALS", N8300, Klagenfurt,
  Austria, July 2006, available for MPEG members at http://wg11.sc29.org/
- The reference software also implements the corrigenda specified in document
  N9493, "ISO/IEC 14496-3:2005/AMD2:2006/Cor.3, ALS", N9493, Shenzhen,
  China, October 2007, available for MPEG members at http://wg11.sc29.org/
- The reference software also implements the MP4FF box for original audio file
  information support specified in document N9497, "ISO/IEC 14496-3:2005/FDAM8,
  MP4FF box for original audio file information", N9497, Shenzhen, China,
  October 2007, available for MPEG members at http://wg11.sc29.org/
- A "Verification Report on MPEG-4 ALS" is publicly available at 
  http://www.chiariglione.org/mpeg/working_documents/mpeg-04/audio/als_vt.zip
- For latest information and updates on MPEG-4 ALS please visit 
  http://www.nue.tu-berlin.de/mp4als

-------------------------------------------------------
