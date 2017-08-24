#include "stdafx.h"
#include "CameraServer.h"

void SaveJPEG(char *pPathName, const char* pAddr, int index, char *pData, int iSize);

CameraServer::CameraServer(int port)
:TcpServer(port)
{
}

void CameraServer::captureAll()
{
	CAPTURE_REQUEST	CapRequest;
	SYSTEMTIME		SystemTime;

	GetLocalTime( &SystemTime );

	CapRequest.STX = _STX_;
	CapRequest.Command = QDIS_REQ_CAPTURE;
	CapRequest.Year = SystemTime.wYear;
	CapRequest.Month = SystemTime.wMonth;
	CapRequest.Day = SystemTime.wDay;
	CapRequest.Hour = SystemTime.wHour;
	CapRequest.Minute = SystemTime.wMinute;
	CapRequest.Second = SystemTime.wSecond;
	CapRequest.ETX = _ETX_;
	CapRequest.CS = CapRequest.Command + CapRequest.Year + CapRequest.Month + CapRequest.Day + CapRequest.Hour + CapRequest.Minute + CapRequest.Second;

	printf("captureAll\n");
	sendAll((char*)&CapRequest, sizeof(CAPTURE_REQUEST));
	
}

TcpServer::ClientConnection* CameraServer::createClientConnect(SOCKET sock, SOCKADDR_IN* clientaddr)
{
  return new CameraServerClientConnection(this, sock, clientaddr);
}

void CameraServer::onEvent(int uEvent, void* lpvData)
{
	switch( uEvent )
	{
		case EVT_CONSUCCESS:
			printf("Connection Established %s\n", (char*)lpvData);
			break;
		case EVT_CONFAILURE:
			printf("Connection Failed\r\n");
			break;
		case EVT_CONDROP:
			printf("Connection Abandonned %s\n", (char*)lpvData);
			break;
		default:
			//TRACE("Unknown Socket event\n");
			break;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CameraServer::CameraServerClientConnection::onEvent(int uEvent, void* lpvData)
{
	switch( uEvent )
	{

		case EVT_ZEROLENGTH:
			//AppendMessage( _T("Zero Length Message\r\n") );
			break;
		default:
			//TRACE("Unknown Socket event\n");
			break;
	}
}

#define HOME_PATH			"C:\\Image_Data"
int CameraServer::CameraServerClientConnection::onReceive(char* buf, int length)
{
  //printf("receive %d\n", length);
  if(length <= 0)
    return length;
  
  memcpy(m_buf + m_received_byte, buf, length);
  m_received_byte += length;
  if(!m_header){
    //find header
    for( int i = 0; i < m_received_byte - 20; i++ )
    {
      // Find Start Code
      if( m_buf[i] == _STX_ && m_buf[i+1] == QDIS_FIN_CAPTURE && m_buf[i+20] == _ETX_  )
      {
        m_start_offset = i + sizeof(CAPTURE_FINISHED);
        m_header = (CAPTURE_FINISHED *)&m_buf[i];
        //printf("[%s]filesize %d\n", m_ip.c_str(), m_header->FileSize);
        break;
      }
    }
  }
  else{
    int count = m_received_byte - m_start_offset;
    if(count >= m_header->FileSize){
	  char strFileName[128];
	  sprintf(strFileName, "%s\\%d-%d-%d_%d.%d.%d", HOME_PATH, m_header->Year, m_header->Month, m_header->Day, m_header->Hour, m_header->Minute, m_header->Second);
      //printf("save file\n");
	  SaveJPEG(strFileName, m_ip.c_str(), m_header->FileIndex, &m_buf[m_start_offset], m_header->FileSize);
      int leaved = count - m_header->FileSize;
      if(leaved > 0){
        memcpy(m_buf, &m_buf[m_start_offset + m_header->FileSize], leaved);
      }
	  m_received_byte = leaved;
	  m_header = NULL;
    }

  }
  
}

void SaveJPEG(char *pPathName, const char* pAddr, int index, char *pData, int iSize)
{
	CreateDirectory(pPathName, NULL);

	FILE *p_write_file;
	char strFileName[128];
	sprintf(strFileName, "%s\\%s - %d.jpg", pPathName, pAddr, index);

	DeleteFile(strFileName);

	p_write_file = fopen(strFileName, "wb");
	fwrite(pData, 1, iSize, p_write_file);
	fclose(p_write_file);
  printf("saved picture %s\n", strFileName);

}

