/****************************************************************************/
/*   MPEG4 Visual Texture Coding (VTC) Mode Software                        */
/*                                                                          */
/*   This software was jointly developed by the following participants:     */
/*                                                                          */
/*   Single-quant,  multi-quant and flow control                            */
/*   are provided by  Sarnoff Corporation                                   */
/*     Iraj Sodagar   (iraj@sarnoff.com)                                    */
/*     Hung-Ju Lee    (hjlee@sarnoff.com)                                   */
/*     Paul Hatrack   (hatrack@sarnoff.com)                                 */
/*     Shipeng Li     (shipeng@sarnoff.com)                                 */
/*     Bing-Bing Chai (bchai@sarnoff.com)                                   */
/*     B.S. Srinivas  (bsrinivas@sarnoff.com)                               */
/*                                                                          */
/*   Bi-level is provided by Texas Instruments                              */
/*     Jie Liang      (liang@ti.com)                                        */
/*                                                                          */
/*   Shape Coding is provided by  OKI Electric Industry Co., Ltd.           */
/*     Zhixiong Wu    (sgo@hlabs.oki.co.jp)                                 */
/*     Yoshihiro Ueda (yueda@hlabs.oki.co.jp)                               */
/*     Toshifumi Kanamaru (kanamaru@hlabs.oki.co.jp)                        */
/*                                                                          */
/*   OKI, Sharp, Sarnoff, TI and Microsoft contributed to bitstream         */
/*   exchange and bug fixing.                                               */
/*                                                                          */
/*                                                                          */
/* In the course of development of the MPEG-4 standard, this software       */
/* module is an implementation of a part of one or more MPEG-4 tools as     */
/* specified by the MPEG-4 standard.                                        */
/*                                                                          */
/* The copyright of this software belongs to ISO/IEC. ISO/IEC gives use     */
/* of the MPEG-4 standard free license to use this  software module or      */
/* modifications thereof for hardware or software products claiming         */
/* conformance to the MPEG-4 standard.                                      */
/*                                                                          */
/* Those intending to use this software module in hardware or software      */
/* products are advised that use may infringe existing  patents. The        */
/* original developers of this software module and their companies, the     */
/* subsequent editors and their companies, and ISO/IEC have no liability    */
/* and ISO/IEC have no liability for use of this software module or         */
/* modification thereof in an implementation.                               */
/*                                                                          */
/* Permission is granted to MPEG members to use, copy, modify,              */
/* and distribute the software modules ( or portions thereof )              */
/* for standardization activity within ISO/IEC JTC1/SC29/WG11.              */
/*                                                                          */
/* Copyright 1995, 1996, 1997, 1998 ISO/IEC                                 */
/****************************************************************************/

/****************************************************************************/
/*     Texas Instruments Predictive Embedded Zerotree (PEZW) Image Codec    */
/*	   Developed by Jie Liang (liang@ti.com)                                */
/*													                        */ 
/*     Copyright 1996, 1997, 1998 Texas Instruments	      		            */
/****************************************************************************/

/****************************************************************************
   File name:         wvtpezw_tree_init_decode.c
   Author:            Jie Liang  (liang@ti.com)
   Functions:         functions for initialization and closing of the 
                      PEZW coder.
   Revisions:         v1.0 (10/04/98)
*****************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "wvtPEZW.hpp"
#include "PEZW_zerotree.hpp"
#include "wvtpezw_tree_codec.hpp"
#include "PEZW_functions.hpp"

/* initialize the global datastructure set up
   in wvtpezw_tree_codec.h */

