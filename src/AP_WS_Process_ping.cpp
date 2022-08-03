//
// Created by stephane bourque on 2022-07-26.
//

#include "AP_WS_Connection.h"

namespace OpenWifi {
	void AP_WS_Connection::Process_ping(Poco::JSON::Object::Ptr ParamsObj) {
		if (ParamsObj->has(uCentralProtocol::UUID)) {
			[[maybe_unused]] uint64_t UUID = ParamsObj->get(uCentralProtocol::UUID);
			poco_trace(Logger(), fmt::format("PING({}): Current config is {}", CId_, UUID));
		} else {
			poco_warning(Logger(), fmt::format("PING({}): Missing parameter.", CId_));
		}
	}
}