#include <node.h>
#include "util.h"

extern "C" {
#include "algebra/rotation_group.h"
}

class RotationBasisObj : public node::ObjectWrap {
public:
    static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Handle<v8::Object> target);
    static v8::Handle<v8::Value> Standard(const v8::Arguments& args);

    RotationBasisObj(RotationBasis aBasis);

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> IsSubset(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetDimensions(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetPowers(const v8::Arguments& args);
    
    static RotationBasisObj * CastToRotationBasis(v8::Handle<v8::Value> value);
    static v8::Handle<v8::Value> CreateRotationBasis(RotationBasis basis);
    
protected:
    static RotationBasisObj * ProcessThisArgument(const v8::Arguments& args);
    
    RotationBasis basis;
};
