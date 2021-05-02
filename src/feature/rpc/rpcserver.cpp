#include "mgba/feature/rpcserver.h"
#include "X-RPC/x_RPC.h"
#include <thread>

mCore* rpc_mCore = nullptr;
std::thread* rpcThread = nullptr;
unsigned short lastPort;
char* lastIp;
bool enabled = false;

void enableRPC(char* ip, unsigned short port) {
	enabled = true;
	lastIp = ip;
	lastPort = port;
}

void disableRPC() {
	enabled = false;
}

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
		msgpack_pack_fix_uint8(packer, (char)rpc_mCore->busRead8(rpc_mCore, i));
	}
}

void supportedGens(msgpack_object* _, msgpack_packer* packer) {
	msgpack_pack_array(packer, 3);

	msgpack_pack_char(packer, 1);
	msgpack_pack_char(packer, 2);
	msgpack_pack_char(packer, 3);
}

void startRPC() {
	if (rpcThread != nullptr || !enabled) {
		return;
	}

	rpcThread = new std::thread([]() {
		xRPC_Server_RegisterCallBack("readMemory", &readMemory);
		xRPC_Server_RegisterCallBack("supportedGens", &supportedGens);
		xRPC_Server_Start(lastPort, lastIp, 10);
		xRPC_Server_ClearCallbacks();
		rpcThread = nullptr;
	});
}

void stopRPC(bool wait) {
	xRPC_Server_Stop();

	if (wait && rpcThread != nullptr) {
		if (rpcThread->joinable()) {
			rpcThread->join();
		}
	}
}

void setCore(struct mCore* core) {
	if (rpc_mCore == nullptr) {
		startRPC();
	}
	rpc_mCore = core;
}
