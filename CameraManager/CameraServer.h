#pragma once
#include "TcpServer.h"

#define _STX_				0x38	
#define _ETX_				0x45	

typedef enum tagQDIS_COMMAND
{
    QDIS_REQ_CAPTURE = 0x11,
    QDIS_FIN_CAPTURE = 0x12,
} QDIS_COMMAND;

typedef struct tagCAPTURE_REQUEST
{
	unsigned char	STX;
	unsigned char	Command;
	unsigned short	Year;
	unsigned short	Month;
	unsigned short	Day;
	unsigned short	Hour;
	unsigned short	Minute;
	unsigned short	Second;
	unsigned char	ETX;
	unsigned char	CS;
} CAPTURE_REQUEST;

typedef struct tagCAPTURE_FINISHED
{
	unsigned char	STX;
	unsigned char	Command;
	unsigned short	Year;
	unsigned short	Month;
	unsigned short	Day;
	unsigned short	Hour;
	unsigned short	Minute;
	unsigned short	Second;
	unsigned short	FileIndex;
	int				FileSize;
	unsigned char	ETX;
	unsigned char	CS;
} CAPTURE_FINISHED;

class CameraServer :public TcpServer
{
public:
	CameraServer(int port);

	//~CameraServer();
	
  void captureAll();
  virtual TcpServer::ClientConnection* createClientConnect(SOCKET sock, SOCKADDR_IN* clientaddr);
  virtual void onEvent(int uEvent, void* lpvData);

  class CameraServerClientConnection: public TcpServer::ClientConnection
  {
  public:
    CameraServerClientConnection(TcpServer* server, SOCKET sock, SOCKADDR_IN* clientaddr):TcpServer::ClientConnection(server, sock, clientaddr)
    {
		m_received_byte = 0;
      m_start_offset = 0;
      m_file_size = 0;
      m_header = NULL;
	  m_ip = inet_ntoa(clientaddr->sin_addr);
    }
    
	  virtual void onEvent(int uEvent, void* lpvData);
    int onReceive(char* buf, int length);
  private:
    char m_buf[1024*1024];
    int m_received_byte;
    int m_start_offset;
    int m_file_size;
    CAPTURE_FINISHED* m_header;
	std::string m_ip;
    
  };
};

