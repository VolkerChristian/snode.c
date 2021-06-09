/*
 * snode.c - a slim toolkit for network communication
 * Copyright (C) 2020, 2021 Volker Christian <me@vchrist.at>
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

#include "SocketContextUpgradeFactorySelector.h"

#include "log/Logger.h"
#include "web/http/server/SocketContextUpgradeFactory.h"
#include "web/http/server/SocketContextUpgradeInterface.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <dlfcn.h>
#include <type_traits> // for add_const<>::type

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace web::http::server {

    SocketContextUpgradeFactorySelector* SocketContextUpgradeFactorySelector::socketContextUpgradeFactorySelector = nullptr;

    SocketContextUpgradeFactorySelector::SocketContextUpgradeFactorySelector() {
        if (SocketContextUpgradeFactorySelector::socketContextUpgradeFactorySelector == nullptr) {
            SocketContextUpgradeFactorySelector::socketContextUpgradeFactorySelector = this;
        }

        VLOG(0) << "hihihihih " << SocketContextUpgradeFactorySelector::socketContextUpgradeFactorySelector;
        void* handle = dlopen("/home/voc/projects/ServerVoc/build/debug/web/ws/libwebsocket.so", RTLD_LAZY | RTLD_GLOBAL);

        SocketContextUpgradeInterface* (*plugin)() = reinterpret_cast<SocketContextUpgradeInterface* (*) ()>(dlsym(handle, "plugin"));

        SocketContextUpgradeInterface* socketContextUpgradeInterface = plugin();

        SocketContextUpgradeFactorySelector::instance().registerSocketContextUpgradeFactory(socketContextUpgradeInterface->create(),
                                                                                            handle);

        delete socketContextUpgradeInterface;
    }

    SocketContextUpgradeFactorySelector::~SocketContextUpgradeFactorySelector() {
        VLOG(0) << "##############################";
        for (const auto& [name, socketContextPlugin] : serverSocketContextPlugins) {
            delete socketContextPlugin.socketContextFactory;
            if (socketContextPlugin.handle != nullptr) {
                VLOG(0) << "Delete server: " << name << " - " << socketContextPlugin.handle;
                dlclose(socketContextPlugin.handle);
            }
        }

        for (const auto& [name, socketContextPlugin] : clientSocketContextPlugins) {
            delete socketContextPlugin.socketContextFactory;
            if (socketContextPlugin.handle != nullptr) {
                dlclose(socketContextPlugin.handle);
            }
        }

        SocketContextUpgradeFactorySelector::socketContextUpgradeFactorySelector = nullptr;

        VLOG(0) << "SocketContextUpgradeFactorySelector DELETED";
    }

    SocketContextUpgradeFactorySelector& SocketContextUpgradeFactorySelector::instance() {
        return *SocketContextUpgradeFactorySelector::socketContextUpgradeFactorySelector;
    }

    void SocketContextUpgradeFactorySelector::registerSocketContextUpgradeFactory(
        web::http::server::SocketContextUpgradeFactory* socketContextUpgradeFactory) {
        VLOG(0) << "register";
        registerSocketContextUpgradeFactory(socketContextUpgradeFactory, nullptr);
    }

    void SocketContextUpgradeFactorySelector::registerSocketContextUpgradeFactory(
        web::http::server::SocketContextUpgradeFactory* socketContextUpgradeFactory, void* handle) {
        SocketContextPlugin socketContextPlugin = {.socketContextFactory = socketContextUpgradeFactory, .handle = handle};

        if (socketContextUpgradeFactory->type() == "server") {
            VLOG(0) << "server: " << socketContextUpgradeFactory->name() << " - " << handle;
            [[maybe_unused]] const auto [it, success] =
                serverSocketContextPlugins.insert({socketContextUpgradeFactory->name(), socketContextPlugin});
        } else {
            VLOG(0) << "client";
            [[maybe_unused]] const auto [it, success] =
                clientSocketContextPlugins.insert({socketContextUpgradeFactory->name(), socketContextPlugin});
        }
    }

    web::http::server::SocketContextUpgradeFactory* SocketContextUpgradeFactorySelector::select(const std::string& name) {
        web::http::server::SocketContextUpgradeFactory* socketContextFactory = nullptr;

        VLOG(0) << "Select: " << name;

        if (serverSocketContextPlugins.contains(name)) {
            socketContextFactory = serverSocketContextPlugins[name].socketContextFactory;
            VLOG(0) << "Socket Context Factory: " << socketContextFactory;
        }

        return socketContextFactory;
    }

} // namespace web::http::server