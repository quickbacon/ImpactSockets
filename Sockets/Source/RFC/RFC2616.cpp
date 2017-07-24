/**
 * Created by TekuConcept on July 22, 2017
 */

#include <map>
#include <sstream>
#include "RFC/RFC2616.h"

#include <iostream>
#define DMSG(x) std::cerr << x << std::endl

using namespace Impact;

bool RFC2616::URI::parseScheme(std::string uri, std::string &scheme) {
    std::ostringstream os;
    const char* buffer = uri.c_str();
    const char diff = 'A' - 'a';
    unsigned int idx = 0, length = uri.length();
    bool foundDelimiter = false;
    while(idx < length) {
        if(buffer[idx] == ':') {
            foundDelimiter = true;
            break;
        }
        else if(buffer[idx] >= 'A' && buffer[idx] <= 'Z') {
            // to lower
            os << (buffer[idx] - diff);
        }
        else os << buffer[idx];
        idx++;
    }
    scheme.assign(os.str());
    return foundDelimiter;
}

bool parseHostHelperIPv6(const char* buffer, unsigned int length,
    unsigned int &idx, RFC2616::URI::Info &info) {
    std::ostringstream os;
    const int MAX_HOST = 39; // IPv6 fully exapanded with ':' is 39 cahrs
    const int MAX_LABEL = 4; // labels are only 4 hex chars long
    int colonDelimiterCount = 0;

    int labelLen = 0, hostLen = 0;
    while(idx < length) {
        if(buffer[idx] == '/') return false; // didn't close with ']'
        else if(buffer[idx] == ':') {
            labelLen = 0;
            hostLen++;
            os << buffer[idx];
            
            colonDelimiterCount++;
            // IPv6 doesn't have more than 8 groups
            if (colonDelimiterCount > 7) return false;
        }
        else if(buffer[idx] == ']') {
            idx++; // align with parseHostHelper() function's return index
            // expecting "::" minimum
            if(colonDelimiterCount < 2) return false;
            else {
                info.host = os.str();
                return true;
            }
        }
        else if( // only allow legal hex values
            (buffer[idx] >= 'a' && buffer[idx] <= 'f') || // a-z
            (buffer[idx] >= 'A' && buffer[idx] <= 'F') || // A-Z
            (buffer[idx] >= '0' && buffer[idx] <= '9')
        ) {
            labelLen++;
            hostLen++;
            os << buffer[idx];
        }
        else return false;
        
        if(labelLen > MAX_LABEL || hostLen > MAX_HOST) return false;
        idx++;
    }
    
    return false;
}

bool parseHostHelper(const char* buffer, unsigned int length,
    unsigned int &idx, RFC2616::URI::Info &info) {
    std::ostringstream os;
    const int MAX_HOST = 254; // 253 + '.'
    const int MAX_LABEL = 63;
    int labelLen = 0, hostLen = 0;
    bool firstChar = true;
    
    while(idx < length) {
        if(firstChar) {
            // RFC 952: non-alphanumeric chars as the first char
            // are not allowed.
            if(buffer[idx] == '-') return false;
            else firstChar = false;
        }
        if(buffer[idx] == '/') break;
        else if(buffer[idx] == ':') break;
        else if(
            (buffer[idx] >= 'a' && buffer[idx] <= 'z') || // a-z
            (buffer[idx] >= 'A' && buffer[idx] <= 'Z') || // A-Z
            (buffer[idx] >= '0' && buffer[idx] <= '9') || // 0-9
            (buffer[idx] == '-')
        ) {
            os << buffer[idx];
            labelLen++;
            hostLen++;
        }
        else if(buffer[idx] == '.') {
            // end of label
            os << buffer[idx];
            labelLen = 0;
            hostLen++;
        }
        else return false;
        
        if(labelLen > MAX_LABEL || hostLen > MAX_HOST) return false;
        idx++;
    }
    
    info.host = os.str();
    return true;
}

bool parseHostHelperPort(const char* buffer, unsigned int length,
    unsigned int &idx, RFC2616::URI::Info &info) {
    std::ostringstream os;
    info.port = 0;
    if(idx < length && buffer[idx] == ':') {
        // parse port
        const int MAX_CHARS = 5; // "65535"
        int charCount = 0;
        idx++; // skip ':'
        while(idx < length) {
            if(buffer[idx] == '/') break;
            else if (buffer[idx] >= '0' && buffer[idx] <= '9') {
                os << buffer[idx];
                charCount++;
            }
            else return false;
            if(charCount > MAX_CHARS) return false;
            idx++;
        }
        
        std::string sport = os.str();
        if(sport.length() >= 1) {
            // only try to parse if there is something to parse
            info.port = std::stoi(sport);
            if(info.port > 65535) return false;
        }
    }
    return true;
}

