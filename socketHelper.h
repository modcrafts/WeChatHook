#pragma once
#include "json/json.h"
bool Socket_startup(int port);
bool Socket_send(Json::Value json);
bool Socket_sendstr(char* str);