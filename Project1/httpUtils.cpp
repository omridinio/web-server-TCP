#pragma once
#include "httpUtils.h"


void pharse(string message, Request& request)
{
	string currWord;
	istringstream stream(message);
	vector<string> words;
	while (std::getline(stream, currWord, ' ')) {
		words.push_back(currWord);
	}
	request.method = words[0];
	if (request.method == "TRACE")
		request.allMessage = message;
	istringstream secondStream(words[1]);
	string queryParams;
	if (secondStream.str().find('?') != string::npos) {
		std::getline(secondStream, request.url, '?');
		std::getline(secondStream, queryParams, '?');
		parseQueryParmetrs(queryParams, request);
	}
	else {
		request.url = words[1];
	}

	size_t pos = message.find("\r\n\r\n");
	if (pos != string::npos) {
		request.body = message.substr(pos + 4);
	}
	else {
		request.body = "";
	}
}

void parseQueryParmetrs(string url, Request& request)
{
	if (url.empty())
		request.queryParams = unordered_map<string, string>();
	istringstream stream(url);
	string currWord;
	vector<string> words;
	while (std::getline(stream, currWord, '&')) {
		request.queryParams[currWord.substr(0, currWord.find('='))] = currWord.substr(currWord.find('=') + 1);
	}
}

void doGet(Request request, string& response) {
	if (request.url != "/index.html") {
		response = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
		return;
	}
	string lang = request.queryParams["lang"];
	string nameFile = "index-";
	set <string> langSet = { "en", "fr", "he"};
	nameFile += langSet.find(lang) != langSet.end() ? lang : "en";
	nameFile += ".html";
	response = htmlToString(nameFile);
}

//string htmlToString(string fileName) {
//	string file_path = "C:\\temp\\" + fileName;
//	ifstream file(file_path);
//	if (!file.is_open()) {
//		return "<html><body><h1>404 Not Found</h1></body></html>";
//	}
//	ostringstream content;
//	content << file.rdbuf();
//	std::ostringstream response_stream;
//	response_stream << "HTTP/1.1 200 OK\r\n"
//		<< "Content-Type: text/html\r\n"
//		<< "Content-Length: " << content.str().size() << "\r\n"
//		<< "\r\n"
//		<< content.str();
//	return response_stream.str();
//}

string createStatusLine(int statusCode) {
	string statusLine = "HTTP/1.1 ";
	switch (statusCode) {
	case 200:
		statusLine += "200 OK";
		break;
	case 404:
		statusLine += "404 Not Found";
		break;
	case 500:
		statusLine += "500 Internal Server Error";
		break;
	}
	return statusLine;
}

string createResponseHeader(string fileName, string contentType) {
	string file_path = "C:\\temp\\" + fileName;
	ifstream file(file_path);
	int statusCode;
	ostringstream content;
	
	if (!file.is_open()) {
		statusCode = 404;
		content << "<html><body><h1>404 Not Found</h1></body></html>";
	}
	else {
		statusCode = 200;
		content << file.rdbuf();
	}

	std::ostringstream response_stream;
	response_stream << createStatusLine(statusCode) << "\r\n"
	<< "Content-Type: " << contentType << "\r\n"
	<< "Content-Length: " << content.str().size() << "\r\n"
	<< "\r\n";
	return response_stream.str();
}

string createBody(string fileName) {
	string file_path = "C:\\temp\\" + fileName;
	ifstream file(file_path);
	if (!file.is_open()) {
		return "<html><body><h1>404 Not Found</h1></body></html>";
	}
	ostringstream content;
	content << file.rdbuf();
	return content.str();
}

string htmlToString(string fileName) {
	string response = createResponseHeader(fileName,"text/html");
	response += createBody(fileName);
	return response;
}
void doTrace(Request request, string& response) {
	ostringstream stream;
	stream << "HTTP/1.1 200 OK\r\n"
		<< "Content-Type: message/http\r\n"
		<< "content-length: " << request.allMessage.size() << "\r\n"
		<< "\r\n"
		<< request.allMessage;
	response = stream.str();
}


void doHead(Request request, string& response) {
	if (request.url != "/index.html") {
		response = "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
		return;
	}
	string lang = request.queryParams["lang"];
	string nameFile = "index-";
	set <string> langSet = { "en", "fr", "he" };
	nameFile += langSet.find(lang) != langSet.end() ? lang : "en";
	nameFile += ".html";
	response = createResponseHeader(nameFile, "text/html");
}

