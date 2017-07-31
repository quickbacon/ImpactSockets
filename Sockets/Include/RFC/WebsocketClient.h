/**
 * Created by TekuConcept on July 27, 2017
 */

#ifndef RFC_WEBSOCKET_CLIENT_H
#define RFC_WEBSOCKET_CLIENT_H

#include "RFC/ResponseMessage.h"
#include "RFC/Websocket.h"
#include "RFC/WSURI.h"

namespace Impact {
namespace RFC6455 {
    class WebsocketClient : public Websocket {
    public:
        WebsocketClient(std::iostream &stream, WSURI uri);
        
        bool initiateHandshake();
        bool acceptHandshake();
    
    private:
        WSURI _uri_;

        std::string generateKey();
        bool responseHelper(RFC2616::ResponseMessage message);
    };
}}

#endif