#include "eocontext.h"

using namespace v8;

void EOContextObj::Init(Handle<Object> target) {
    HandleScope scope;
    
    NODE_SET_METHOD(target, "edgeOrientation", CuboidEdgeOrientation);
}

Handle<Value> EOContextObj::CuboidEdgeOrientation(const Arguments& args) {
    HandleScope scope;
    
    if (args.Length() != 3) {
        return ThrowV8(Exception::Error, "this function takes exactly 2 arguments");
    }
    if (!args[1]->IsNumber() || !args[2]->IsNumber()) {
        return ThrowV8(Exception::TypeError, "arguments 2 and 3 must be integers");
    }
    if (!args[0]->IsObject()) {
        return ThrowV8(Exception::TypeError, "argument 1 must be an object");
    }
    
    uint8_t slot = args[1]->ToNumber()->Value();
    uint8_t axis = args[2]->ToNumber()->Value();
    if (axis >= 3) {
        return ThrowV8(Exception::RangeError, "axis must range from 0 to 2");
    }
    if (slot >= 12) {
        return ThrowV8(Exception::RangeError, "slot must range from 0 to 11");
    }

    Handle<Object> edge = args[0]->ToObject();
    const char * keys[] = {"symmetry", "edgeIndex", "dedgeIndex"};
    for (int i = 0; i < 3; i++) {
        if (!edge->Get(String::NewSymbol(keys[i]))->IsNumber()) {
            char message[92];
            snprintf(message, 92, "an edge must have an integer %s key", keys[i]);
            return ThrowV8(Exception::TypeError, message);
        }
    }
    uint8_t symmetry = edge->Get(String::NewSymbol("symmetry"))->ToNumber()->Value();
    uint8_t dedgeIndex = edge->Get(String::NewSymbol("dedgeIndex"))->ToNumber()->Value();
    uint8_t edgeIndex = edge->Get(String::NewSymbol("edgeIndex"))->ToNumber()->Value();
    if (symmetry >= 6 || dedgeIndex >= 12) {
        return ThrowV8(Exception::Error, "invalid CuboidEdge");
    }

    CuboidEdge edgeStruct;
    edgeStruct.symmetry = symmetry;
    edgeStruct.dedgeIndex = dedgeIndex;
    edgeStruct.edgeIndex = edgeIndex;
    
    uint8_t orientation = cuboid_edge_orientation(edgeStruct, slot, axis);
    return scope.Close(Number::New(orientation));
}
