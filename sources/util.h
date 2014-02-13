#ifndef _UTIL_H_
#define _UTIL_H_

#include <node.h>

extern "C" {
#include "representation/cuboid_base.h"
}

#define ThrowV8(t,m) ThrowException(t(String::New(m)))

#define Ensure2IntArgs(args) if (args.Length() != 2)\
        return ThrowV8(Exception::Error, "this function takes exactly 2 arguments");\
    if (!args[0]->IsNumber() || !args[1]->IsNumber())\
        return ThrowV8(Exception::TypeError, "arguments must be integers");\

#define Ensure1IntArg(args) if (args.Length() != 1) {\
        return ThrowV8(Exception::Error, "this function takes exactly 1 argument");\
    }\
    if (!args[0]->IsNumber()) {\
        return ThrowV8(Exception::TypeError, "argument must be an integer");\
    }

bool verifySpecifier(v8::Handle<v8::Value> object, const char * specifier);
v8::Handle<v8::Value> dimensionsToObject(CuboidDimensions dims);
v8::Handle<v8::Value> flatDimensionsToObject(int x, int y);

#endif
