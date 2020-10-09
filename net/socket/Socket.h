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

#ifndef SOCKET_H
#define SOCKET_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cerrno>
#include <functional>

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#include "Descriptor.h"
#include "socket/SocketAddress.h"

namespace net::socket {

    template <typename SocketAddressT>
    class Socket : virtual public net::Descriptor {
    public:
        using SocketAddress = SocketAddressT;

        Socket() = default;

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        ~Socket() {
            if (!dontClose()) {
                ::shutdown(getFd(), SHUT_RDWR);
            }
        }
        virtual void open(const std::function<void(int errnum)>& onError, int flags = 0) = 0;

        void bind(const SocketAddress& localAddress, const std::function<void(int errnum)>& onError) {
            socklen_t addrlen = sizeof(typename SocketAddress::SockAddr);

            int ret = ::bind(getFd(), &localAddress.getSockAddr(), addrlen);

            if (ret < 0) {
                onError(errno);
            } else {
                onError(0);
            }
        }

        const SocketAddress& getLocalAddress() const {
            return localAddress;
        }

        void setLocalAddress(const SocketAddress& localAddress) {
            this->localAddress = localAddress;
        }

    protected:
        SocketAddress localAddress{};
    };

} // namespace net::socket

#endif // SOCKET_H
