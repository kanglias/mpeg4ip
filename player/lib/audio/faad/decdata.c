#include "all.h"

Hcb book[NSPECBOOKS+2];
int sfbwidth128[(1<<LEN_MAX_SFBS)];

const int SampleRates[] = {
  96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000, 0
};

static int sfb_96_1024[] =
{
  4, 8, 12, 16, 20, 24, 28, 
  32, 36, 40, 44, 48, 52, 56, 
  64, 72, 80, 88, 96, 108, 120, 
  132, 144, 156, 172, 188, 212, 240, 
  276, 320, 384, 448, 512, 576, 640, 
  704, 768, 832, 896, 960, 1024
};   /* 41 scfbands */

static int sfb_96_128[] =
{
  4, 8, 12, 16, 20, 24, 32, 
  40, 48, 64, 92, 128
};   /* 12 scfbands */

static int sfb_64_1024[] =
{
  4, 8, 12, 16, 20, 24, 28, 
  32, 36, 40, 44, 48, 52, 56, 
  64, 72, 80, 88, 100, 112, 124, 
  140, 156, 172, 192, 216, 240, 268, 
  304, 344, 384, 424, 464, 504, 544, 
  584, 624, 664, 704, 744, 784, 824, 
  864, 904, 944, 984, 1024
};   /* 41 scfbands 47 */ 

static int sfb_64_128[] =
{
  4, 8, 12, 16, 20, 24, 32, 
  40, 48, 64, 92, 128
};   /* 12 scfbands */


static int sfb_48_1024[] =
{
  4, 8, 12, 16, 20, 24, 28,
  32,	36,	40,	48,	56,	64,	72,	
  80,	88,	96,	108,	120,	132,	144,	
  160,	176,	196,	216,	240,	264,	292,	
  320,	352,	384,	416,	448,	480,	512,	
  544,	576,	608,	640,	672,	704,	736,	
  768,	800,	832,	864,	896,	928,	1024
};

static int sfb_48_128[] =
{
          4,	8,	12,	16,	20,	28,	36,	
          44,	56,	68,	80,	96,	112,	128
};

static int sfb_32_1024[] =
{
        4,	8,	12,	16,	20,	24,	28,	
        32,	36,	40,	48,	56,	64,	72,	
        80,	88,	96,	108,	120,	132,	144,	
        160,	176,	196,	216,	240,	264,	292,	
        320,	352,	384,	416,	448,	480,	512,	
        544,	576,	608,	640,	672,	704,	736,	
        768,	800,	832,	864,	896,	928,	960,
        992,	1024
};

static int sfb_24_1024[] =
{
  4, 8, 12, 16, 20, 24, 28, 
  32, 36, 40, 44, 52, 60, 68, 
  76, 84, 92, 100, 108, 116, 124, 
  136, 148, 160, 172, 188, 204, 220, 
  240, 260, 284, 308, 336, 364, 396, 
  432, 468, 508, 552, 600, 652, 704, 
  768, 832, 896, 960, 1024
};   /* 47 scfbands */

static int sfb_24_128[] =
{
  4, 8, 12, 16, 20, 24, 28, 
  36, 44, 52, 64, 76, 92, 108, 
  128
};   /* 15 scfbands */

static int sfb_16_1024[] =
{
  8, 16, 24, 32, 40, 48, 56, 
  64, 72, 80, 88, 100, 112, 124, 
  136, 148, 160, 172, 184, 196, 212, 
  228, 244, 260, 280, 300, 320, 344, 
  368, 396, 424, 456, 492, 532, 572, 
  616, 664, 716, 772, 832, 896, 960, 
  1024
};   /* 43 scfbands */

static int sfb_16_128[] =
{
  4, 8, 12, 16, 20, 24, 28, 
  32, 40, 48, 60, 72, 88, 108, 
  128
};   /* 15 scfbands */

static int sfb_8_1024[] =
{
  12, 24, 36, 48, 60, 72, 84, 
  96, 108, 120, 132, 144, 156, 172, 
  188, 204, 220, 236, 252, 268, 288, 
  308, 328, 348, 372, 396, 420, 448, 
  476, 508, 544, 580, 620, 664, 712, 
  764, 820, 880, 944, 1024
};   /* 40 scfbands */

static int sfb_8_128[] =
{
  4, 8, 12, 16, 20, 24, 28, 
  36, 44, 52, 60, 72, 88, 108, 
  128
};   /* 15 scfbands */

SR_Info samp_rate_info[(1<<LEN_SAMP_IDX)] =
{
    /* sampling_frequency, #long sfb, long sfb, #short sfb, short sfb */
    /* samp_rate, nsfb1024, SFbands1024, nsfb128, SFbands128 */
    {96000, 41, sfb_96_1024, 12, sfb_96_128},	    /* 96000 */
    {88200, 41, sfb_96_1024, 12, sfb_96_128},	    /* 88200 */
    {64000, 47, sfb_64_1024, 12, sfb_64_128},	    /* 64000 */
    {48000, 49, sfb_48_1024, 14, sfb_48_128},	    /* 48000 */
    {44100, 49, sfb_48_1024, 14, sfb_48_128},	    /* 44100 */
    {32000, 51, sfb_32_1024, 14, sfb_48_128},	    /* 32000 */
    {24000, 47, sfb_24_1024, 15, sfb_24_128},	    /* 24000 */
    {22050, 47, sfb_24_1024, 15, sfb_24_128},	    /* 22050 */
    {16000, 43, sfb_16_1024, 15, sfb_16_128},	    /* 16000 */
    {12000, 43, sfb_16_1024, 15, sfb_16_128},	    /* 12000 */
    {11025, 43, sfb_16_1024, 15, sfb_16_128},	    /* 11025 */
    {8000,  40, sfb_8_1024,  15, sfb_8_128 },	    /* 8000  */
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,0,0,0}
};

int tns_max_bands_tbl[(1<<LEN_SAMP_IDX)][4] =
{
    /* entry for each sampling rate	
     * 1    Main/LC long window
     * 2    Main/LC short window
     * 3    SSR long window
     * 4    SSR short window
     */
    {31,  9, 28,  7},	    /* 96000 */
    {31,  9, 28,  7},	    /* 88200 */
    {34, 10, 27,  7},	    /* 64000 */
    {40, 14, 26,  6},	    /* 48000 */
    {42, 14, 26,  6},	    /* 44100 */
    {51, 14, 26,  6},	    /* 32000 */
    {46, 14, 29,  7},	    /* 24000 */
    {46, 14, 29,  7},	    /* 22050 */
    {42, 14, 23,  8},	    /* 16000 */
    {42, 14, 23,  8},	    /* 12000 */
    {42, 14, 23,  8},	    /* 11025 */
    {39, 14, 19,  7},	    /* 8000  */
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0}
};

int pred_max_bands_tbl[(1<<LEN_SAMP_IDX)] = {
  33,     /* 96000 */
  33,     /* 88200 */
  38,     /* 64000 */
  40,     /* 48000 */
  40,     /* 44100 */
  40,     /* 32000 */
  41,     /* 24000 */
  41,     /* 22050 */
  37,     /* 16000 */
  37,     /* 12000 */
  37,     /* 11025 */
  34,     /* 8000  */
  0,
  0,
  0,
  0
};
