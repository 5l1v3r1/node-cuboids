#include "rotationbasis.h"

#define CLASS_SPECIFIER "ROTATION_BASIS_REPRESENTATION"
#define GETTHIS RotationBasisObj * thisObj = ProcessThisArgument(args);\
    if (!thisObj) return ThrowV8(Exception::TypeError, "must be called on StickerMap object");

using namespace v8;

Persistent<FunctionTemplate> RotationBasisObj::constructor;

void RotationBasisObj::Init(Handle<Object> target) {
    HandleScope scope;
    
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    Local<String> name = String::NewSymbol("RotationBasis");

    constructor = Persistent<FunctionTemplate>::New(tpl);
    constructor->InstanceTemplate()->SetInternalFieldCount(2);
    constructor->SetClassName(name);

    NODE_SET_PROTOTYPE_METHOD(constructor, "isSubset", IsSubset);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getDimensions", GetDimensions);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getPowers", GetPowers);
    NODE_SET_METHOD(constructor, "standard", Standard);
    
    // This has to be last, otherwise the properties won't show up on the
    // object in JavaScript.
    target->Set(name, constructor->GetFunction());
}

Handle<Value> RotationBasisObj::Standard(const Arguments& args) {
    HandleScope scope;
    
    if (args.Length() != 3) {
        return ThrowV8(Exception::Error, "this function takes exactly 3 arguments");
    }
    for (int i = 0; i < 3; i++) {
        if (!args[i]->IsNumber()) {
            return ThrowV8(Exception::TypeError, "arguments must be integers");
        }
    }
    uint8_t x = (uint8_t)args[0]->ToNumber()->Value();
    uint8_t y = (uint8_t)args[1]->ToNumber()->Value();
    uint8_t z = (uint8_t)args[2]->ToNumber()->Value();
    if (x < 2 || y < 2 || z < 2) {
        return ThrowV8(Exception::RangeError, "dimensions must be > 1");
    }
    CuboidDimensions dims = {x, y, z, 0};
    RotationBasis basis = rotation_basis_standard(dims);
    
    return scope.Close(CreateRotationBasis(basis));
}

RotationBasisObj::RotationBasisObj(RotationBasis aBasis) : ObjectWrap() {
    basis = aBasis;
}

Handle<Value> RotationBasisObj::New(const Arguments& args) {
    HandleScope scope;
    
    if (!args.IsConstructCall()) {
        return ThrowV8(Exception::TypeError, "must be constructor call");
    }
    
    if (args.Length() != 6) {
        return ThrowV8(Exception::Error, "this function takes exactly 6 arguments");
    }
    
    for (int i = 0; i < 6; i++) {
        if (!args[i]->IsNumber()) {
            return ThrowV8(Exception::TypeError, "arguments must be integers");
        }
    }
    
    uint8_t dimX = (uint8_t)args[0]->ToNumber()->Value();
    uint8_t dimY = (uint8_t)args[1]->ToNumber()->Value();
    uint8_t dimZ = (uint8_t)args[2]->ToNumber()->Value();
    if (dimX < 2 || dimY < 0 || dimZ < 0) {
        return ThrowV8(Exception::RangeError, "dimensions too small");
    }
    uint8_t x = (uint8_t)args[3]->ToNumber()->Value();
    uint8_t y = (uint8_t)args[4]->ToNumber()->Value();
    uint8_t z = (uint8_t)args[5]->ToNumber()->Value();
    
    if (x != 2 && x != 1 && x != 0) {
        return ThrowV8(Exception::RangeError, "invalid x power");
    } else if (y != 2 && y != 1 && y != 0) {
        return ThrowV8(Exception::RangeError, "invalid y power");
    } else if (z != 2 && z != 1 && z != 0) {
        return ThrowV8(Exception::RangeError, "invalid z power");
    }
    
    RotationBasis basis;
    basis.dims.x = dimX;
    basis.dims.y = dimY;
    basis.dims.z = dimZ;
    basis.xPower = x;
    basis.yPower = y;
    basis.zPower = z;
    RotationBasisObj * result = new RotationBasisObj(basis);
    
    args.This()->SetInternalField(1, String::NewSymbol(CLASS_SPECIFIER));
    result->Wrap(args.This());
    return args.This();
}

Handle<Value> RotationBasisObj::IsSubset(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    if (args.Length() != 1) {
        return ThrowV8(Exception::Error, "this function takes exactly 1 argument");
    }
    
    RotationBasisObj * object = CastToRotationBasis(args[0]);
    if (!object) {
        return ThrowV8(Exception::Error, "this function takes a RotationBasis argument");
    }
    
    if (!cuboid_dimensions_equal(thisObj->basis.dims, object->basis.dims)) {
        return ThrowV8(Exception::Error, "basis dimensions don't match");
    }
    
    if (rotation_basis_is_subset(thisObj->basis, object->basis)) {
        return scope.Close(True());
    } else {
        return scope.Close(False());
    }
}

Handle<Value> RotationBasisObj::GetDimensions(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    return scope.Close(dimensionsToObject(thisObj->basis.dims));
}

Handle<Value> RotationBasisObj::GetPowers(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    // a little hacky, but it's nice to be able to use dimensionsToObject
    CuboidDimensions dims = {thisObj->basis.xPower,
                             thisObj->basis.yPower,
                             thisObj->basis.zPower, 0};
    return scope.Close(dimensionsToObject(dims));
}

RotationBasisObj * RotationBasisObj::CastToRotationBasis(Handle<Value> value) {
    if (!verifySpecifier(value, CLASS_SPECIFIER)) return NULL;
    return ObjectWrap::Unwrap<RotationBasisObj>(value->ToObject());
}

Handle<Value> RotationBasisObj::CreateRotationBasis(RotationBasis basis) {
    HandleScope scope;
    
    Handle<Function> function = constructor->GetFunction();
    Handle<Value> argv[] = {
        Number::New(basis.dims.x),
        Number::New(basis.dims.y),
        Number::New(basis.dims.z),
        Number::New(basis.xPower),
        Number::New(basis.yPower),
        Number::New(basis.zPower)
    };
    
    return scope.Close(function->NewInstance(6, argv));
}

/* PROTECTED */

RotationBasisObj * RotationBasisObj::ProcessThisArgument(const Arguments& args) {
    return CastToRotationBasis(args.This());
}
