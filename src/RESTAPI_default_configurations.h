//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#ifndef UCENTRAL_RESTAPI_DEFAULT_CONFIGURATIONS_H
#define UCENTRAL_RESTAPI_DEFAULT_CONFIGURATIONS_H

#include "RESTAPI_handler.h"

namespace uCentral {
class RESTAPI_default_configurations : public RESTAPIHandler {
  public:
	RESTAPI_default_configurations(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, bool Internal)
		: RESTAPIHandler(bindings, L,
						 std::vector<std::string>{Poco::Net::HTTPRequest::HTTP_GET,
												  Poco::Net::HTTPRequest::HTTP_OPTIONS},
						 Internal){};
	void handleRequest(Poco::Net::HTTPServerRequest &request,
					   Poco::Net::HTTPServerResponse &response) override;
	static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/default_configurations"};}
	};
}
#endif //UCENTRAL_RESTAPI_DEFAULT_CONFIGURATIONS_H