bool RFC2616::URI::parse(std::string uri, Info &info) {
    if(!parseScheme(uri, info.scheme)) return false;
    
    const char *buffer = uri.c_str();
    unsigned int length = uri.length();
    if(length < 7) return false; // smallest uri: "a://b.c"
    unsigned int idx = info.scheme.length() + 3; // skip "://"
    
    if(buffer[idx] == '[') {
        // IPv6 enclosed: "[::]"
        idx++;
        if((length - idx) < 3) return false; // allow for at least [::]
        if(!parseHostHelperIPv6(buffer, length, idx, info)) return false;
    }
    else if(!parseHostHelper(buffer, length, idx, info)) return false;
    if(info.host.length() < 2) return false;
    
    // idx should either be pointing at ':' for port,
    // '/' for resource name, or the end of uri
    if(!parseHostHelperPort(buffer, length, idx, info)) return false;
    if(info.port == 0) {
        // if port is still 0, attempt to resolve port based on scheme
        if(info.scheme == "http")       info.port = 80;
        else if(info.scheme == "https") info.port = 443;
    }
    
    // store what is left into resourceName
    std::ostringstream os;
    while(idx < length) {
        if(buffer[idx] == '#') break;
        os << buffer[idx];
        idx++;
    }
    info.resourceName = os.str();
    
    return true;
}

bool RFC2616::URI::validate(std::string uri) {
    Info info;
    return parse(uri, info);
}

bool parseRequestMethod(std::string request, RFC2616::Request::Info &info) {
    switch(request.at(0)) {
        case 'C': info.method = RFC2616::Request::METHOD::CONNECT; break;
        case 'D': info.method = RFC2616::Request::METHOD::DELETE;  break;
        case 'G': info.method = RFC2616::Request::METHOD::GET;     break;
        case 'H': info.method = RFC2616::Request::METHOD::HEAD;    break;
        case 'O': info.method = RFC2616::Request::METHOD::OPTIONS; break;
        case 'P':
            if(request.at(1) == 'O')
                info.method = RFC2616::Request::METHOD::POST;
            else if(request.at(1) == 'U')
                info.method = RFC2616::Request::METHOD::PUT;
            else return false;
            break;
        case 'T': info.method = RFC2616::Request::METHOD::TRACE;   break;
        default: return false;
    }
    // double check because we only initially checked the first character(s)
    if(request.find(RFC2616::toString(info.method)) != 0)
        return false;
    return true;
}

bool parseRequestURI(const char* buffer, unsigned int length,
    unsigned int &idx, RFC2616::Request::Info &info) {
    std::ostringstream os;
    if(buffer[idx] == '*' && buffer[idx+1] != ' ') return false;
    else if(buffer[idx] != '/') return false; // full paths not yet supported
    
    while(idx < length) {
        if(buffer[idx] == ' ') break;
        else os << buffer[idx];
        idx++;
    }
    info.requestURI = os.str();
    return true;
}

bool parseRequestVersion(const char* buffer, unsigned int length,
    unsigned int &idx, RFC2616::Request::Info &info) {
    const char* version = "HTTP/";
    
    // check protocol identifier
    for(unsigned int i = 0, len = 5; i < len; i++) {
        if(version[i] != buffer[idx]) return false;
        idx++;
    }
    
    // get major and minor
    std::ostringstream os;
    while(idx < length) {
        if(buffer[idx] >= '0' && buffer[idx] <= '9')
            os << buffer[idx];
        else if (buffer[idx] == '.') {
            idx++;
            break;
        }
        else return false;
        idx++;
    }
    info.major = std::stoi(os.str());
    os.str(std::string());
    while(idx < length) {
        if(buffer[idx] >= '0' && buffer[idx] <= '9')
            os << buffer[idx];
        else if (buffer[idx] == '\r') {
            // end of line found, break
            idx++;
            break;
        }
        else if (buffer[idx] == ' ' || buffer[idx] == '\t') {
            // skip whitespace
            idx++;
            continue;
        }
        else return false;
        idx++;
    }
    info.minor = std::stoi(os.str());
    return true;
}

bool parseRequestHeader(std::string header, RFC2616::Request::Info &info) {
    if(!parseRequestMethod(header, info)) return false;

    unsigned int idx    = RFC2616::toString(info.method).length(),
                 length = header.length();
    const char*  buffer = header.c_str();
    
    while(buffer[idx] == ' ' || buffer[idx] == '\t') {
        idx++; // skip LWS
        if((length - idx) < 10) return false; // "/ HTTP/1.1"
    }
    
    if(!parseRequestURI(buffer, length, idx, info))
        return false;
    
    while(buffer[idx] == ' ' || buffer[idx] == '\t') {
        idx++; // skip LWS
        if((length - idx) < 8) return false; // "HTTP/1.1"
    }
    
    if(!parseRequestVersion(buffer, length, idx, info)) return false;
    return true;
}

bool RFC2616::Request::parse(std::istream &request, Info &info) {
    const unsigned int MIN_REQUEST_HEADER_LEN = 14; // "GET / HTTP/1.X"
    std::string requestHeader, optionalHeader;
    if (!getline(request, requestHeader)) return false;
    
    unsigned int length;
    do {
        if (!getline(request, optionalHeader)) return false;
        length = optionalHeader.length();
        if (length == 0) return false;
        else if (optionalHeader[length - 1] != '\r') return false;
        else if (length != 1)
            info.headers.push_back(optionalHeader.substr(0, length-1));
    } while(length > 1);

    if(requestHeader.length() < MIN_REQUEST_HEADER_LEN) return false;
    else if(!parseRequestHeader(requestHeader, info)) return false;
    
    // todo validate headers?
    // todo get body?
    
    return true;
}

bool RFC2616::Request::validate(std::string request) {
    Info info;
    std::stringstream ss(request);
    // validate resource uri ?
    return parse(ss, info);
}