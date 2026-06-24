#ifndef _response_hpp_
#define _response_hpp_

#include <string>

class Response {
public:
    Response(std::string msg);
    std::string get_data();
    
private:
    std::string data;
};

#endif