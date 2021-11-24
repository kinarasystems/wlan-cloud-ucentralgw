//
// Created by stephane bourque on 2021-11-23.
//

#pragma once

#include <array>

#include "framework/MicroService.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotifier.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/NObserver.h"

namespace OpenWifi {

class RTTY_Device_ConnectionHandler {
  public:

	enum RTTY_MSG_TYPE { msgTypeRegister = 0,
		msgTypeLogin,
		msgTypeLogout,
		msgTypeTermData,
		msgTypeWinsize,
		msgTypeCmd,
		msgTypeHeartbeat,
		msgTypeFile,
		msgTypeHttp,
		msgTypeAck,
		msgTypeMax };

	RTTY_Device_ConnectionHandler(Poco::Net::StreamSocket& socket, Poco::Net::SocketReactor & reactor);
	~RTTY_Device_ConnectionHandler();

	void onSocketReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);
	void onSocketWritable(const Poco::AutoPtr<Poco::Net::WritableNotification>& pNf);
	void onSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf);

	std::string SafeCopy( const u_char * buf, int MaxSize, int & NewPos);
	void PrintBuf(const u_char * buf, int size);
	int SendMessage( Poco::Net::StreamSocket & Socket, int Type, const u_char * Buf, int len);
	int SendMessage( Poco::Net::StreamSocket & Socket, int Type, std::string &S );
	int SendMessage( Poco::Net::StreamSocket & Socket, int Type);

  private:
	enum {
		BUF_SIZE = 8192
	};
	Poco::Net::StreamSocket       socket_;
	Poco::Net::SocketReactor&     reactor_;
	int                           proto_;
	std::string                   id_;
	std::string                   token_;
	std::string                   desc_;
	std::array<u_char,32>         sid_;
	u_char                        sid_code_;
	std::array<u_char,BUF_SIZE>   inBuf_;
};


}