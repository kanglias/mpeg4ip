/**************************************************************************

This software module was originally developed by
Nokia in the course of development of the MPEG-2 AAC/MPEG-4 
Audio standard ISO/IEC13818-7, 14496-1, 2 and 3.
This software module is an implementation of a part
of one or more MPEG-2 AAC/MPEG-4 Audio tools as specified by the
MPEG-2 aac/MPEG-4 Audio standard. ISO/IEC  gives users of the
MPEG-2aac/MPEG-4 Audio standards free license to this software module
or modifications thereof for use in hardware or software products
claiming conformance to the MPEG-2 aac/MPEG-4 Audio  standards. Those
intending to use this software module in hardware or software products
are advised that this use may infringe existing patents. The original
developer of this software module, the subsequent
editors and their companies, and ISO/IEC have no liability for use of
this software module or modifications thereof in an
implementation. Copyright is not released for non MPEG-2 aac/MPEG-4
Audio conforming products. The original developer retains full right to
use the code for the developer's own purpose, assign or donate the code to a
third party and to inhibit third party from using the code for non
MPEG-2 aac/MPEG-4 Audio conforming products. This copyright notice
must be included in all copies or derivative works.
Copyright (c)1997.  

***************************************************************************/

#ifndef _NOK_LT_PREDICTION_H
#define _NOK_LT_PREDICTION_H

#include "block.h"
#include "nok_ltp_common.h"

#if 0
#define double_to_int(sig_in) \
  ((sig_in) > 32767 ? 32767 : (\
    (sig_in) < -32768 ? -32768 : (\
      (sig_in) > 0.0 ? (sig_in)+0.5 : (\
        (sig_in) <= 0.0 ? (sig_in)-0.5 : 0))))
#else
static inline short double_to_int (double sig_in)
{
  if (sig_in > 32767.0) return 32767;
  if (sig_in < -32768.0) return -32768;
  if (sig_in > 0.0) return (short)(sig_in + 0.5);
  if (sig_in < 0.0) return (short)(sig_in -0.5);
  return(0);
}
#endif

extern void nok_init_lt_pred (NOK_LT_PRED_STATUS * lt_status);

extern void nok_lt_predict (int profile, Info *info, WINDOW_TYPE win_type,
			    Wnd_Shape *win_shape,
			    int *sbk_prediction_used, int *sfb_prediction_used,
			    NOK_LT_PRED_STATUS * lt_status,
			    Float weight, int *delay, Float * current_frame,
			    int block_size_long, int block_size_medium,
			    int block_size_short);

extern void nok_lt_decode (WINDOW_TYPE win_type, int max_sfb,
			   int *sbk_prediction_used, int *sfb_prediction_used,
			   Float *weight, int *delay);

#endif /* not defined _NOK_LT_PREDICTION_H */
