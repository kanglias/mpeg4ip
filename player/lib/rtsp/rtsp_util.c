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
 * rtsp_util.c - mixture of various utilities needed for rtsp client
 */

#ifndef _WINDOWS
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#endif
#include "rtsp_private.h"

static int rtsp_debug_level = LOG_INFO;
static error_msg_func_t rtsp_error_msg = NULL;
/*
 * Ugh - a global variable for the loglevel.
 * We probably want to enhance this to pass a function vector as well.
 */
void rtsp_set_loglevel (int loglevel)
{
  rtsp_debug_level = loglevel;
}

void rtsp_set_error_func (error_msg_func_t func)
{
  rtsp_error_msg = func;
}
/*
 * rtsp_debug()
 * Display rtsp debug information.  Probably want to hook this into
 * a rtsp_client, and enhance it to do file, console, and user routine
 * output.
 */
void rtsp_debug (int loglevel, const char *fmt, ...)
{
  //struct timeval thistime;
  va_list ap;
  //char buffer[80];

  if (loglevel <= rtsp_debug_level) {
    if (rtsp_error_msg != NULL) {
      va_start(ap, fmt);
      (rtsp_error_msg)(loglevel, "librtsp", fmt, ap);
      va_end(ap);
    }
  }
}

void free_session_info (rtsp_session_t *session)
{
  CHECK_AND_FREE(session, session);
  CHECK_AND_FREE(session, url);
  free(session);
}

/*
 * clear_decode_response()
 * Frees memory associated with that structure and clears it.
 */
void clear_decode_response (rtsp_decode_t *resp)
{
  CHECK_AND_FREE(resp, retresp);
  CHECK_AND_FREE(resp, body);
  CHECK_AND_FREE(resp, accept);
  CHECK_AND_FREE(resp, accept_encoding);
  CHECK_AND_FREE(resp, accept_language);
  CHECK_AND_FREE(resp, allow_public);
  CHECK_AND_FREE(resp, authorization);
  CHECK_AND_FREE(resp, bandwidth);
  CHECK_AND_FREE(resp, blocksize);
  CHECK_AND_FREE(resp, cache_control);
  CHECK_AND_FREE(resp, content_base);
  CHECK_AND_FREE(resp, content_encoding);
  CHECK_AND_FREE(resp, content_language);
  CHECK_AND_FREE(resp, content_location);
  CHECK_AND_FREE(resp, content_type);
  CHECK_AND_FREE(resp, cookie);
  CHECK_AND_FREE(resp, date);
  CHECK_AND_FREE(resp, expires);
  CHECK_AND_FREE(resp, from);
  CHECK_AND_FREE(resp, if_modified_since);
  CHECK_AND_FREE(resp, last_modified);
  CHECK_AND_FREE(resp, location);
  CHECK_AND_FREE(resp, proxy_authenticate);
  CHECK_AND_FREE(resp, proxy_require);
  CHECK_AND_FREE(resp, range);
  CHECK_AND_FREE(resp, referer);
  CHECK_AND_FREE(resp, require);
  CHECK_AND_FREE(resp, retry_after);
  CHECK_AND_FREE(resp, rtp_info);
  CHECK_AND_FREE(resp, scale);
  CHECK_AND_FREE(resp, server);
  CHECK_AND_FREE(resp, session);
  CHECK_AND_FREE(resp, speed);
  CHECK_AND_FREE(resp, transport);
  CHECK_AND_FREE(resp, unsupported);
  CHECK_AND_FREE(resp, user_agent);
  CHECK_AND_FREE(resp, via);
  CHECK_AND_FREE(resp, www_authenticate);
  resp->content_length = 0;
  resp->cseq = 0;
  resp->close_connection = FALSE;
}

/*
 * free_decode_response()
 * frees memory associated with response, along with response.
 */
void free_decode_response (rtsp_decode_t *decode)
{
  if (decode != NULL) {
    clear_decode_response(decode);
    free(decode);
  }
}

/*
 * free_rtsp_client()
 * frees all memory associated with rtsp client information
 */
void free_rtsp_client (rtsp_client_t *rptr)
{
  CHECK_AND_FREE(rptr, orig_url);
  CHECK_AND_FREE(rptr, url);
  CHECK_AND_FREE(rptr, server_name);
  CHECK_AND_FREE(rptr, recv_buff);
  CHECK_AND_FREE(rptr, cookie);
  free_decode_response(rptr->decode_response);
  rptr->decode_response = NULL;
  free(rptr);
#ifdef _WINDOWS
  WSACleanup();
#endif
}

