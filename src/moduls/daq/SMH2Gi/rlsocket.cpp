/***************************************************************************
                          rlsocket.cpp  -  description
                             -------------------
    begin                : Tue Jan 02 2001
    copyright            : (C) 2001 by Rainer Lehrig
    email                : lehrig@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as        *
 *   published by the Free Software Foundation                             *
 *                                                                         *
 ***************************************************************************/
#include "rldefine.h"
#ifdef RLWIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#define  MSG_NOSIGNAL 0
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "unistd.h"
#endif

#ifdef __VMS
#define  MSG_NOSIGNAL 0
#endif

#ifdef PVMAC
#define MSG_NOSIGNAL 0
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "rlsocket.h"
#include "rlwthread.h"
#include "rlcutil.h"

/* windows stuff */
int rlwsa()
{
#ifdef RLWIN32
  static int first = 1;
  WORD    wVersionRequested;
  WSADATA wsadata;
  int     err;

  if(first == 1)
  {
    wVersionRequested = MAKEWORD(1,1);
    err = WSAStartup(wVersionRequested, &wsadata);
    if(err != 0)
    {
      printf("Startup error=%d on windows\n",err);
      exit(0);
    }
    first = 0;
  }
#endif
  return 0;
}

rlSocket::rlSocket(const char *a, int p, int act)
{
  rlwsa(); // init sockets on windows
  setAdr(a);
  port = p;
  active = act;
  s = -1;
  os = -1;
  first = 1;
}

rlSocket::rlSocket(int socket)
{
  adr[0] = '\0';
  port   = 0;
  active = 0;
  s      = socket;
  os     = s;
  first  = 0;
}

rlSocket::~rlSocket()
{
  disconnect();
  if(os != -1 && active == 0)
  {
#ifdef RLWIN32
    closesocket(os);
#else
    close(os);
#endif
  }
}

void rlSocket::setAdr(const char *a)
{
  adr[0] = '\0';
  if(a == NULL) return;
  if((strlen(a)+1) > sizeof(adr)) return;
  strcpy(adr,a);
}

void rlSocket::setPort(int p)
{
  port = p;
}

int rlSocket::getPort()
{
  return port;
}

void rlSocket::setActive(int act)
{
  active = act;
}

int rlSocket::read(void *buf, int len, int timeout)
{
  int  i,ret;
  char *cbuf;

  if(s == -1) return -1;
  if(select(timeout) == 0) return 0; // timeout

  cbuf = (char *) buf;
  i = 0;
  while(i < len)
  {
    ret = recv(s,&cbuf[i],len-i,0);
    if(ret <= 0)
    {
      disconnect();
      return -1;
    }
    i += ret;
    if(i < len)
    {
      if(select(timeout) == 0) return 0; // timeout
    }
  }

  return i;
}

int rlSocket::readStr(char *buf, int len, int timeout)
{
  int ret,i;

  if(s == -1) return -1;
  if(select(timeout) == 0) return 0; // timeout

  i = 0;
  while(1)
  {
#ifdef RLWIN32
tryagain:
#endif
    ret = recv(s,&buf[i],1,0);
    if(ret <= 0)
    {
#ifdef RLWIN32
      if(WSAEWOULDBLOCK == WSAGetLastError()) goto tryagain;
#endif
      disconnect();
      buf[i] = '\0';
      return -1;
    }
    if(buf[i] == '\n')
    {
      buf[i+1] = '\0';
      return i+1;
    }
    if(i >= len-1)
    {
      buf[i+1] = '\0';
      return i+1;
    }
    i++;
  }
}

int rlSocket::write(const void *buf, int len)
{
  int ret,bytes_left,first_byte;
  const char *cbuf;

  if(s == -1) return -1;
  cbuf = (char *) buf;
  bytes_left = len;
  first_byte = 0;

  while(bytes_left > 0)
  {
    ret = send(s,&cbuf[first_byte],bytes_left,MSG_NOSIGNAL);
    if(ret <= 0)
    {
      disconnect();
      return -1;
    }
    bytes_left -= ret;
    first_byte += ret;
  }

  return first_byte;
}

