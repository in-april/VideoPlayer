#include <string>
#include <map>
#include <sstream>
#include <iostream>

enum Method
{
    OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN, GET_PARAMETER, RTCP,
    NONE,
};

class RtspParser {
private:
    std::map<std::string, std::string> rtsp;
    std::string format_key(std::string& str);
public:
    RtspParser(char* buf);
    ~RtspParser();
    void show();
    std::string operator[](std::string str);
};

RtspParser::RtspParser(char* msg) {
    std::string buf(msg);
    std::istringstream buf_stream(buf);
    enum parts {
        start_line,
        headers,
        body
    };
    parts part = start_line;
    std::string line;
    std::string body_string;
    while (getline(buf_stream, line)) {
        switch (part)
        {
        case start_line:
        {
            std::istringstream line_stream(line);
            std::string tmp;
            line_stream >> tmp;
            if (tmp.find("RTSP") == std::string::npos) {
                rtsp.insert(std::make_pair("method", tmp));
                line_stream >> tmp;
                rtsp.insert(std::make_pair("path", tmp));
                line_stream >> tmp;
                rtsp.insert(std::make_pair("version", tmp));
            }
            else {
                rtsp.insert(std::make_pair("version", tmp));
                line_stream >> tmp;
                rtsp.insert(std::make_pair("status", tmp));
                line_stream >> tmp;
                rtsp.insert(std::make_pair("status_text", tmp));
            }
            part = headers;
            break;
        }
        case headers:
        {
            if (line.size() == 1) {
                part = body;
                break;
            }
            auto pos = line.find(":");
            if (pos == std::string::npos)
                continue;
            std::string tmp1(line, 0, pos);
            std::string tmp2(line, pos + 2);
            rtsp.insert(std::make_pair(format_key(tmp1), tmp2));
            break;
        }
        case body:
        {
            body_string.append(line);
            body_string.push_back('\n');
            break;
        }
        default:
            break;
        }
    }
    rtsp.insert(std::make_pair("body", body_string));
}

RtspParser::~RtspParser() {}

void RtspParser::show() {
    for (auto it = rtsp.cbegin(); it != rtsp.cend(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

std::string RtspParser::operator[](std::string str) {
    auto it = rtsp.find(format_key(str));
    return it != rtsp.end() ? it->second : "";
}

std::string RtspParser::format_key(std::string& str) {

    for (char& a : str)
    {
        a = tolower(a);
    }
    return str;
}