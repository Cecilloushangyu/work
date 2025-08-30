/*------------------------------------------------------------------------------
* rcvraw.c : receiver raw data functions
*
*          Copyright (C) 2009-2020 by T.TAKASU, All rights reserved.
*          Copyright (C) 2014 by T.SUZUKI, All rights reserved.
*
* references :
*     [1] IS-GPS-200K, Navstar GPS Space Segment/Navigation User Interfaces,
*         March 4, 2019
*     [2] Global navigation satellite system GLONASS interface control document
*         navigation radiosignal in bands L1,L2 (version 5.1), 2008
*     [3] BeiDou satellite navigation system signal in space interface control
*         document open service signal B1I (version 3.0), February, 2019
*     [4] Quasi-Zenith Satellite System Interface Specification Satellite
*         Positioning, Navigation and Timing Service (IS-QZSS-PN-003), November
*         5, 2018
*     [5] European GNSS (Galileo) Open Service Signal In Space Interface Control
*         Document, Issue 1.3, December, 2016
*     [6] ISRO-IRNSS-ICD-SPS-1.1, Indian Regional Navigation Satellite System
*         Signal in Space ICD for Standard Positioning Service version 1.1,
*         August, 2017
*
* version : $Revision:$ $Date:$
* history : 2009/04/10 1.0  new
*           2009/06/02 1.1  support glonass
*           2010/07/31 1.2  support eph_t struct change
*           2010/12/06 1.3  add almanac decoding, support of GW10
*                           change api decode_frame()
*           2013/04/11 1.4  fix bug on decode fit interval
*           2014/01/31 1.5  fix bug on decode fit interval
*           2014/06/22 1.6  add api decode_glostr()
*           2014/06/22 1.7  add api decode_bds_d1(), decode_bds_d2()
*           2014/08/14 1.8  add test_glostr()
*                           add support input format rt17
*           2014/08/31 1.9  suppress warning
*           2014/11/07 1.10 support qzss navigation subframes
*           2016/01/23 1.11 enable septentrio
*           2016/01/28 1.12 add decode_gal_inav() for galileo I/NAV
*           2016/07/04 1.13 support CMR/CMR+
*           2017/05/26 1.14 support TERSUS
*           2018/10/10 1.15 update reference [5]
*                           add set of eph->code/flag for galileo and beidou
*           2018/12/05 1.16 add test of galileo i/nav word type 5
*           2020/11/30 1.17 add API decode_gal_fnav() and decode_irn_nav()
*                           allocate double size of raw->nav.eph[] for multiple
*                            ephemeris sets (e.g. Gallieo I/NAV and F/NAV)
*                           no support of STRFMT_LEXR by API input_raw/rawf()
*                           update references [1], [3] and [4]
*                           add reference [6]
*                           use integer types in stdint.h
*-----------------------------------------------------------------------------*/
#include "rtklib.h"

#define P2_8        0.00390625            /* 2^-8 */
#define P2_15       3.051757812500000E-05 /* 2^-15 */
#define P2_28       3.725290298461914E-09 /* 2^-28 */
#define P2_34       5.820766091346740E-11 /* 2^-34 */
#define P2_41       4.547473508864641E-13 /* 2^-41 */
#define P2_46       1.421085471520200E-14 /* 2^-46 */
#define P2_51       4.440892098500626E-16 /* 2^-51 */
#define P2_59       1.734723475976810E-18 /* 2^-59 */
#define P2_66       1.355252715606881E-20 /* 2^-66 */
#define P2_68       3.388131789017201E-21 /* 2^-68 */
#define P2P11       2048.0                /* 2^11 */
#define P2P12       4096.0                /* 2^12 */
#define P2P14       16384.0               /* 2^14 */
#define P2P15       32768.0               /* 2^15 */
#define P2P16       65536.0               /* 2^16 */

#define SQR(x)      ((x)*(x))

/* get sign-magnitude bits ---------------------------------------------------*/
static double getbitg(const uint8_t *buff, int pos, int len)
{
    double value=getbitu(buff,pos+1,len-1);
    return getbitu(buff,pos,1)?-value:value;
}