int rlSocket::connect()
{
  int option;
  int ret;
  struct sockaddr_in localAddr;
  struct sockaddr_in remoteAddr;
  struct hostent     *host;
  struct in_addr     RemoteIpAddress;

  if(port < 0)       return PORT_ERR;
  if(port >= 256*256) return PORT_ERR;
  if(active == 0 || active == 3)
  { // accept calls
    s = -1;
    if(first == 1)
    {
      // create a socket
      os = socket(AF_INET,SOCK_STREAM,0);
      if(os == -1) return SOCKET_ERR;
      // set socket options 
#ifdef __VMS
      option = 1;
      if(setsockopt(os,SOL_SOCKET,SO_KEEPALIVE,&option,sizeof(option)) < 0)
      {
        return SETSOCKOPT_ERR;
      }
#endif
      option = 1;
#ifdef RLWIN32
      setsockopt(os,SOL_SOCKET,SO_REUSEADDR,(const char *) &option,sizeof(option));
#else
      setsockopt(os,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));
#endif      
      // Bind our server to the agreed upon port number.
      memset(&localAddr,0,sizeof(localAddr));
      localAddr.sin_port = htons((short) port);
      localAddr.sin_family = AF_INET;
bind:
      ret = bind(os, (struct sockaddr *) &localAddr, sizeof(localAddr));
      if(ret == -1)
      {
        rlwthread_sleep(1000);
        goto bind;
      }
      // Prepare to accept client connections.  Allow up to 5 pending 
      // connections.                                            
      ret = listen(os, 5);
      if(ret == -1) return LISTEN_ERR;
    }
    first = 0;

    if(active == 0)
    {
        // accept connections
	s = accept(os, NULL, NULL);
        if(s == -1) return ACCEPT_ERR;
    }
    else
	s = os;

  } // end active == 0 
  else if(active == 1)
  {
    disconnect();
    //::printf("debug: adr=%s port=%d\n",adr,port);
    s = -1;
    os = socket(AF_INET,SOCK_STREAM,0);
    if(os == -1) return SOCKET_ERR;
    s = os;

    //::printf("debug: gethostbyname\n");
    // fill destblk structure 
    host = gethostbyname(adr);
    if(host == NULL)
    {
      // See if the host is specified in "dot address" form
      RemoteIpAddress.s_addr = inet_addr(adr);
      if(RemoteIpAddress.s_addr == INADDR_NONE)
      {
        s = -1;
        return INET_ADDR_ERR; // -1
      }
    }
    else
    {
       memcpy(&RemoteIpAddress,host->h_addr,host->h_length);
    }
    memset(&remoteAddr,0,sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons((short) port);
    remoteAddr.sin_addr = RemoteIpAddress;

    //::printf("debug: connect\n");
    ret = ::connect(s, (struct sockaddr *) &remoteAddr, sizeof(remoteAddr));
    //::printf("debug: connect ret=%d\n",ret);
    if(ret == -1) 
    {
      s = -1;
      return CONNECT_ERR;
    }
  }
  return s;
}

int rlSocket::disconnect()
{
  if(s != -1)
  {
#ifdef RLWIN32
    closesocket(s);
#else
    close(s);
#endif
  }
  s = -1;
  return 0;
}

int rlSocket::isConnected()
{
  if(s == -1) return 0;
  return 1;
}

int rlSocket::select(int timeout)
{
  struct timeval timout;
  fd_set wset,rset,eset;
  int    ret,maxfdp1;

  if(timeout == 0) return 1;
  /* setup sockets to read */
  maxfdp1 = s+1;
  FD_ZERO(&rset);
  FD_SET (s,&rset);
  FD_ZERO(&wset);
  FD_ZERO(&eset);
  timout.tv_sec  = timeout / 1000;
  timout.tv_usec = timeout % 1000;

  ret = ::select(maxfdp1,&rset,&wset,&eset,&timout);
  if(ret == 0) return 0; /* timeout */
  return 1;
}

int rlSocket::printf(const char *format, ...)
{
  int ret;
  char message[rl_PRINTF_LENGTH]; // should be big enough

  va_list ap;
  va_start(ap,format);
  ret = rlvsnprintf(message, rl_PRINTF_LENGTH - 1, format, ap);
  va_end(ap);
  if(ret < 0) return ret;
  return write(message,strlen(message));
}

int rlSocket::sendProcessViewBrowserButtonEvent(int id)
{
  return printf("QPushButton(%d)\n",id);
}
