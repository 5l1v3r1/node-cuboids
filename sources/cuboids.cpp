#include "representation.h"

using namespace v8;

void RegisterModule(Handle<Object> target) {
    CuboidObj::Init(target);
}

NODE_MODULE(cuboids, RegisterModule);