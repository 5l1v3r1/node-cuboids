#include "cocontext.h"

using namespace v8;

void COContextObj::Init(Handle<Object> target) {
    HandleScope scope;
    
    NODE_SET_METHOD(target, "cuboidCornerOrientation", CuboidCornerOrientation);
}

Handle<Value> COContextObj::CuboidCornerOrientation(const Arguments& args) {
    HandleScope scope;
    
    if (args.Length() != 2) {
        return ThrowV8(Exception::Error, "this function takes exactly 2 arguments");
    }
    if (!args[0]->IsNumber() || !args[1]->IsNumber()) {
        return ThrowV8(Exception::TypeError, "arguments must be integers");
    }
    
    uint8_t sym = args[0]->ToNumber()->Value();
    uint8_t axis = args[1]->ToNumber()->Value();
    if (axis >= 3) {
        return ThrowV8(Exception::RangeError, "axis must range from 0 to 3");
    }
    if (sym >= 6) {
        return ThrowV8(Exception::RangeError, "symmetry must range from 0 to 6");
    }
    
    uint8_t orientation = cuboid_corner_orientation(sym, axis);
    return scope.Close(Number::New(orientation));
}