void PEZW_decode_init (Int levels, Int Imgwidth, Int Imgheight)
{
	int len;
	int i,j;
	int x,y;
	int adapt=ADAPTATION_MODE;
	int contexts;
	int pos, bpos;
	int hpos_start, hpos_end;
	int vpos_start, vpos_end;
	int hsize, vsize;
	int band;
	int npix;
	int start,end;
	//int IsStream=1;
	//int Mbplane=0;
	//int NumTree=0;
    int nsym = 0, *freq;

#ifdef DEBUG_FILE
	fp_debug = fopen("zerotree_debug_decode.txt","w");
	if(DEBUG_SYMBOL)
		fprintf(fp_debug,"\n\n\n********Decoder*********\n");
#endif


    hsize = Imgwidth;
    vsize = Imgheight;

	/* define the global variables defined in 
	 wvtpezw_tree_codec.h */
	tree_depth = levels;
	MaxValue=0;

	/* positions of each depth within the tree */
    len_tree_struct = 0;
	level_pos = (short *)calloc(tree_depth,sizeof(short));
	level_pos[0]=0;
	for (i=1;i<levels;i++){
		len_tree_struct += 1<<(2*(i-1));
		level_pos[i] = len_tree_struct;
	}
	len_tree_struct += 1<<(2*(levels-1));

	/* mask */
	snr_weight = (int *)calloc(tree_depth,sizeof(int));
	bitplane = (unsigned char *)calloc(tree_depth,sizeof(char));

	/* data structure for the wavelet coefficients */
	the_wvt_tree = (WINT *)calloc(len_tree_struct,sizeof(WINT));
	abs_wvt_tree = (WINT *)calloc(len_tree_struct,sizeof(WINT));
	maskbit=(unsigned int *) calloc(tree_depth,sizeof(int));
    sign_bit = (char *) calloc(tree_depth,sizeof(int));

	/* data structure for the maximum abs value of the coeffs. */
	wvt_tree_maxval = (WINT *)calloc(len_tree_struct - (1<<(2*(levels-1))),
					   sizeof(WINT));

    /* location map for reading wavelet coefficients */
	hloc_map=(int *)calloc(len_tree_struct,sizeof(int));
	vloc_map=(int *)calloc(len_tree_struct,sizeof(int));
	hloc_map[0]=0;
	vloc_map[0]=0;
	for(i=1;i<tree_depth;i++){
			npix=1<<(2*(i-1));
			pos=level_pos[i];
			start=level_pos[i-1];
			end=level_pos[i];

			for(j=start;j<end;j++)
				{
					hpos_start=2*hloc_map[j];
					vpos_start=2*vloc_map[j];
					hpos_end=hpos_start+2;
					vpos_end=vpos_start+2;

					for(y=vpos_start;y<vpos_end;y++)
						for(x=hpos_start;x<hpos_end;x++){
							hloc_map[pos]=x;
							vloc_map[pos]=y;
							pos++;
						}
				}  /* end of j */
		}  /* end of i */


	/* scan trees */
	len=2*(len_tree_struct-(1<<(2*(levels-1))));
	ScanTrees = (short *)calloc(len,sizeof(short));
	next_ScanTrees = (short *)calloc(len,sizeof(short));

	/* significant coefficents */
	sig_pos = (short *)calloc(len_tree_struct,sizeof(short));
	sig_layer = (char *)calloc(len_tree_struct,sizeof(char));

	/* total number of significant coefficients */
	num_Sig = 0;

    /* sign bit buffer */
	sign_bit = (char *)calloc(len_tree_struct,sizeof(char));
	
	/* previous zerotree status */
	prev_label = (unsigned char*)calloc(len_tree_struct,sizeof(char));

	/* arithmetic encoder structure */
	Decoder = (Ac_decoder **)calloc(tree_depth,sizeof(Ac_decoder *));
	for(i=0;i<tree_depth;i++)
		Decoder[i]=(Ac_decoder *)calloc(Max_Bitplane, sizeof(Ac_decoder));
	
	/* set the bitstream buffer for encoder */
	/* arithmetic encoder structure */
	Decoder = (Ac_decoder **)calloc(tree_depth,sizeof(Ac_decoder *));
	for(i=0;i<tree_depth;i++)
		Decoder[i]=(Ac_decoder *)calloc(Max_Bitplane, sizeof(Ac_decoder));
	
	/* context models */
	context_model = (Ac_model *)calloc(Max_Bitplane*levels*NumContexts,sizeof(Ac_model));

	for(bpos=Max_Bitplane-1;bpos>=0;bpos--)
	  for(i=0;i<tree_depth;i++)
		for(j=0;j<NumContext_per_pixel;j++)
			for (band=0;band<NumBands;band++)
			{
                nsym = 4;
				contexts=bpos*tree_depth*NumContexts+i*NumContexts+j*NumBands+band;
				if ((i==tree_depth-1)||(j==IZER))
				  freq = freq_dom2_IZER;			  
				else
				  freq = freq_dom_ZTRZ;	

				Ac_model_init (&context_model[contexts], nsym, freq, 
							Max_frequency_TI, adapt);
			}

	model_sub  = (Ac_model *)calloc(tree_depth*MAX_BITPLANE,sizeof(Ac_model));
	model_sign = (Ac_model *)calloc(tree_depth*MAX_BITPLANE,sizeof(Ac_model));
    for(i=0;i<tree_depth*MAX_BITPLANE;i++){
		Ac_model_init (&model_sub[i],nsym,freq_dom2_IZER,	Max_frequency_TI, adapt);
		Ac_model_init (&model_sign[i],nsym,freq_dom2_IZER,	Max_frequency_TI, adapt);
	}    

    return;
}



