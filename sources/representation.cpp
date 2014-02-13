#include "representation.h"

#define CLASS_SPECIFIER "CUBOID_REPRESENTATION"

// these are really nice because they return *for* you
#define GETTHIS CuboidObj * thisObj = ProcessThisArgument(args);\
    if (!thisObj) return ThrowV8(Exception::TypeError, "must be called on Cuboid object");

#define ENSURE_2_INT_ARGS if (args.Length() != 2)\
        return ThrowV8(Exception::Error, "this function takes exactly 2 arguments");\
    if (!args[0]->IsNumber() || !args[1]->IsNumber())\
        return ThrowV8(Exception::TypeError, "arguments must be integers");\

#define ENSURE_1_INT_ARG if (args.Length() != 1) {\
        return ThrowV8(Exception::Error, "this function takes exactly 1 argument");\
    }\
    if (!args[0]->IsNumber()) {\
        return ThrowV8(Exception::TypeError, "argument must be an integer");\
    }

using namespace v8;

Persistent<FunctionTemplate> CuboidObj::constructor;

void CuboidObj::Init(Handle<Object> target) {
    HandleScope scope;
    
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    Local<String> name = String::NewSymbol("Cuboid");

    constructor = Persistent<FunctionTemplate>::New(tpl);
    // ObjectWrap uses the first internal field to store the wrapped pointer.
    constructor->InstanceTemplate()->SetInternalFieldCount(2);
    constructor->SetClassName(name);

    // operations returning new CuboidObj instances
    NODE_SET_PROTOTYPE_METHOD(constructor, "multiply", Multiply);
    
    // retrieving indices and counts
    NODE_SET_PROTOTYPE_METHOD(constructor, "edgeIndex", EdgeIndex);
    NODE_SET_PROTOTYPE_METHOD(constructor, "centerIndex", CenterIndex);
    NODE_SET_PROTOTYPE_METHOD(constructor, "countEdgesForDedge", CountEdgesForDedge);
    NODE_SET_PROTOTYPE_METHOD(constructor, "countEdges", CountEdges);
    NODE_SET_PROTOTYPE_METHOD(constructor, "countCentersForFace", CountCentersForFace);
    NODE_SET_PROTOTYPE_METHOD(constructor, "countCenters", CountCenters);
    
    // retrieving piece information
    NODE_SET_PROTOTYPE_METHOD(constructor, "getCenter", GetCenter);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getEdge", GetEdge);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getCorner", GetCorner);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getDimensions", GetDimensions);

    // This has to be last, otherwise the properties won't show up on the
    // object in JavaScript.
    target->Set(name, constructor->GetFunction());
}

CuboidObj::CuboidObj(Cuboid * object) : ObjectWrap() {
    cuboidData = object;
}

CuboidObj::~CuboidObj() {
    cuboid_free(cuboidData);
}

Handle<Value> CuboidObj::New(const Arguments& args) {
    HandleScope scope;
    if (!args.IsConstructCall()) {
        return ThrowV8(Exception::TypeError, "must be constructor call");
    }
    
    // copy constructor
    if (args.Length() == 1) {
        CuboidObj * obj = CastToCuboid(args[0]);
        if (!obj) {
            return ThrowV8(Exception::TypeError, "argument must be Cuboid");
        }
        CuboidObj * result = new CuboidObj(cuboid_copy(obj->cuboidData));
        args.This()->SetInternalField(1, String::NewSymbol(CLASS_SPECIFIER));
        result->Wrap(args.This());
        return args.This();
    }
    
    // the first 3 argument should be integers
    if (args.Length() != 3) {
        return ThrowV8(Exception::Error, "constructor takes 3 or 1 argument(s)");
    }
    
    for (int i = 0; i < 3; i++) {
        if (!args[i]->IsNumber()) {
            return ThrowV8(Exception::TypeError, "each argument must be an integer");
        }
    }
    
    CuboidDimensions d;
    d.x = (uint8_t)args[0]->ToNumber()->Value();
    d.y = (uint8_t)args[0]->ToNumber()->Value();
    d.z = (uint8_t)args[0]->ToNumber()->Value();
    Cuboid * object = cuboid_create(d);
    
    CuboidObj * obj = new CuboidObj(object);
    args.This()->SetInternalField(1, String::NewSymbol(CLASS_SPECIFIER));
    obj->Wrap(args.This());
    
    return args.This();
}

