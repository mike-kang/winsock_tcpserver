
#include <winsock2.h>
#include <ws2tcpip.h>
//#include <vector>
#include <map>

// Event value
#define EVT_CONSUCCESS      0x0000  // Connection established
#define EVT_CONFAILURE      0x0001  // General failure - Wait Connection failed
#define EVT_CONDROP         0x0002  // Connection dropped
#define EVT_ZEROLENGTH      0x0003  // Zero length message


class TcpServer {
public:
  TcpServer(int port){
	  m_port = port;
  }

  virtual ~TcpServer(){
    closesocket(m_listen_sock);
  }

  class ClientConnection {
  public:
	  ClientConnection(TcpServer* server, SOCKET sock, SOCKADDR_IN* clientaddr);
	  int send(char* buf, int length);
	  virtual int onReceive(char* buf, int size) = 0;
	  virtual void onEvent(int uEvent, void* lpvData) = 0;
    void run();
	  static DWORD WINAPI loop(LPVOID arg);
	  void _loop();

  protected:
	  SOCKET m_sock;
	  SOCKADDR_IN m_clientaddr;
	  HANDLE m_hThread;
	  TcpServer* m_server;

  };
  virtual ClientConnection* createClientConnect(SOCKET sock, SOCKADDR_IN* clientaddr) = 0;
  void sendAll(char* buf, int length);

  
  void run();
  static DWORD WINAPI loop(LPVOID arg);
  void _loop();
  virtual void onEvent(int uEvent, void* lpvData) = 0;
private:
  bool deleteClientConnect(char* ip);

protected:
  //std::vector<ClientConnection*> m_vector_connection;
  std::map<std::string, ClientConnection*> m_map_connection;
  SOCKET m_listen_sock;
  int m_port;
  HANDLE m_hThread;
};
