#include "stickermap.h"

#define CLASS_SPECIFIER "STICKERMAP_REPRESENTATION"
#define GETTHIS StickerMapObj * thisObj = ProcessThisArgument(args);\
    if (!thisObj) return ThrowV8(Exception::TypeError, "must be called on StickerMap object");

using namespace v8;

Persistent<FunctionTemplate> StickerMapObj::constructor;

void StickerMapObj::Init(Handle<Object> target) {
    HandleScope scope;
    
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    Local<String> name = String::NewSymbol("StickerMap");

    constructor = Persistent<FunctionTemplate>::New(tpl);
    constructor->InstanceTemplate()->SetInternalFieldCount(2);
    constructor->SetClassName(name);

    NODE_SET_PROTOTYPE_METHOD(constructor, "getCuboid", GetCuboid);
    NODE_SET_PROTOTYPE_METHOD(constructor, "getDimensions", GetDimensions);
    
    NODE_SET_PROTOTYPE_METHOD(constructor, "countStickers", CountStickers);
    NODE_SET_PROTOTYPE_METHOD(constructor, "countStickersForFace", CountStickersForFace);
    NODE_SET_PROTOTYPE_METHOD(constructor, "faceStartIndex", FaceStartIndex);
    
    NODE_SET_PROTOTYPE_METHOD(constructor, "dimensionsOfFace", DimensionsOfFace);
    NODE_SET_PROTOTYPE_METHOD(constructor, "indexToPoint", IndexToPoint);
    NODE_SET_PROTOTYPE_METHOD(constructor, "indexFromPoint", IndexFromPoint);
    
    NODE_SET_PROTOTYPE_METHOD(constructor, "getSticker", GetSticker);
    NODE_SET_PROTOTYPE_METHOD(constructor, "setSticker", SetSticker);
    
    // This has to be last, otherwise the properties won't show up on the
    // object in JavaScript.
    target->Set(name, constructor->GetFunction());
}

StickerMapObj::StickerMapObj(StickerMap * object) : ObjectWrap() {
    stickerMap = object;
}

StickerMapObj::~StickerMapObj() {
    stickermap_free(stickerMap);
}

Handle<Value> StickerMapObj::New(const Arguments& args) {
    HandleScope scope;
    StickerMap * map = NULL;
    
    if (!args.IsConstructCall()) {
        return ThrowV8(Exception::TypeError, "must be constructor call");
    }
    
    if (args.Length() == 1) {
        CuboidObj * obj = CuboidObj::CastToCuboid(args[0]);
        if (!obj) {
            StickerMapObj * stickerObj = CastToStickerMap(args[0]);
            if (!stickerObj) {
                return ThrowV8(Exception::TypeError, "argument must be StickerMap or Cuboid");
            }
            // create a duplicate stickermap
            CuboidDimensions dim = stickerObj->stickerMap->dimensions;
            map = stickermap_create(dim);
            size_t len = stickermap_count_stickers(map);
            memcpy(map->stickers, stickerObj->stickerMap->stickers, len);
        } else {
            // create a stickermap from a cuboid
            map = stickermap_create(obj->getCuboidData()->dimensions);
            convert_cb_to_sm(map, obj->getCuboidData());
        }
    } else if (args.Length() == 3) {
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
        map = stickermap_create(d);
        size_t len = stickermap_count_stickers(map);
        memset(map->stickers, 1, len);
    } else {
        return ThrowV8(Exception::Error, "this function takes 1 or 3 arguments");
    }
    
    StickerMapObj * result = new StickerMapObj(map);
    args.This()->SetInternalField(1, String::NewSymbol(CLASS_SPECIFIER));
    result->Wrap(args.This());
    return args.This();
}

/** Instance Methods **/

Handle<Value> StickerMapObj::GetCuboid(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    Cuboid * cb = cuboid_create(thisObj->stickerMap->dimensions);
    if (!convert_sm_to_cb(cb, thisObj->stickerMap)) {
        cuboid_free(cb);
        return ThrowV8(Exception::Error, "invalid sticker data");
    }
    return scope.Close(CuboidObj::CreateCuboid(cb));
}

Handle<Value> StickerMapObj::GetDimensions(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    CuboidDimensions dimensions = thisObj->stickerMap->dimensions;
    return scope.Close(dimensionsToObject(dimensions));
}

Handle<Value> StickerMapObj::CountStickers(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    uint32_t count = stickermap_count_stickers(thisObj->stickerMap);
    return scope.Close(Number::New((double)count));
}

Handle<Value> StickerMapObj::CountStickersForFace(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    Ensure1IntArg(args);
    
    int face = args[0]->ToNumber()->Value();
    if (face < 1 || face > 6) {
        return ThrowV8(Exception::RangeError, "face index must range from 1 to 6");
    }
    
    uint16_t count = stickermap_count_stickers_for_face(thisObj->stickerMap, face);
    return scope.Close(Number::New((double)count));
}

