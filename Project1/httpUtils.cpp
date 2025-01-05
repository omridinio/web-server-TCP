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
	size_t pos = message.find("Content-Type: ");
	if (pos != string::npos) {
		request.contentType = message.substr(pos + 14, message.find("\r\n", pos) - pos - 14);
	}
	pos = message.find("\r\n\r\n");
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
	string nameFile;
	string lang = request.queryParams["lang"];
	set <string> langSet = { "en", "fr", "he"};
	string selectedLang = langSet.find(lang) != langSet.end() ? lang : "en";
	
	nameFile = selectedLang + "\\" + request.url.substr(1);

	response = htmlToString(nameFile);
}

string createStatusLine(int statusCode) {
	string statusLine = "HTTP/1.1 ";
	switch (statusCode) {
	case 200:
		statusLine += "200 OK";
		break;
	case 201:
		statusLine += "201 Created";
		break;
	case 404:
		statusLine += "404 Not Found";
		break;
	case 500:
		statusLine += "500 Internal Server Error";
		break;
	case 409:
		statusLine += "409 Conflict";
		break;
	}

	return statusLine;
}

string createResponseHeader(int status, string fileName, string contentType,int contentLength) {
	std::ostringstream response_stream;
	response_stream << createStatusLine(status) << "\r\n"
	<< "Content-Type: " << contentType << "\r\n"
	<< "Content-Length: " << contentLength << "\r\n"
	<< "\r\n";
	return response_stream.str();
}

string createBody(string fileName, int& status) {
	string file_path = "C:\\temp\\" + fileName;
	ifstream file(file_path);
	if (!file.is_open()) {
		status = 404;
		return "<html><body><h1>404 Not Found</h1></body></html>";
	}
	status = 200;
	ostringstream content;
	content << file.rdbuf();
	return content.str();
}

string htmlToString(string fileName) {
	int statusCode;
	string body = createBody(fileName , statusCode);
	string response = createResponseHeader(statusCode ,fileName, "text/html", body.size());
	response += body;
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
	string nameFile;
	int statusCode;
	string lang = request.queryParams["lang"];
	set <string> langSet = { "en", "fr", "he" };
	string selectedLang = langSet.find(lang) != langSet.end() ? lang : "en";
	nameFile = selectedLang + "\\" + request.url.substr(1);
	string body = createBody(nameFile, statusCode);
	response = createResponseHeader(statusCode, nameFile, "text/html", body.size());
}

void doPut(Request request, string& response) {
	string nameFile;
	int statusCode;
	string lang = request.queryParams["lang"];
	set <string> langSet = { "en", "fr", "he" };
	string selectedLang = langSet.find(lang) != langSet.end() ? lang : "en";
	nameFile = selectedLang + "\\" + request.url.substr(1);
	statusCode = writeToFile(nameFile, request.body);
	response = createResponseHeader(statusCode, nameFile, request.contentType ,0);
}

int writeToFile(string fileName, string content) {
	string file_path = "C:\\temp\\" + fileName;
	int status = 201;
	ifstream infile(file_path);
	//check if file exists
	if (infile.good()) {
		//file exists so we need to update it
		status = 200;
	}
	infile.close();
	//open the file in append mode 
	ofstream file(file_path, ios::app);
	file << content;
	file.close();
	return status;
}

