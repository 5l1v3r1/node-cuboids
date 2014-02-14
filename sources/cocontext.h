#ifndef _COCONTEXT_CPP_H_
#define _COCONTEXT_CPP_H_

#include <node.h>
#include "util.h"

extern "C" {
#include "pieces/co.h"
}

namespace COContextObj {

void Init(v8::Handle<v8::Object> target);
v8::Handle<v8::Value> CuboidCornerOrientation(const v8::Arguments& args);

}

#endif
