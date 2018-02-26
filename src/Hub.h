#ifndef HUB_UWS_H
#define HUB_UWS_H

#include "Group.h"
#include "Node.h"
#include <string>
#include <zlib.h>
#include <mutex>
#include <map>

namespace uWS {

struct UWS_API Hub : protected uS::Node, public Group<Role::SERVER>, public Group<Role::CLIENT> {
protected:
    struct ConnectionData {
        std::string path;
        void *user;
        Group<Role::CLIENT> *group;
    };

    z_stream inflationStream = {};
    char *inflationBuffer;
    char *inflate(char *data, size_t &length, size_t maxPayload);
    std::string dynamicInflationBuffer;
    static const int LARGE_BUFFER_SIZE = 300 * 1024;

    static void onServerAccept(uS::Socket *s);
    static void onClientConnection(uS::Socket *s, bool error);

public:
    template <bool isServer>
    Group<isServer> *createGroup(int extensionOptions = 0, unsigned int maxPayload = 16777216) {
        return new Group<isServer>(extensionOptions, maxPayload, this, nodeData);
    }

    template <bool isServer>
    Group<isServer> &getDefaultGroup() {
        return static_cast<Group<isServer> &>(*this);
    }

    bool listen(int port, uS::TLS::Context sslContext = nullptr, int options = 0, Group<Role::SERVER> *eh = nullptr);
    bool listen(const char *host, int port, uS::TLS::Context sslContext = nullptr, int options = 0, Group<Role::SERVER> *eh = nullptr);
    void connect(std::string uri, void *user = nullptr, std::map<std::string, std::string> extraHeaders = {}, int timeoutMs = 5000, Group<Role::CLIENT> *eh = nullptr);
    void upgrade(uv_os_sock_t fd, const char *secKey, SSL *ssl, const char *extensions, size_t extensionsLength, const char *subprotocol, size_t subprotocolLength, Group<Role::SERVER> *serverGroup = nullptr);

    Hub(int extensionOptions = 0, bool useDefaultLoop = false, unsigned int maxPayload = 16777216) : uS::Node(LARGE_BUFFER_SIZE, WebSocketProtocol<Role::SERVER, WebSocket<Role::SERVER>>::CONSUME_PRE_PADDING, WebSocketProtocol<Role::SERVER, WebSocket<Role::SERVER>>::CONSUME_POST_PADDING, useDefaultLoop),
                                             Group<Role::SERVER>(extensionOptions, maxPayload, this, nodeData), Group<Role::CLIENT>(0, maxPayload, this, nodeData) {
        inflateInit2(&inflationStream, -15);
        inflationBuffer = new char[LARGE_BUFFER_SIZE];

#ifdef UWS_THREADSAFE
        getLoop()->preCbData = nodeData;
        getLoop()->preCb = [](void *nodeData) {
            static_cast<uS::NodeData *>(nodeData)->asyncMutex->lock();
        };

        getLoop()->postCbData = nodeData;
        getLoop()->postCb = [](void *nodeData) {
            static_cast<uS::NodeData *>(nodeData)->asyncMutex->unlock();
        };
#endif
    }

    ~Hub() {
        inflateEnd(&inflationStream);
        delete [] inflationBuffer;
    }

    using uS::Node::run;
    using uS::Node::poll;
    using uS::Node::getLoop;
    using Group<Role::SERVER>::onConnection;
    using Group<Role::CLIENT>::onConnection;
    using Group<Role::SERVER>::onTransfer;
    using Group<Role::CLIENT>::onTransfer;
    using Group<Role::SERVER>::onMessage;
    using Group<Role::CLIENT>::onMessage;
    using Group<Role::SERVER>::onDisconnection;
    using Group<Role::CLIENT>::onDisconnection;
    using Group<Role::SERVER>::onPing;
    using Group<Role::CLIENT>::onPing;
    using Group<Role::SERVER>::onPong;
    using Group<Role::CLIENT>::onPong;
    using Group<Role::SERVER>::onError;
    using Group<Role::CLIENT>::onError;
    using Group<Role::SERVER>::onHttpRequest;
    using Group<Role::SERVER>::onHttpData;
    using Group<Role::SERVER>::onHttpConnection;
    using Group<Role::SERVER>::onHttpDisconnection;
    using Group<Role::SERVER>::onHttpUpgrade;
    using Group<Role::SERVER>::onCancelledHttpRequest;

    friend struct WebSocket<Role::SERVER>;
    friend struct WebSocket<Role::CLIENT>;
};

}

#endif // HUB_UWS_H
