/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020 Volker Christian <me@vchrist.at>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EXPRESS_WEBAPPT_H
#define EXPRESS_WEBAPPT_H

#include "express/Request.h"
#include "express/Response.h"
#include "express/WebApp.h"
#include "log/Logger.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <any>
#include <functional>
#include <map>
#include <string> // for string

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace express {

    template <typename ServerT>
    class WebAppT : public WebApp {
    public:
        using Server = ServerT;
        using SocketServer = typename Server::SocketServer;
        using SocketConnection = typename SocketServer::SocketConnection;
        using Socket = typename SocketConnection::Socket;
        using SocketAddress = typename Socket::SocketAddress;

        WebAppT(const std::map<std::string, std::any>& options = {{}})
            : WebAppT(Router(), options) {
        }

        WebAppT(const Router& router, const std::map<std::string, std::any>& options = {{}})
            : WebApp(router)
            , server(
                  [&onConnect = this->_onConnect](SocketConnection* socketConnection) -> void { // onConnect
                      if (onConnect) {
                          onConnect(socketConnection);
                      }
                  },
                  [routerDispatcher = this->routerDispatcher](express::Request& req,
                                                              express::Response& res) -> void { // onRequestReady
                      req.extend();
                      routerDispatcher->dispatch(req, res);
                  },
                  [this](SocketConnection* socketConnection) -> void { // onDisconnect
                      if (_onDisconnect != nullptr) {
                          _onDisconnect(socketConnection);
                      }
                  },
                  options) {
        }

        void listen(uint16_t port, const std::function<void(int err)>& onError = nullptr) override {
            server.listen(port, onError);
        }

        void listen(const std::string& host, uint16_t port, const std::function<void(int err)>& onError = nullptr) override {
            server.listen(host, port, onError);
        }

        void onConnect(const std::function<void(SocketConnection*)>& onConnect) {
            _onConnect = onConnect;
        }

        void onDisconnect(const std::function<void(SocketConnection*)>& onDisconnect) {
            _onDisconnect = onDisconnect;
        }

    protected:
        Server server;

        std::function<void(SocketConnection*)> _onConnect = nullptr;
        std::function<void(SocketConnection*)> _onDisconnect = nullptr;

    private:
        using express::WebApp::init;
        using express::WebApp::start;
        using express::WebApp::stop;
    };

} // namespace express

#endif // EXPRESS_WEBAPPT_H