Handle<Value> CuboidObj::Multiply(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    if (args.Length() != 1) {
        return ThrowV8(Exception::Error, "this function takes exactly 1 argument");
    }
    
    CuboidObj * rightObj = CastToCuboid(args[0]);
    if (!rightObj) {
        Handle<String> message = String::New("first argument must be a Cuboid");
        return ThrowException(Exception::TypeError(message));
    }
    
    Cuboid * right = rightObj->cuboidData;
    Cuboid * left = thisObj->cuboidData;
    if (!cuboid_dimensions_equal(left->dimensions, right->dimensions)) {
        Handle<String> message = String::New("dimensions do not match");
        return ThrowException(Exception::Error(message));
    }
    
    Cuboid * result = cuboid_create(right->dimensions);
    cuboid_multiply(result, left, right);
    return scope.Close(CreateCuboid(result));
}

Handle<Value> CuboidObj::EdgeIndex(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_2_INT_ARGS;
    
    int dedge = args[0]->ToNumber()->Value();
    if (dedge >= 12 || dedge < 0) {
        Handle<String> message = String::New("dedge index must be < 12 and >= 0");
        return ThrowException(Exception::RangeError(message));
    }
    
    int edge = args[1]->ToNumber()->Value();
    uint16_t index = cuboid_edge_index(thisObj->cuboidData, dedge, edge);
    return scope.Close(Number::New(index));
}

Handle<Value> CuboidObj::CenterIndex(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_2_INT_ARGS;
    
    int face = args[0]->ToNumber()->Value();
    if (face < 1 || face > 6) {
        Handle<String> message = String::New("face index must range from 1 to 6");
        return ThrowException(Exception::RangeError(message));
    }
    
    int piece = args[1]->ToNumber()->Value();
    uint16_t index = cuboid_center_index(thisObj->cuboidData, face, piece);
    return scope.Close(Number::New(index));
}


Handle<Value> CuboidObj::CountEdgesForDedge(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_1_INT_ARG;
    
    int dedge = args[0]->ToNumber()->Value();
    if (dedge < 0 || dedge > 11) {
        Handle<String> message = String::New("dedge index must be < 12 and >= 0");
        return ThrowException(Exception::RangeError(message));
    }
    
    uint16_t count = cuboid_count_edges_for_dedge(thisObj->cuboidData, dedge);
    return scope.Close(Number::New(count));
}


Handle<Value> CuboidObj::CountEdges(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    if (args.Length() != 0) {
        return ThrowV8(Exception::Error, "this function takes no arguments");
    }
    
    return scope.Close(Number::New(cuboid_count_edges(thisObj->cuboidData)));
}

Handle<Value> CuboidObj::CountCentersForFace(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_1_INT_ARG;
    
    int face = args[0]->ToNumber()->Value();
    if (face < 1 || face > 6) {
        Handle<String> message = String::New("face index must range from 1-6");
        return ThrowException(Exception::RangeError(message));
    }
    
    uint16_t count = cuboid_count_centers_for_face(thisObj->cuboidData, face);
    return scope.Close(Number::New(count));
}

Handle<Value> CuboidObj::CountCenters(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    if (args.Length() != 0) {
        return ThrowV8(Exception::Error, "this function takes no arguments");
    }
    
    return scope.Close(Number::New(cuboid_count_centers(thisObj->cuboidData)));
}

Handle<Value> CuboidObj::GetCenter(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_1_INT_ARG;
    
    int index = args[0]->ToNumber()->Value();
    if (index < 0 || index >= cuboid_count_centers(thisObj->cuboidData)) {
        return ThrowV8(Exception::RangeError, "invalid center index");
    }
    
    CuboidCenter c = thisObj->cuboidData->centers[index];
    Handle<Object> object = Object::New();
    Handle<Number> sideVal = Number::New((double)c.side);
    Handle<Number> indexVal = Number::New((double)c.index);
    object->Set(String::NewSymbol("side"), sideVal);
    object->Set(String::NewSymbol("index"), indexVal);

    return scope.Close(object);
}