Handle<Value> StickerMapObj::FaceStartIndex(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    Ensure1IntArg(args);
    
    int face = args[0]->ToNumber()->Value();
    if (face < 1 || face > 6) {
        return ThrowV8(Exception::RangeError, "face index must range from 1 to 6");
    }
    
    uint32_t index = stickermap_face_start_index(thisObj->stickerMap, face);
    return scope.Close(Number::New((double)index));
}

Handle<Value> StickerMapObj::DimensionsOfFace(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    Ensure1IntArg(args);
    
    int face = args[0]->ToNumber()->Value();
    if (face < 1 || face > 6) {
        return ThrowV8(Exception::RangeError, "face index must range from 1 to 6");
    }
    
    int x, y;
    stickermap_dimensions_of_face(thisObj->stickerMap, face, &x, &y);
    return scope.Close(flatDimensionsToObject(x, y));
}

Handle<Value> StickerMapObj::IndexToPoint(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    Ensure2IntArgs(args);
    
    int face = args[0]->ToNumber()->Value();
    if (face < 1 || face > 6) {
        return ThrowV8(Exception::RangeError, "face index must range from 1 to 6");
    }
    
    int faceIndex = args[1]->ToNumber()->Value();
    uint16_t max = stickermap_count_stickers_for_face(thisObj->stickerMap, face);
    if (faceIndex >= max || faceIndex < 0) {
        return ThrowV8(Exception::RangeError, "face sticker index out of bounds");
    }
    
    int x, y;
    stickermap_index_to_point(thisObj->stickerMap, face, faceIndex, &x, &y);
    return scope.Close(flatDimensionsToObject(x, y));
}

Handle<Value> StickerMapObj::IndexFromPoint(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    
    if (args.Length() != 3) {
        return ThrowV8(Exception::Error, "this function takes exactly 3 argumetns");
    }
    for (int i = 0; i < 3; i++) {
        if (!args[i]->IsNumber()) {
            return ThrowV8(Exception::TypeError, "arguments must be integers");
        }
    }
    
    int face = args[0]->ToNumber()->Value();
    if (face < 1 || face > 6) {
        return ThrowV8(Exception::RangeError, "face index must range from 1 to 6");
    }
    int x = args[1]->ToNumber()->Value();
    int y = args[2]->ToNumber()->Value();
    int maxX, maxY;
    stickermap_dimensions_of_face(thisObj->stickerMap, face, &maxX, &maxY);
    if (x < 0 || x >= maxX) {
        return ThrowV8(Exception::RangeError, "x coordinate out of bounds");
    }
    if (y < 0 || y >= maxX) {
        return ThrowV8(Exception::RangeError, "y coordinate out of bounds");
    }
    
    int index = stickermap_index_from_point(thisObj->stickerMap, face, x, y);
    return scope.Close(Number::New((double)index));
}

Handle<Value> StickerMapObj::GetSticker(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    Ensure1IntArg(args);
    
    int index = args[0]->ToNumber()->Value();
    if (index < 0 || index >= (int)stickermap_count_stickers(thisObj->stickerMap)) {
        return ThrowV8(Exception::RangeError, "index out of bounds");
    }
    
    uint8_t sticker = thisObj->stickerMap->stickers[index];
    return scope.Close(Number::New((double)sticker));
}

Handle<Value> StickerMapObj::SetSticker(const Arguments& args) {
    HandleScope scope;
    GETTHIS;
    Ensure2IntArgs(args);
    
    int index = args[0]->ToNumber()->Value();
    if (index < 0 || index >= (int)stickermap_count_stickers(thisObj->stickerMap)) {
        return ThrowV8(Exception::RangeError, "index out of bounds");
    }
    uint8_t sticker = (uint8_t)args[1]->ToNumber()->Value();
    if (sticker < 1 || sticker > 6) {
        return ThrowV8(Exception::RangeError, "invalid sticker number");
    }
    thisObj->stickerMap->stickers[index] = sticker;
    
    return scope.Close(Undefined());
}

/** Utility **/

Handle<Value> StickerMapObj::CreateStickerMap(StickerMap * object) {
    HandleScope scope;
    
    Handle<Function> function = constructor->GetFunction();
    Handle<Value> argv[] = {
        Number::New(object->dimensions.x),
        Number::New(object->dimensions.y),
        Number::New(object->dimensions.z)
    };
    
    Handle<Object> result = function->NewInstance(3, argv);
    StickerMapObj * resultObj = CastToStickerMap(result);
    stickermap_free(resultObj->stickerMap);
    resultObj->stickerMap = object;
    
    return scope.Close(result);
}

StickerMapObj * StickerMapObj::CastToStickerMap(Handle<Value> value) {
    if (!verifySpecifier(value, CLASS_SPECIFIER)) return NULL;
    return ObjectWrap::Unwrap<StickerMapObj>(value->ToObject());
}

/* PROTECTED */

StickerMapObj * StickerMapObj::ProcessThisArgument(const Arguments& args) {
    return CastToStickerMap(args.This());
}
