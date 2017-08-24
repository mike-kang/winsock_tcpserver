// CameraManager.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//


#include "stdafx.h"
#include "CameraServer.h"
#define SERVERPORT 7000

int main()
{
	// 윈속 초기화
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

