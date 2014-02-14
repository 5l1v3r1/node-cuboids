#include <node.h>
#include "util.h"

extern "C" {
#include "representation/symmetry3.h"
}

namespace Symmetry3 {

void Init(v8::Handle<v8::Object> target);
v8::Handle<v8::Value> Compose(const v8::Arguments& args);
v8::Handle<v8::Value> Inverse(const v8::Arguments& args);

}