/* end of coding processing (for each layer):
   flush the arithmetic coder  
   output the pointer to the bitstream and
   the lenght of the bitstream 
   
   bitstream and Init_Bufsize are the
   return parameters (global variables )      */

void PEZW_decode_done ()
{
    Int i,j;
    Int bpos, band, contexts;

	/* free memory */
	free(hloc_map);
	free(vloc_map);
	free(level_pos);
	free(snr_weight);
	free(bitplane);
	free(the_wvt_tree); 
	free(sign_bit);
	free(ScanTrees);
	free(next_ScanTrees);
	free(sig_pos);
	free(sig_layer);
	free(prev_label);

    for(i=0;i<tree_depth;i++)
         for(bpos=Max_Bitplane-1;bpos>=0;bpos--)
             {
                bits_to_go_inBuffer[i][bpos] = AC_decoder_buffer_adjust(&Decoder[i][bpos]);
                decoded_bytes[i][bpos] = Decoder[i][bpos].stream-PEZW_bitstream[i][bpos];
             }

    for(i=0;i<tree_depth;i++)
		free(Decoder[i]);
	free(Decoder);

	for(bpos=Max_Bitplane-1;bpos>=0;bpos--)
	  for(i=0;i<tree_depth;i++)
		for(j=0;j<NumContext_per_pixel;j++)
			for (band=0;band<NumBands;band++)
			{
				contexts=bpos*tree_depth*NumContexts+i*NumContexts+j*NumBands+band;
				AC_free_model(&context_model[contexts]);
			}

    for(i=0;i<tree_depth*MAX_BITPLANE;i++){
		AC_free_model(&model_sign[i]);
		AC_free_model(&model_sub[i]);
	}
	free(model_sign);
	free(model_sub);
	free(context_model);

#ifdef DEBUG_FILE
	fp_debug = fclose(fp_debug);
#endif
	
	return;  
}

void setbuffer_PEZW_decode ()
{
    Int bpos, i;
    //UChar adapt=1;

    /* set the bitstream buffer for decoder */
	for(bpos=Max_Bitplane-1;bpos>=Min_Bitplane;bpos--)
	  for (i=0; i<tree_depth-spatial_leveloff; i++){

	    Ac_decoder_open (&Decoder[i][bpos],PEZW_bitstream[i][bpos],1);
		Ac_decoder_init (&Decoder[i][bpos],PEZW_bitstream[i][bpos]);
	}
}

void reset_PEZW_decode ()
{
    Int bpos;
    Int i,j, band;
    Int contexts;
    Int nsym = 0;
    Int adapt=1;
    int *freq;
    unsigned char *spbuffer;

    /* AC context models */
	for(bpos=Max_Bitplane-1;bpos>=0;bpos--)
	  for(i=0;i<tree_depth;i++)
		for(j=0;j<NumContext_per_pixel;j++)
			for (band=0;band<NumBands;band++)
			{
                nsym = 4;
				contexts=bpos*tree_depth*NumContexts+i*NumContexts+j*NumBands+band;
				if ((i==tree_depth-1)||(j==IZER))
				  freq = freq_dom2_IZER;			  
				else
				  freq = freq_dom_ZTRZ;	

				AC_free_model(&context_model[contexts]);
				Ac_model_init (&context_model[contexts], nsym, freq, 
							Max_frequency_TI, adapt);
			}

        for(i=0;i<tree_depth*MAX_BITPLANE;i++){
		    AC_free_model(&model_sub[i]);
		    AC_free_model(&model_sign[i]);
		    Ac_model_init (&model_sub[i],nsym,freq_dom2_IZER,	Max_frequency_TI, adapt);
		    Ac_model_init (&model_sign[i],nsym,freq_dom2_IZER,	Max_frequency_TI, adapt);
	    }   

    /* reset AC decoder buffer */
    for(bpos=Max_Bitplane-1;bpos>=0;bpos--)
	  for(i=0;i<tree_depth;i++)
          {
             /* adjust buffer position */
             AC_decoder_buffer_adjust(&Decoder[i][bpos]);
             spbuffer = Decoder[i][bpos].stream;

             /* initizlize AC coder */
	        Ac_decoder_open (&Decoder[i][bpos],spbuffer,1);
		    Ac_decoder_init (&Decoder[i][bpos],spbuffer);           
          }

    return;
}
