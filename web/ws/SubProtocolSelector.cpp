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

#include "SubProtocolSelector.h"

#include "log/Logger.h"
#include "web/ws/SubProtocol.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <dlfcn.h>
#include <filesystem>
#include <sstream>     // for basic_stringbuf<>::int_type, basic_st...
#include <type_traits> // for add_const<>::type

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

namespace web::ws {

    SubProtocolSelector::SubProtocolSelector(SubProtocolInterface::Role role)
        : role(role) {
    }

    void SubProtocolSelector::destroy(web::ws::SubProtocol* subProtocol) {
        if (subProtocols.contains(subProtocol->getName())) {
            web::ws::SubProtocolInterface* subProtocolInterface =
                static_cast<SubProtocolInterface*>(subProtocols.find(subProtocol->getName())->second.subProtocolInterface);

            subProtocolInterface->destroy(subProtocol);
        }
    }

    void SubProtocolSelector::loadSubProtocols(const std::string& directoryPath) {
        if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath)) {
            for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::recursive_directory_iterator(directoryPath)) {
                if (std::filesystem::is_regular_file(directoryEntry) && directoryEntry.path().extension() == ".so") {
                    loadSubProtocol(directoryEntry.path());
                } else {
                    VLOG(1) << "Not a library: Ignoring " << directoryEntry;
                }
            }
        } else {
            VLOG(1) << "Not a directory: Ignoring path: " << directoryPath;
        }
    }

    void SubProtocolSelector::loadSubProtocol(const std::string& filePath) {
        void* handle = dlopen(filePath.c_str(), RTLD_LAZY | RTLD_GLOBAL);

        if (handle != nullptr) {
            VLOG(0) << "DLOpen: success: " << filePath;

            SubProtocolInterface* (*plugin)() = reinterpret_cast<SubProtocolInterface* (*) ()>(dlsym(handle, "plugin"));

            if (plugin != nullptr) {
                SubProtocolInterface* subProtocolInterface = plugin();
                if (subProtocolInterface != nullptr) {
                    registerSubProtocol(subProtocolInterface, handle);
                } else {
                    dlclose(handle);
                }
            } else {
                VLOG(0) << "Optaining function \"plugin()\" in plugin failed: " << dlerror();
            }
        } else {
            VLOG(0) << "DLOpen: error: " << dlerror() << " - " << filePath;
        }
    }

    void SubProtocolSelector::registerSubProtocol(SubProtocolInterface* subProtocolInterface, void* handle) {
        SubProtocolPlugin subProtocolPlugin = {.subProtocolInterface = subProtocolInterface, .handle = handle};

        if (subProtocolInterface != nullptr) {
            if (subProtocolInterface->role() == role) {
                const auto [it, success] = subProtocols.insert({subProtocolInterface->name(), subProtocolPlugin});
                if (!success) {
                    VLOG(0) << "Subprotocol already existing: not using " << subProtocolInterface->name();
                    subProtocolInterface->destroy();
                    if (handle != nullptr) {
                        dlclose(handle);
                    }
                }
            } else if (handle != nullptr) {
                subProtocolInterface->destroy();
                dlclose(handle);
            }
        } else if (handle != nullptr) {
            dlclose(handle);
        }
    }

    void SubProtocolSelector::unloadSubProtocols() {
        for (const auto& [name, subProtocolPlugin] : subProtocols) {
            subProtocolPlugin.subProtocolInterface->destroy();
            if (subProtocolPlugin.handle != nullptr) {
                dlclose(subProtocolPlugin.handle);
            }
        }
    }

    SubProtocolInterface* SubProtocolSelector::selectSubProtocolInterface(const std::string& subProtocolName) {
        SubProtocolInterface* subProtocolInterface = nullptr;

        if (subProtocols.contains(subProtocolName)) {
            subProtocolInterface = subProtocols.find(subProtocolName)->second.subProtocolInterface;
        }

        return subProtocolInterface;
    }

} // namespace web::ws