/*
 * rtsp_set_and_decode_url()
 * will decode the url, make sure it matches RTSP url information,
 * pulls out the server name and port, then gets the server address
 */
static int rtsp_set_and_decode_url (const char *url, rtsp_client_t *rptr)
{
  const char *uptr;
  const char *nextslash, *nextcolon;
  size_t hostlen;
  struct hostent *host;
  
  if (rptr->url != NULL || rptr->server_name != NULL) {
    return (EEXIST);
  }

  uptr = url;
  rptr->url = strdup(url);
  if (strncmp("rtsp://", url, strlen("rtsp://")) == 0) {
    rptr->useTCP = TRUE;
    uptr += strlen("rtsp://");
#if 0
  } else if (strncmp("rtspu://", url, strlen("rtspu:")) == 0) {
    uptr += strlen("rtspu://");
#endif
  } else {
    return(-1);
  }

  nextslash = strchr(uptr, '/');
  nextcolon = strchr(uptr, ':');
  rptr->port = 554;
  if (nextslash != NULL || nextcolon != NULL) {
    if (nextcolon != NULL &&
	(nextcolon < nextslash || nextslash == NULL)) {
      hostlen = nextcolon - uptr;
      // have a port number
      nextcolon++;
      rptr->port = 0;
      while (isdigit(*nextcolon)) {
	rptr->port *= 10;
	rptr->port += *nextcolon - '0';
	nextcolon++;
      }
      if (rptr->port == 0 || (*nextcolon != '/' && *nextcolon != '\0')) {
	return (EINVAL);
      }
    } else {
      // no port number
      hostlen = nextslash - uptr;
	
    }
    if (hostlen == 0) {
      return (EINVAL);
    }
    rptr->server_name = malloc(hostlen + 1);
    if (rptr->server_name == NULL) {
      return (ENOMEM);
    }
    memcpy(rptr->server_name, uptr, hostlen);
    rptr->server_name[hostlen] = '\0';
  } else {
    if (*uptr == '\0') {
      return (EINVAL);
    }
    rptr->server_name = strdup(uptr);
    if (rptr->server_name == NULL) {
      return (ENOMEM);
    }
  }

  host = gethostbyname(rptr->server_name);
  if (host == NULL) {
    return (h_errno);
  }
  rptr->server_addr = *(struct in_addr *)host->h_addr;
  return (0);
}

/*
 * rtsp_setup_url()
 * decodes and creates/connect socket for rtsp url
 */
static int rtsp_setup_url (rtsp_client_t *info, const char *url)
{
  int err;
  err = rtsp_set_and_decode_url(url, info);
  if (err != 0) {
    rtsp_debug(LOG_ERR, "Couldn't decode url %d\n", err);
    return (err);
  }

  err = rtsp_create_socket(info);
  if (err != 0) {
    rtsp_debug(LOG_ERR,"Couldn't create socket %d\n", errno);
  }
  return (err);
}

/*
 * rtsp_create_client()
 * Creates rtsp_client_t for client information, initializes it, and
 * connects to server.
 */
rtsp_client_t *rtsp_create_client (const char *url, int *err)
{
  rtsp_client_t *info;

#ifdef _WINDOWS
	WORD wVersionRequested;
	WSADATA wsaData;
	int ret;
 
	wVersionRequested = MAKEWORD( 2, 0 );
 
	ret = WSAStartup( wVersionRequested, &wsaData );
	if ( ret != 0 ) {
	   /* Tell the user that we couldn't find a usable */
	   /* WinSock DLL.*/
		*err = ret;
	    return (NULL);
	}
#endif
  info = malloc(sizeof(rtsp_client_t));
  if (info == NULL) {
    *err = ENOMEM;
    return (NULL);
  }
  memset(info, 0, sizeof(rtsp_client_t));
  info->url = NULL;
  info->orig_url = NULL;
  info->server_name = NULL;
  info->cookie = NULL;
  info->recv_timeout = 2 * 1000;  // default timeout is 2 seconds.
  info->recv_buff_len = RECV_BUFF_DEFAULT_LEN;
  info->recv_buff = malloc(info->recv_buff_len + 1); // always room for \0
  info->server_socket = -1;
  info->next_cseq = 1;
  info->session = NULL;
  
  if (info->recv_buff == NULL) {
    *err = ENOMEM;
    free_rtsp_client(info);
    return (NULL);
  }
  info->recv_buff[info->recv_buff_len] = '\0';
  *err = rtsp_setup_url(info, url);
  if (*err != 0) {
    free_rtsp_client(info);
    return (NULL);
  }
  return (info);
}

