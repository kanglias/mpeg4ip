/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2000, 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *              Bill May        wmay@cisco.com
 */
/*
 * video.h - contains the interface class between the codec and the video
 * display hardware.
 */
#ifndef __VIDEO_H__
#define __VIDEO_H__ 1

#include "systems.h"
//#include <type/typeapi.h>
#include <SDL.h>

#define MAX_VIDEO_BUFFERS 16

class CVideoSync {
 public:
  CVideoSync(void);
  ~CVideoSync(void);
  int initialize_video(const char *name, int x, int y);  // from sync task
  int is_video_ready(uint64_t &disptime);  // from sync task
  int64_t play_video_at(uint64_t current_time, // from sync task
		    uint64_t &play_this_at,
		    int &have_eof);
  int get_video_buffer(unsigned char **y,
		       unsigned char **u,
		       unsigned char **v);
  int filled_video_buffers(uint64_t time, uint64_t &current_time);
  int set_video_frame(const Uint8 *y,      // from codec
		      const Uint8 *u,
		      const Uint8 *v,
		      int m_pixelw_y,
		      int m_pixelw_uv,
		      uint64_t time,
		      uint64_t &current_time);
  void config (int w, int h, int frame_rate); // from codec
  void set_wait_sem (SDL_sem *p) { m_decode_sem = p; };  // from set up
  void set_sync_sem (SDL_sem *p) {m_sync_sem = p;};      // from set up
  void flush_decode_buffers(void);    // from decoder task in response to stop
  void flush_sync_buffers(void);      // from sync task in response to stop
  void play_video(void);
  void set_eof(void) { m_eof_found = 1; };
  void set_screen_size(int scaletimes2); // 1 gets 50%, 2, normal, 4, 2 times
  void do_video_resize(void); // from sync
  uint64_t get_video_msec_per_frame (void) { return m_msec_per_frame; };
 private:
  int m_eof_found;
  int m_video_bpp;
  int m_video_scale;
  unsigned int m_width, m_height;
  int m_video_initialized;
  int m_config_set;
  int m_paused;
  volatile int m_have_data;
  SDL_Surface *m_screen;
  SDL_Overlay *m_image;
  SDL_Rect m_dstrect;
  SDL_sem *m_sync_sem;
  size_t m_fill_index, m_play_index;
  int m_decode_waiting;
  volatile int m_buffer_filled[MAX_VIDEO_BUFFERS];
  Uint8 *m_y_buffer[MAX_VIDEO_BUFFERS];
  Uint8 *m_u_buffer[MAX_VIDEO_BUFFERS];
  Uint8 *m_v_buffer[MAX_VIDEO_BUFFERS];
  uint64_t m_play_this_at[MAX_VIDEO_BUFFERS];
  uint64_t m_current_time;
  SDL_sem *m_decode_sem;
  int m_dont_fill;
  size_t m_consec_skipped;
  size_t m_behind_frames;
  size_t m_total_frames;
  uint64_t m_behind_time;
  uint64_t m_behind_time_max;
  size_t m_skipped_render;
  uint64_t m_msec_per_frame;
  uint64_t m_first_frame_time;
  uint64_t m_first_frame_count;
  uint64_t m_calculated_frame_rate;
};
#endif
