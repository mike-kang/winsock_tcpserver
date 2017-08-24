#include "stdafx.h"

#include "TcpServer.h"
#define BUFSIZE    1024
#pragma comment(lib, "ws2_32")

using namespace std;
void err_quit(char *msg)
{
	exit(1);
}

void err_display(char *msg)
{
	printf(msg);
}


void TcpServer::run()
{
	int retval;

	// socket()
	m_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(m_listen_sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(m_port);
	retval = bind(m_listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("bind()");

	// listen()
	retval = listen(m_listen_sock, SOMAXCONN);
	if(retval == SOCKET_ERROR) err_quit("listen()");

  m_hThread = CreateThread(NULL, 0, loop, this, 0, NULL);
}

DWORD WINAPI TcpServer::loop(LPVOID arg)
{
  TcpServer* my = (TcpServer*)arg;
  my->_loop();

  return 0;
}
void TcpServer::_loop()
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE+1];
	int retval;
	printf("start server loop\n");
	while(1){
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(m_listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if(client_sock == INVALID_SOCKET){
			onEvent(EVT_CONFAILURE, NULL);
			break;
		}

		ClientConnection* cc = createClientConnect(client_sock, &clientaddr);
		onEvent(EVT_CONSUCCESS, inet_ntoa(clientaddr.sin_addr)); // connect
		m_map_connection.insert(pair<string, ClientConnection*>(inet_ntoa(clientaddr.sin_addr), cc));
		cc->run();
	}

}

/*
TcpServer::ClientConnection* TcpServer::createClientConnect(SOCKET sock, SOCKADDR_IN* clientaddr)
{
  return new ClientConnection(sock, clientaddr);
}
*/

void TcpServer::sendAll(char* buf, int length)
{
  for(map<std::string, ClientConnection*>::iterator itr = m_map_connection.begin(); itr != m_map_connection.end(); itr++){
    ClientConnection* cc = itr->second;
    cc->send(buf, length);
  }
}

bool TcpServer::deleteClientConnect(char* ip)
{
  map<string, ClientConnection*>::iterator itr = m_map_connection.find(ip);
  if(itr == m_map_connection.end())
    return false;
  delete m_map_connection[ip];
  m_map_connection.erase(ip);
  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

TcpServer::ClientConnection::ClientConnection(TcpServer* server, SOCKET sock, SOCKADDR_IN* clientaddr)
{
  m_sock = sock;
  memcpy(&m_clientaddr, clientaddr, sizeof(SOCKADDR_IN));
  m_server = server;
}

void TcpServer::ClientConnection::run()
{
  m_hThread = CreateThread(NULL, 0, loop, this, 0, NULL);
}

DWORD WINAPI TcpServer::ClientConnection::loop(LPVOID arg)
{
  TcpServer::ClientConnection* my = (TcpServer::ClientConnection*)arg;
  my->_loop();

  return 0;
}

void TcpServer::ClientConnection::_loop()
{
	int addrlen;
	char buf[BUFSIZE+1];
	int retval;

  while(1){
    // 데이터 받기
    retval = recv(m_sock, buf, BUFSIZE, 0);
    if(retval == SOCKET_ERROR){
      err_display("recv()");
      break;
    }
    else if(retval == 0)
      break;
  
    // 받은 데이터 출력
    //buf[retval] = '\0';
    //printf("[TCP/%s:%d] %s\n", inet_ntoa(m_clientaddr.sin_addr),
    //  ntohs(m_clientaddr.sin_port), buf);
    
    onReceive(buf, retval);
  }
  
  closesocket(m_sock);
  m_server->onEvent(EVT_CONDROP, inet_ntoa(m_clientaddr.sin_addr));
  m_server->deleteClientConnect(inet_ntoa(m_clientaddr.sin_addr));
  //printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
  //  inet_ntoa(m_clientaddr.sin_addr), ntohs(m_clientaddr.sin_port));
}

int TcpServer::ClientConnection::send(char* buf, int length)
{
  return ::send(m_sock, buf, length, 0);
}

/*
int TcpServer::ClientConnection::onReceive(char* buf, int length)
{
  printf("receive %d\n", length);
  return 0;
}

void TcpServer::ClientConnection::onEvent(int uEvent, void* lpvData)
{
}
*/
