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
 * player_session.h - provides definitions for a CPlayerSession.
 * CPlayerSession is the base class that provides a combination audio/video
 * stream/file playback.
 * This class should be the main interface between any outside functionality
 * and the player window.
 */
#ifndef __PLAYER_SESSION_H__
#define __PLAYER_SESSION_H__

#include <rtsp/rtsp_client.h>
#include <sdp/sdp.h>
#include "player_media.h"
#include "audio.h"
#include "video.h"
#include "msg_queue.h"

typedef enum {
  SESSION_PAUSED,
  SESSION_BUFFERING,
  SESSION_PLAYING
} session_state_t;

class CPlayerSession {
 public:
  /*
   * API routine - create player session.
   */
  CPlayerSession(CMsgQueue *master_queue,
		 SDL_sem *master_sem,
		 const char *name);
  /*
   * API routine - destroy session - free all sub-structures, cleans
   * up rtsp, etc
   */
  ~CPlayerSession();
  /*
   * API routine - create a rtsp session with the url.  After that, you
   * need to associate media
   */
  int create_streaming(const char *url, const char **errmsg);
  /*
   * API routine - play at time.  If start_from_begin is FALSE, start_time
   * and we're paused, it will continue from where it left off.
   */
  int play_all_media(int start_from_begin = FALSE, double start_time = 0.0);
  /*
   * API routine - pause
   */
  int pause_all_media(void);
  /*
   * API routine for media set up - associate a created
   * media with the session.
   */
  void add_media(CPlayerMedia *m) {
    CPlayerMedia *p;
    if (m_my_media == NULL) {
      m_my_media = m;
    } else {
      p = m_my_media;
      while (p->get_next() != NULL) {
	if (p == m) return;
	p = p->get_next();
      }
      p->set_next(m);
    }
  };
  /*
   * API routine - returns sdp info for streamed session
   */
  session_desc_t *get_sdp_info (void) { return m_sdp_info;} ;
  rtsp_client_t *get_rtsp_client (void) { return m_rtsp_client; };
  /*
   * API routine - after setting up media, need to set up sync thread
   */
  void set_up_sync_thread(void);
  /*
   * API routine - get the current time
   */
  double get_playing_time (void) {
    double ret = (double)m_current_time;
    ret /= 1000.0;
    return (ret);
  };
  /*
   * API routine - get max play time
   */
  double get_max_time (void);
  /*
   * Other API routines
   */
  int session_has_audio(void);
  int session_has_video(void);
  void set_audio_volume(int volume);
  void set_screen_location(int x, int y);
  void set_screen_size(int scaletimes2);
  void session_set_seekable (int seekable) {
    m_seekable = seekable;
  };
  int session_is_seekable (void) {
    return (m_seekable);
  };
  session_state_t get_session_state(void) {
    return (m_session_state);
  }
  /*
   * Non-API routines - used for c interfaces, for sync task APIs.
   */
  SDL_sem *get_sync_sem(void) { return m_sync_sem; };
  int sync_thread(void);
  uint64_t get_current_time(void);
  void audio_is_ready (uint64_t latency, uint64_t time);
 private:
  const char *m_session_name;
  int m_paused;
  int m_streaming;
  uint64_t m_current_time; // current time playing
  uint64_t m_start;
  int m_clock_wrapped;
  uint64_t m_play_start_time;
  session_desc_t *m_sdp_info;
  rtsp_client_t *m_rtsp_client;
  CPlayerMedia *m_my_media;
  CAudioSync *m_audio_sync;
  CVideoSync *m_video_sync;
  SDL_Thread *m_sync_thread;
  SDL_sem *m_sync_sem;
  CMsgQueue *m_master_msg_queue;
  SDL_sem *m_master_msg_queue_sem;
  CMsgQueue m_sync_thread_msg_queue;
  range_desc_t *m_range;
  int m_waiting_for_audio;
  int m_audio_volume;
  int m_screen_scale;
  int m_seekable;
  volatile int m_sync_pause_done;
  session_state_t m_session_state;
  int m_screen_pos_x;
  int m_screen_pos_y;
  int m_hardware_error;
};

#endif
