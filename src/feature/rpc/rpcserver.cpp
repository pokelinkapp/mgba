#include "mgba/feature/rpcserver.h"
#include "X-RPC/x_RPC.h"
#include <thread>

mCore* rpc_mCore;
std::thread rpcThread;

void readMemory(msgpack_object* args, msgpack_packer* packer) {
	if (rpc_mCore == nullptr) {
		return;
	}

	if (args->type != MSGPACK_OBJECT_ARRAY) {
		return;
	}

	if (args->via.array.size < 2) {
		return;
	}

	if (args->via.array.ptr[0].type != MSGPACK_OBJECT_POSITIVE_INTEGER
	    || args->via.array.ptr[1].type != MSGPACK_OBJECT_POSITIVE_INTEGER) {
		return;
	}

	auto address = args->via.array.ptr[0].via.u64;
	auto size = args->via.array.ptr[1].via.u64;

	msgpack_pack_array(packer, size);

	for (auto i = address; i < address + size; i++) {
		msgpack_pack_char(packer, (char)rpc_mCore->busRead8(rpc_mCore, i));
	}
}

void startRPC(char* ip, unsigned short port) {
	if (xRPC_Server_GetStatus() != xRPC_SERVER_STATUS_STOPPED) {
		return;
	}

	xRPC_Server_RegisterCallBack("readMemory", &readMemory);

	rpcThread = std::thread([ip, port]() {
		xRPC_Server_Start(port, ip, 10);
		xRPC_Server_ClearCallbacks();
	});
}

void stopRPC() {
	xRPC_Server_Stop();

	if (rpcThread.joinable()) {
		rpcThread.join();
	}
}

void setCore(struct mCore* core) {
	rpc_mCore = core;
}
