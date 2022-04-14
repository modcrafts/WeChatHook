#include "pch.h"
#include<iostream>
#include<winsock.h>
#pragma comment(lib,"ws2_32.lib")
#include "json/json.h"
#include<string>
using namespace std;

bool initialization();
bool Socket_startup(const char* ipadress);
//bool Socket_send(Json::Value json);

SOCKET s_server;
bool Socket_startup(int port) {
	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[100];
	char recv_buf[100];
	//定义服务端套接字，接受请求套接字

	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	if (!initialization()) return false;
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(port);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		WSACleanup();
		return false;
	}
	else {
		return true;
	}
	/*
	//发送,接收数据
	while (1) {
		cout << "请输入发送信息:";
		cin >> send_buf;
		send_len = send(s_server, send_buf, 100, 0);
		if (send_len < 0) {
			cout << "发送失败！" << endl;
			return false;
			break;
		}
		recv_len = recv(s_server, recv_buf, 100, 0);
		if (recv_len < 0) {
			cout << "接受失败！" << endl;
			return false;
			break;
		}
		else {
			cout << "服务端信息:" << recv_buf << endl;
			return true;
		}

	}
	//关闭套接字
	closesocket(s_server);
	//释放DLL资源
	WSACleanup();
	return 0;
	*/
}
bool initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		return false;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
		return false;
	}
	return true;
	//填充服务端地址信息

}

bool Socket_send(Json::Value json) {
	std::string a = Json::FastWriter().write(json);
	const char* send_buf = a.c_str();
	int send_len = send(s_server, send_buf, 100, 0);
	if (send_len < 0) {
		return false;
	}
	else {
		return true;
	}
}

bool Socket_sendstr(char* str) {
	int send_len = send(s_server, str, 100, 0);
	if (send_len < 0) {
		return false;
	}
	else {
		return true;
	}
}