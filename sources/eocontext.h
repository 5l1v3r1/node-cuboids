#ifndef _EOCONTEXT_CPP_H_
#define _EOCONTEXT_CPP_H_

#include <node.h>
#include "util.h"

extern "C" {
#include "pieces/eo.h"
}

namespace EOContextObj {

void Init(v8::Handle<v8::Object> target);
v8::Handle<v8::Value> CuboidEdgeOrientation(const v8::Arguments& args);

}

#endif
