#include <io/ToLPCBridge.h>
#include <Application.h>
#include "network/WebSocketReceiver.h"
#include "files/FileIOSender.h"

ToLPCBridge::ToLPCBridge() :
    super(new FileIOSender("/dev/lpc_bb_driver"), new WebSocketReceiver(Domain::Lpc))
{

}

ToLPCBridge::~ToLPCBridge()
{
}
