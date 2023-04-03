//
// Created by stephane bourque on 2023-03-19.
//

#pragma once

#include <framework/SubSystemServer.h>
#include <Poco/Runnable.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include <Poco/JSON/Object.h>

#include "RADIUS_helpers.h"

#include <RESTObjects/RESTAPI_GWobjects.h>

namespace OpenWifi {

	class SessionNotification : public Poco::Notification {
	  public:

		enum class NotificationType {
			accounting_session_message,
			authentication_session_message,
			ap_disconnect
		};

		explicit  SessionNotification(NotificationType T, const std::string &Destination, const std::string &SerialNumber, const RADIUS::RadiusPacket &P)
			: Type_(T), Destination_(Destination), SerialNumber_(SerialNumber), Packet_(P) {
		}

		explicit SessionNotification(const std::string &SerialNumber)
			: Type_(NotificationType::ap_disconnect), SerialNumber_(SerialNumber) {

		}

		NotificationType			Type_;
		std::string 				Destination_;
		std::string 				SerialNumber_;
		RADIUS::RadiusPacket		Packet_;
	};

	using RADIUSSessionPtr = std::shared_ptr<GWObjects::RADIUSSession>;

	class RADIUSSessionTracker : public SubSystemServer, Poco::Runnable {
	  public:

		static auto instance() {
			static auto instance_ = new RADIUSSessionTracker;
			return instance_;
		}

		int Start() override;
		void Stop() override;
		void run() final;

		inline void AddAccountingSession(const std::string &Destination, const std::string &SerialNumber, const RADIUS::RadiusPacket &P) {
			SessionMessageQueue_.enqueueNotification(new SessionNotification(SessionNotification::NotificationType::accounting_session_message, Destination, SerialNumber, P));
		}

		inline void AddAuthenticationSession(const std::string &Destination, const std::string &SerialNumber, const RADIUS::RadiusPacket &P) {
			SessionMessageQueue_.enqueueNotification(new SessionNotification(SessionNotification::NotificationType::authentication_session_message, Destination, SerialNumber, P));
		}

		inline void DeviceDisconnect(const std::string &serialNumber) {
			SessionMessageQueue_.enqueueNotification(new SessionNotification(serialNumber));
		}

		inline void GetAPList(std::vector<std::string> &SerialNumbers) {
			std::lock_guard	G(Mutex_);

			for(const auto &[serialNumber,_]:AccountingSessions_) {
				SerialNumbers.emplace_back(serialNumber);
			}
		}

		inline void GetAPSessions(const std::string &SerialNumber, GWObjects::RADIUSSessionList & list) {
			std::lock_guard	G(Mutex_);

			auto ap_hint = AccountingSessions_.find(SerialNumber);
			if(ap_hint!=end(AccountingSessions_)) {
				for(const auto &[index,session]:ap_hint->second) {
					list.sessions.emplace_back(*session);
				}
			}
		}

		bool SendCoADM(const std::string &serialNumber, const std::string &sessionId);
		bool SendCoADM(const RADIUSSessionPtr &session);

		inline bool HasSessions(const std::string & serialNumber) {
			std::lock_guard	G(Mutex_);
			return AccountingSessions_.find(serialNumber)!=end(AccountingSessions_);
		}

	  private:
		std::atomic_bool 			Running_=false;
		Poco::NotificationQueue 	SessionMessageQueue_;
		Poco::Thread				QueueManager_;

		using SessionMap = std::map<std::string,RADIUSSessionPtr>;	//	calling-station-id + accounting-session-id
		std::map<std::string,SessionMap>		AccountingSessions_;				//	serial-number -> session< accounting-session -> session>

		void ProcessAccountingSession(SessionNotification &Notification);
		void ProcessAuthenticationSession(SessionNotification &Notification);
		void DisconnectSession(const std::string &SerialNumber);

		RADIUSSessionTracker() noexcept
			: SubSystemServer("RADIUSSessionTracker", "RADIUS-SESSION", "radius.session") {}
	};

	inline auto RADIUSSessionTracker() { return RADIUSSessionTracker::instance(); }

} // namespace OpenWifi

