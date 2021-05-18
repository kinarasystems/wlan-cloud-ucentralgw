//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include "RESTAPI_commands.h"
#include "uStorageService.h"
#include "uUtils.h"

void RESTAPI_commands::handleRequest(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response)
{
    if(!ContinueProcessing(Request,Response))
        return;

    if(!IsAuthorized(Request,Response))
        return;

    try {
        ParseParameters(Request);

        if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
            auto SerialNumber = GetParameter("serialNumber", "");
            auto StartDate = uCentral::Utils::from_RFC3339(GetParameter("startDate", ""));
            auto EndDate = uCentral::Utils::from_RFC3339(GetParameter("endDate", ""));
            auto Offset = GetParameter("offset", 0);
            auto Limit = GetParameter("limit", 100);

            std::vector<uCentral::Objects::CommandDetails> Commands;

			std::cout << "Get commands: " << SerialNumber << " " << StartDate << " " << EndDate << " " << Offset << " " << Limit << std::endl;

            uCentral::Storage::GetCommands(SerialNumber, StartDate, EndDate, Offset, Limit,
                                           Commands);

            Poco::JSON::Array ArrayObj;

            for (const auto &i : Commands) {
                Poco::JSON::Object Obj;
				i.to_json(Obj);
                ArrayObj.add(Obj);
            }

			std::cout << "Returning " << Commands.size() << " commands." << std::endl;

            Poco::JSON::Object RetObj;
            RetObj.set("commands", ArrayObj);
            ReturnObject(RetObj, Response);

            return;

        } else if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_DELETE) {
            auto SerialNumber = GetBinding("serialNumber", "");
            auto StartDate = uCentral::Utils::from_RFC3339(GetParameter("startDate", ""));
            auto EndDate = uCentral::Utils::from_RFC3339(GetParameter("endDate", ""));

            if (uCentral::Storage::DeleteCommands(SerialNumber, StartDate, EndDate))
                OK(Response);
            else
                BadRequest(Response);

            return;
        }
    }
    catch(const Poco::Exception &E)
    {
        Logger_.error(Poco::format("%s: failed with %s",std::string(__func__), E.displayText()));
    }
    BadRequest(Response);
}