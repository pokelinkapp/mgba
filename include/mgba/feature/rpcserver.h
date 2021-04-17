#ifndef MGBA_RPCSERVER_H
#define MGBA_RPCSERVER_H
#include <mgba/core/core.h>
#if __cplusplus
extern "C" {
#endif
void startRPC(char* ip, unsigned short port);
void stopRPC();
void setCore(struct mCore* core);
#if __cplusplus
};
#endif
#endif // MGBA_RPCSERVER_H
