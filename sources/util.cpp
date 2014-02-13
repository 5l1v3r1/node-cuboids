#include "util.h"

using namespace v8;

bool verifySpecifier(Handle<Value> value, const char * specifier) {
    HandleScope scope;
    
    if (!value->IsObject()) return false;
    Handle<Object> obj = value->ToObject();
    if (obj->InternalFieldCount() < 2) return false;
    if (!obj->GetInternalField(1)->IsString()) return false;
    
    String::AsciiValue str(obj->GetInternalField(1));
    return strcmp(*str, specifier) == 0;
}

Handle<Value> dimensionsToObject(CuboidDimensions dims) {
    HandleScope scope;
    
    Handle<Object> object = Object::New();
    Handle<Number> xVal = Number::New((double)dims.x);
    Handle<Number> yVal = Number::New((double)dims.y);
    Handle<Number> zVal = Number::New((double)dims.z);
    object->Set(String::NewSymbol("x"), xVal);
    object->Set(String::NewSymbol("y"), yVal);
    object->Set(String::NewSymbol("z"), zVal);

    return scope.Close(object);
}

v8::Handle<v8::Value> flatDimensionsToObject(int x, int y) {
    HandleScope scope;
    
    Handle<Object> object = Object::New();
    Handle<Number> xVal = Number::New((double)x);
    Handle<Number> yVal = Number::New((double)y);
    object->Set(String::NewSymbol("x"), xVal);
    object->Set(String::NewSymbol("y"), yVal);

    return scope.Close(object);
}
