// CameraManager.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//


#include "stdafx.h"
#include "CameraServer.h"
#define SERVERPORT 7000

int main()
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;   

  CameraServer* cs = new CameraServer(SERVERPORT);
  cs->run();

  while(1){
   getchar();
   cs->captureAll();
   
  }
	return 0;
}

