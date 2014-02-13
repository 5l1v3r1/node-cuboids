#include "representation.h"
#include "stickermap.h"

using namespace v8;

void RegisterModule(Handle<Object> target) {
    CuboidObj::Init(target);
    StickerMapObj::Init(target);
}

NODE_MODULE(cuboids, RegisterModule);