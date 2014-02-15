#include "cuboid_base.h"

#define CLASS_SPECIFIER "CUBOID_REPRESENTATION"

// these are really nice because they return *for* you
#define GETTHIS CuboidObj * thisObj = ProcessThisArgument(args);\
    if (!thisObj) return ThrowV8(Exception::TypeError, "must be called on Cuboid object");
#define ENSURE_OBJ_INT_ARGS if (args.Length() != 2) {\
        return ThrowV8(Exception::Error, "this function takes exactly 2 arguments");\
    }\
    if (!args[0]->IsNumber()) {\
        return ThrowV8(Exception::TypeError, "the center index must be an integer");\
    }\
    if (!args[1]->IsObject()) {\
        return ThrowV8(Exception::TypeError, "the center must be an object");\
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
    
    // setting piece information
    NODE_SET_PROTOTYPE_METHOD(constructor, "setCenter", SetCenter);
    NODE_SET_PROTOTYPE_METHOD(constructor, "setEdge", SetEdge);
    NODE_SET_PROTOTYPE_METHOD(constructor, "setCorner", SetCorner);

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
    
    if (d.x < 2 || d.y < 2 || d.z < 2) {
        return ThrowV8(Exception::Error, "each dimensions must be > 1");
    }
    
    Cuboid * object = cuboid_create(d);
    
    CuboidObj * obj = new CuboidObj(object);
    args.This()->SetInternalField(1, String::NewSymbol(CLASS_SPECIFIER));
    obj->Wrap(args.This());
    
    return args.This();
}

/** Instance Methods **/

Handle<Value> CuboidObj::Multiply(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    if (args.Length() != 1) {
        return ThrowV8(Exception::Error, "this function takes exactly 1 argument");
    }
    
    CuboidObj * rightObj = CastToCuboid(args[0]);
    if (!rightObj) {
        // TODO: replace all ThrowException with ThrowV8
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
    Ensure2IntArgs(args);
    
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
    Ensure2IntArgs(args);
    
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
    Ensure1IntArg(args);
    
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
    Ensure1IntArg(args);
    
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
    Ensure1IntArg(args);
    
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
    Ensure1IntArg(args);
    
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
    Ensure1IntArg(args);
    
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
    return scope.Close(dimensionsToObject(dimensions));
}

Handle<Value> CuboidObj::SetCenter(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_OBJ_INT_ARGS;
    
    uint32_t slot = args[0]->ToNumber()->Value();
    if (slot >= cuboid_count_centers(thisObj->cuboidData)) {
        return ThrowV8(Exception::RangeError, "the center index is invalid");
    }
    
    Handle<Object> center = args[1]->ToObject();
    if (!center->Get(String::NewSymbol("side"))->IsNumber()) {
        return ThrowV8(Exception::TypeError, "the side must be an integer");
    }
    if (!center->Get(String::NewSymbol("index"))->IsNumber()) {
        return ThrowV8(Exception::TypeError, "the index must be an integer");
    }
    
    uint8_t side = center->Get(String::NewSymbol("side"))->ToNumber()->Value();
    uint16_t index = center->Get(String::NewSymbol("index"))->ToNumber()->Value();
    if (side < 1 || side > 6) {
        return ThrowV8(Exception::RangeError, "the side must range from 1 to 6");
    }
    if (index >= cuboid_count_centers_for_face(thisObj->cuboidData, side)) {
        return ThrowV8(Exception::RangeError, "invalid piece index");
    }
    
    CuboidCenter c;
    c.side = side;
    c.index = index;
    thisObj->cuboidData->centers[slot] = c;
    return scope.Close(Undefined());
}

Handle<Value> CuboidObj::SetEdge(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_OBJ_INT_ARGS;
    
    uint32_t slot = args[0]->ToNumber()->Value();
    if (slot >= cuboid_count_edges(thisObj->cuboidData)) {
        return ThrowV8(Exception::RangeError, "the edge index is invalid");
    }
    
    Handle<Object> edge = args[1]->ToObject();
    if (!edge->Get(String::NewSymbol("edgeIndex"))->IsNumber()) {
        return ThrowV8(Exception::TypeError, "the edgeIndex must be an integer");
    }
    if (!edge->Get(String::NewSymbol("dedgeIndex"))->IsNumber()) {
        return ThrowV8(Exception::TypeError, "the dedgeIndex must be an integer");
    }
    if (!edge->Get(String::NewSymbol("symmetry"))->IsNumber()) {
        return ThrowV8(Exception::TypeError, "the symmetry must be an integer");
    }
    
    uint8_t edgeIndex = edge->Get(String::NewSymbol("edgeIndex"))->ToNumber()->Value();
    uint8_t dedgeIndex = edge->Get(String::NewSymbol("dedgeIndex"))->ToNumber()->Value();
    uint8_t symmetry = edge->Get(String::NewSymbol("symmetry"))->ToNumber()->Value();
    if (symmetry >= 6) {
        return ThrowV8(Exception::RangeError, "symmetry must range from 0 to 5");
    }
    if (dedgeIndex >= 12) {
        return ThrowV8(Exception::RangeError, "dedgeIndex must range from 0 to 11");
    }
    if (edgeIndex >= cuboid_count_edges_for_dedge(thisObj->cuboidData, dedgeIndex)) {
        return ThrowV8(Exception::RangeError, "edgeIndex out of bounds");
    }
    
    CuboidEdge e;
    e.edgeIndex = edgeIndex;
    e.dedgeIndex = dedgeIndex;
    e.symmetry = symmetry;
    thisObj->cuboidData->edges[slot] = e;
    return scope.Close(Undefined());
}

Handle<Value> CuboidObj::SetCorner(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    ENSURE_OBJ_INT_ARGS;
    
    uint32_t slot = args[0]->ToNumber()->Value();
    if (slot >= 8) {
        return ThrowV8(Exception::RangeError, "the center index is invalid");
    }
    
    Handle<Object> corner = args[1]->ToObject();
    if (!corner->Get(String::NewSymbol("index"))->IsNumber()) {
        return ThrowV8(Exception::TypeError, "the index must be an integer");
    }
    if (!corner->Get(String::NewSymbol("symmetry"))->IsNumber()) {
        return ThrowV8(Exception::TypeError, "the symmetry must be an integer");
    }
    uint8_t index = corner->Get(String::NewSymbol("index"))->ToNumber()->Value();
    uint8_t symmetry = corner->Get(String::NewSymbol("symmetry"))->ToNumber()->Value();
    if (index >= 8) {
        return ThrowV8(Exception::RangeError, "invaild piece index");
    }
    if (symmetry >= 6) {
        return ThrowV8(Exception::RangeError, "symmetry must range from 0 to 5");
    }
    
    CuboidCorner c;
    c.index = index;
    c.symmetry = symmetry;
    thisObj->cuboidData->corners[slot] = c;
    return scope.Close(Undefined());
}

/** Utility **/

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

CuboidObj * CuboidObj::CastToCuboid(Handle<Value> value) {
    if (!verifySpecifier(value, CLASS_SPECIFIER)) return NULL;
    return ObjectWrap::Unwrap<CuboidObj>(value->ToObject());
}

Cuboid * CuboidObj::getCuboidData() {
    return cuboidData;
}

/* PROTECTED */

CuboidObj * CuboidObj::ProcessThisArgument(const Arguments& args) {
    return CastToCuboid(args.This());
}
