#include "cuboid_base.h"
#include "stickermap.h"
#include "rotationbasis.h"
#include "cocontext.h"
#include "eocontext.h"
#include "symmetry3.h"

using namespace v8;

void RegisterModule(Handle<Object> target) {
    CuboidObj::Init(target);
    StickerMapObj::Init(target);
    RotationBasisObj::Init(target);
    COContextObj::Init(target);
    EOContextObj::Init(target);
    Symmetry3::Init(target);
}

NODE_MODULE(cuboids, RegisterModule);
