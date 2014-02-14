#include "symmetry3.h"

using namespace v8;

void Symmetry3::Init(Handle<Object> target) {
    HandleScope scope;
    
    Handle<Object> obj = Object::New();
    NODE_SET_METHOD(obj, "compose", Compose);
    NODE_SET_METHOD(obj, "inverse", Inverse);
    target->Set(String::NewSymbol("Symmetry3"), obj);
}

Handle<Value> Symmetry3::Compose(const Arguments& args) {
    HandleScope scope;
    Ensure2IntArgs(args);
    
    uint8_t sym1 = args[0]->ToNumber()->Value();
    uint8_t sym2 = args[1]->ToNumber()->Value();
    if (sym1 >= 6 || sym2 >= 6) {
        return ThrowV8(Exception::RangeError, "symmetries must range from 0 to 5");
    }
    
    uint8_t result = (uint8_t)symmetry3_operation_compose(sym1, sym2);
    return scope.Close(Number::New(result));
}

Handle<Value> Symmetry3::Inverse(const Arguments& args) {
    HandleScope scope;
    Ensure1IntArg(args);
    
    uint8_t sym = args[0]->ToNumber()->Value();
    if (sym >= 6) {
        return ThrowV8(Exception::RangeError, "symmetry must range from 0 to 5");
    }
    
    uint8_t result = (uint8_t)symmetry3_operation_inverse(sym);
    return scope.Close(Number::New(result));
}
