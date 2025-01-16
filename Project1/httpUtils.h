#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <set>
#include <fstream>


struct Request {
	string method;
	string url;
	string host;
	string contentType;
	string contentLength;
	string body;
	unordered_map<string, string> queryParams;
	string allMessage; //need this for TRACE
};

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	int sendSubType;	// Sending sub-type
	vector<string> buffer;
	int numOfMes;
	Request request;
};


void pharse(string message, Request& request);
void parseQueryParmetrs(string url, Request& request);
string htmlToString(string fileName);
void doGet(Request request, string& response);
void doTrace(Request request, string& response);
void doHead(Request request, string& response);
void doPut(Request request, string& response);
void doDelete(Request request, string& response);
void doOptions(Request request, string& response);
void doPost(Request request, string& response);

int writeToFile(string fileName, string content);