/*
 * rtsp_setup_redirect()
 * Sets up URLs, does the connect for redirects.  Need to handle
 * 300 case (multiple choices).  Imagine that if we had that, we'd just
 * loop through the body until we found a server that we could connect
 * with.
 */
int rtsp_setup_redirect (rtsp_client_t *info)
{
  rtsp_decode_t *decode;
  if (info->decode_response == NULL)
    return (-1);

  info->redirect_count++;
  if (info->redirect_count > 5) 
    return (-1);

  decode = info->decode_response;
  if (decode->location == NULL)
    return (-1);
  
  if (info->orig_url == NULL) {
    info->orig_url = info->url;
    info->url = NULL;
  } else {
    CHECK_AND_FREE(info, url);
  }

  CHECK_AND_FREE(info, server_name);
  rtsp_close_socket(info);

  return (rtsp_setup_url(info, decode->location));
}


/* return last occurrence of needle in haystack */
static const char *my_strrstr (const char *haystack, const char *needle)
{
   size_t needle_len = strlen(needle);
   size_t haystack_len = strlen(haystack);

   haystack_len -= needle_len;
   while (haystack_len > 0) {
     if (strncmp(haystack + haystack_len, needle, needle_len) == 0) {
        return (haystack + haystack_len);
     }
     haystack_len--;
   }
   return (NULL);
}

/* 
 * removes the overlap between the base_url and the control_string
 * the base_url will never contain /trackID= x, so remove it
 * Example: rm_rtsp_overlap("rtsp://www.blah.com/foo", "foo/trackId=2")
 *          will return "rtsp://www.blah.com/foo/trackId=2"
 */
static char *rm_rtsp_overlap (const char *control_string, const char *base_url)
{
  char *str = NULL;
  size_t cblen = 0; 
  char *file_ptr = strrchr(control_string, '/'); 

  if (file_ptr != NULL && file_ptr != control_string) {
    char *path = NULL;
    char *last_path_in_base = NULL;
	size_t control_len = strlen(control_string);
	size_t file_len = strlen(file_ptr);
	size_t last_path_len = 0;

    /* path will contain control str without /trackID = x at the end */
    path = (char *)malloc(control_len - file_len + 1);
    if (path == NULL) 
      exit(EXIT_FAILURE);
    strncpy (path, control_string, control_len - file_len);
    path[control_len - file_len] = '\0';

    last_path_in_base = strdup(my_strrstr(base_url, path));
	if (last_path_in_base != NULL) 
	  last_path_len = strlen(last_path_in_base);
    if (last_path_in_base == NULL 
		|| last_path_len != strlen(path)) {
	  /* couldn't find path in base url or isn't at end of base url */
      free(path);
	  free(last_path_in_base);
      return ((char *)NULL);
    } 
	else {
	  /* make sure there is one and only one '/' between the base url
       * and the control string */
      cblen = strlen(base_url) - last_path_len;
      if (*control_string != '/') {
        cblen +=1;
      }
      str = (char *)malloc(cblen + control_len +1);
      if (str == NULL) 
        exit(EXIT_FAILURE);
      /* copy base_url up to last occurrence of path */
      strncpy(str, base_url, cblen);
      str[cblen] = '\0';
      strcat(str, control_string);
      free(path);
    }
	free(last_path_in_base);
  }
  return ((char *)str);
}


/* 
 * attempts to match the session url with  
 * content_base/url or session_name/url
 * if content base and url overlap, or if session_name and 
 * url overlap, remove the overlap and attempt to match again
 * return 1 if matched, 0 otherwise
 */
int rtsp_is_url_my_stream (rtsp_session_t *session, const char *url,
						  const char *content_base, const char *session_name)
{ 
  char *session_url = session->url;
  const char *end =  my_strrstr(session_url, url); 
  int is_match = 0;

  if (end != NULL  || strcmp(url,"*") == 0) {
	if (strncmp(session_url, content_base,
				strlen(session_url) - strlen(end)) == 0
		|| strncmp(session_url, session_name, 
				   strlen(session_url) - strlen(end)) == 0) {
	  is_match = 1;
	}
	else {
	  /* url isn't contained in the session_url */
	  /* check if there is an overlap */
	  char *str1 = rm_rtsp_overlap(url, content_base);
	  char *str2 = rm_rtsp_overlap(url, session_name);
	  if (strcmp(session_url, str1) == 0 
		  || strcmp(session_url, str2) == 0)  
		is_match = 1;
	  free(str1);
	  free(str2);
	}
  }
  return (is_match);
}