Handle<Value> CuboidObj::GetEdge(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_1_INT_ARG;
    
    int index = args[0]->ToNumber()->Value();
    if (index < 0 || index >= cuboid_count_edges(thisObj->cuboidData)) {
        return ThrowV8(Exception::RangeError, "invalid edge index");
    }
    
    CuboidEdge e = thisObj->cuboidData->edges[index];
    Handle<Object> object = Object::New();
    Handle<Number> edgeVal = Number::New((double)e.edgeIndex);
    Handle<Number> symVal = Number::New((double)e.symmetry);
    Handle<Number> dedgeVal = Number::New((double)e.dedgeIndex);
    object->Set(String::NewSymbol("edgeIndex"), edgeVal);
    object->Set(String::NewSymbol("symmetry"), symVal);
    object->Set(String::NewSymbol("dedgeIndex"), dedgeVal);

    return scope.Close(object);
}

Handle<Value> CuboidObj::GetCorner(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_1_INT_ARG;
    
    int index = args[0]->ToNumber()->Value();
    if (index < 0 || index >= 12) {
        return ThrowV8(Exception::RangeError, "invalid corner index");
    }
    
    CuboidCorner c = thisObj->cuboidData->corners[index];
    Handle<Object> object = Object::New();
    Handle<Number> indexVal = Number::New((double)c.index);
    Handle<Number> symVal = Number::New((double)c.symmetry);
    object->Set(String::NewSymbol("index"), indexVal);
    object->Set(String::NewSymbol("symmetry"), symVal);

    return scope.Close(object);
}

Handle<Value> CuboidObj::GetDimensions(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    CuboidDimensions dimensions = thisObj->cuboidData->dimensions;
    
    // create our result {x: <x>, y: <y>, z: <z>}
    Handle<Object> object = Object::New();
    Handle<Number> xVal = Number::New((double)dimensions.x);
    Handle<Number> yVal = Number::New((double)dimensions.y);
    Handle<Number> zVal = Number::New((double)dimensions.z);
    object->Set(String::NewSymbol("x"), xVal);
    object->Set(String::NewSymbol("y"), yVal);
    object->Set(String::NewSymbol("z"), zVal);

    return scope.Close(object);
}

/* PROTECTED */

CuboidObj * CuboidObj::ProcessThisArgument(const v8::Arguments& args) {
    return CastToCuboid(args.This());
}

CuboidObj * CuboidObj::CastToCuboid(Handle<Value> value) {
    HandleScope scope;
    
    // use our internal field to validate the instance
    if (!value->IsObject()) return NULL;
    Handle<Object> obj = value->ToObject();
    if (obj->InternalFieldCount() != 2) return NULL;
    if (!obj->GetInternalField(1)->IsString()) return NULL;
    
    String::AsciiValue str(obj->GetInternalField(1));
    if (strcmp(*str, CLASS_SPECIFIER) != 0) return NULL;
    
    // this is now considered "safe"
    return ObjectWrap::Unwrap<CuboidObj>(value->ToObject());
}

Handle<Object> CuboidObj::CreateCuboid(Cuboid * internal) {
    // we have to go through our constructor in order to
    // get a valid instance of a CuboidObj
    
    HandleScope scope;
    
    Handle<Function> function = constructor->GetFunction();
    Handle<Value> argv[] = {
        Number::New(internal->dimensions.x),
        Number::New(internal->dimensions.y),
        Number::New(internal->dimensions.z)
    };
    
    Handle<Object> result = function->NewInstance(3, argv);
    CuboidObj * resultObj = CastToCuboid(result);
    cuboid_free(resultObj->cuboidData);
    resultObj->cuboidData = internal;
    
    return scope.Close(result);
}
