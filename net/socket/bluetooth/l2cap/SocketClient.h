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

#ifndef NET_SOCKET_BLUETOOTH_L2CAP_SOCKETCLIENT_H
#define NET_SOCKET_BLUETOOTH_L2CAP_SOCKETCLIENT_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <any>
#include <functional>
#include <map>
#include <string>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include "socket/bluetooth/l2cap/Socket.h"
#include "socket/sock_stream/legacy/SocketClient.h"

namespace net::socket::bluetooth::l2cap {

    class SocketClient : public stream::legacy::SocketClient<l2cap::Socket> {
    public:
        using stream::legacy::SocketClient<l2cap::Socket>::SocketClient;

        using SocketConnection = typename stream::legacy::SocketClient<l2cap::Socket>::SocketConnection;
        using Socket = typename SocketConnection::Socket;
        using SocketAddress = typename Socket::SocketAddress;
    };

} // namespace net::socket::bluetooth::l2cap

#endif // NET_SOCKET_BLUETOOTH_L2CAP_SOCKETCLIENT_H