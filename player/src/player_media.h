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
 * player_media.h - provides CPlayerMedia class, which defines the
 * interface to a particular media steam.
 */
#ifndef __PLAYER_MEDIA_H__
#define __PLAYER_MEDIA_H__ 1

#include <SDL.h>
#include <SDL_thread.h>
#include <sdp/sdp.h>
#include <rtsp/rtsp_client.h>
#include <rtp/rtp.h>
#include "our_bytestream.h"
#include "our_msg_queue.h"
#include "player_util.h"

class CPlayerSession;
class CAudioSync;
class CVideoSync;
class C2ConsecIpPort;
class COurInByteStream;
class CRtpByteStreamBase;

class CPlayerMedia {
 public:
  CPlayerMedia();
  ~CPlayerMedia();
  /* API routine - create - for RTP stream */
  int create_streaming(CPlayerSession *p,
		       media_desc_t *sdp_media,
		       const char **errmsg,
		       int on_demand,
		       int use_rtsp,
		       int media_number_in_session);
  /* API routine - create - where we provide the bytestream */
  int create_from_file (CPlayerSession *p, COurInByteStream *b, int is_video);
  /* API routine - play, pause */
  int do_play(double start_time_offset = 0.0);
  int do_pause(void);
  int is_video(void) { return (m_is_video); };
  double get_max_playtime(void);
  /* API routine - ip port information */
  uint16_t get_our_port (void) { return m_our_port; };
  void set_server_port (uint16_t port) { m_server_port = port; };
  uint16_t get_server_port (void) { return m_server_port; };

  media_desc_t *get_sdp_media_desc (void) { return m_media_info; };
  void set_source_addr (char *s)
    {
      if (m_source_addr != NULL) free(m_source_addr);
      m_source_addr = s;
    }
  const char *get_source_addr(void);
  CPlayerMedia *get_next (void) { return m_next; };
  void set_next (CPlayerMedia *newone) { m_next = newone; };
  int decode_thread(void);

  /* Public RTP routines  - receive thread, callback, and routines to
   * pass information from rtsp to rtp byte stream
   */
  int recv_thread(void);
  void recv_callback(struct rtp *session, rtp_event *e);
  void set_rtp_ssrc (uint32_t ssrc)
    { m_rtp_ssrc = ssrc; m_rtp_ssrc_set = TRUE;};
  void set_rtp_rtptime(uint32_t time);
  void set_rtp_rtpinfo(void) { m_rtp_rtpinfo_received = 1; };

  
  void set_video_sync(CVideoSync *p) {m_video_sync = p;};
  void set_audio_sync(CAudioSync *p) {m_audio_sync = p;};

  const video_info_t *get_video_info (void) { return m_video_info; };
  const audio_info_t *get_audio_info (void) { return m_audio_info; };
  void set_video_info (video_info_t *p) { m_video_info = p; };
  void set_audio_info (audio_info_t *p) { m_audio_info = p; };
  void set_codec_type (const char *codec) {
    m_codec_type = strdup(codec);
  };
  void set_user_data (const unsigned char *udata, int length) {
    m_user_data = udata;
    m_user_data_size = length;
  }
  rtsp_session_t *get_rtsp_session(void) { return m_rtsp_session; };
  void rtp_init_tcp(void);
  void rtp_periodic(void);
  void rtp_start(void);
  void rtp_end(void);
  int rtp_receive_packet(unsigned char interleaved, struct rtp_packet *, int len);
  int rtcp_send_packet(char *buffer, int buflen);
  int get_rtp_media_number (void) { return m_rtp_media_number_in_session; };
 private:
  int m_streaming;
  int m_is_video;
  int m_paused;
  CPlayerMedia *m_next;
  CPlayerSession *m_parent;
  media_desc_t *m_media_info;
  format_list_t *m_media_fmt;        // format currently running.
  rtsp_session_t *m_rtsp_session;
  C2ConsecIpPort *m_ports;
  in_port_t m_our_port;
  in_port_t m_server_port;
  char *m_source_addr;

  time_t m_start_time;
  int m_stream_ondemand;
  int m_sync_time_set;
  uint64_t m_sync_time_offset;
  uint32_t m_rtptime_tickpersec;
  double m_play_start_time;
  // Receive thread variables
  SDL_Thread *m_recv_thread;

  /*************************************************************************
   * RTP variables - used to pass info to the bytestream
   *************************************************************************/
  int m_rtp_ondemand;
  int m_rtp_use_rtsp;
  int m_rtp_media_number_in_session;
  int m_rtp_buffering;
  struct rtp *m_rtp_session;
  CRtpByteStreamBase *m_rtp_byte_stream;
  CMsgQueue m_rtp_msg_queue;

  rtp_packet *m_head, *m_tail; // used when determining rtp protocol
  uint32_t m_rtp_queue_len;
  
  // from rtsp...
  int m_rtp_ssrc_set;
  uint32_t m_rtp_ssrc;
  int m_rtp_rtpinfo_received;
  uint32_t m_rtp_rtptime;

  int determine_proto_from_rtp(void);
  void clear_rtp_packets(void);

  // from rtcp, for broadcast, in case we get an RTCP before we determine
  // the protocol
  uint32_t m_rtcp_ntp_frac;
  uint32_t m_rtcp_ntp_sec;
  uint32_t m_rtcp_rtp_ts;
  int m_rtcp_received;
  volatile int m_rtp_inited;

  /*************************************************************************
   * Decoder thread variables
   *************************************************************************/
  SDL_Thread *m_decode_thread;
  volatile int m_decode_thread_waiting;
  SDL_sem *m_decode_thread_sem;
  const char *m_codec_type;

  // State change variable
  CMsgQueue m_decode_msg_queue;
  // Private routines
  int process_rtsp_transport(char *transport);
  CAudioSync *m_audio_sync;
  CVideoSync *m_video_sync;
  void parse_decode_message(int &thread_stop, int &decoding);
  COurInByteStream *m_byte_stream;
  video_info_t *m_video_info;
  audio_info_t *m_audio_info;

  const unsigned char *m_user_data;
  int m_user_data_size;

};

int process_rtsp_rtpinfo(char *rtpinfo, 
			 CPlayerSession *session,
			 CPlayerMedia *media);

#ifdef _WIN32
DEFINE_MESSAGE_MACRO(media_message, "media")
#else
#define media_message(loglevel, fmt...) message(loglevel, "media", fmt)
#endif
#endif
