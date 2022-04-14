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
	//���峤�ȱ���
	int send_len = 0;
	int recv_len = 0;
	//���巢�ͻ������ͽ��ܻ�����
	char send_buf[100];
	char recv_buf[100];
	//���������׽��֣����������׽���

	//����˵�ַ�ͻ��˵�ַ
	SOCKADDR_IN server_addr;
	if (!initialization()) return false;
	//���������Ϣ
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(port);
	//�����׽���
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		WSACleanup();
		return false;
	}
	else {
		return true;
	}
	/*
	//����,��������
	while (1) {
		cout << "�����뷢����Ϣ:";
		cin >> send_buf;
		send_len = send(s_server, send_buf, 100, 0);
		if (send_len < 0) {
			cout << "����ʧ�ܣ�" << endl;
			return false;
			break;
		}
		recv_len = recv(s_server, recv_buf, 100, 0);
		if (recv_len < 0) {
			cout << "����ʧ�ܣ�" << endl;
			return false;
			break;
		}
		else {
			cout << "�������Ϣ:" << recv_buf << endl;
			return true;
		}

	}
	//�ر��׽���
	closesocket(s_server);
	//�ͷ�DLL��Դ
	WSACleanup();
	return 0;
	*/
}
bool initialization() {
	//��ʼ���׽��ֿ�
	WORD w_req = MAKEWORD(2, 2);//�汾��
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		return false;
	}
	//���汾��
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "�׽��ֿ�汾�Ų�����" << endl;
		WSACleanup();
		return false;
	}
	return true;
	//������˵�ַ��Ϣ

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