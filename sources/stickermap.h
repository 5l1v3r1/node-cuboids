#ifndef _STICKERMAP_H_
#define _STICKERMAP_H_

#include "representation.h"

extern "C" {
#include "stickers/stickermap.h"
#include "stickers/mapconversion.h"
}

class StickerMapObj : public node::ObjectWrap {
public:
    static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Handle<v8::Object> target);

    StickerMapObj(StickerMap * object);
    ~StickerMapObj();

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetCuboid(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetDimensions(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> CountStickers(const v8::Arguments& args);
    static v8::Handle<v8::Value> CountStickersForFace(const v8::Arguments& args);
    static v8::Handle<v8::Value> FaceStartIndex(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> DimensionsOfFace(const v8::Arguments& args);
    static v8::Handle<v8::Value> IndexToPoint(const v8::Arguments& args);
    static v8::Handle<v8::Value> IndexFromPoint(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> GetSticker(const v8::Arguments& args);
    static v8::Handle<v8::Value> SetSticker(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> CreateStickerMap(StickerMap * object);
    static StickerMapObj * CastToStickerMap(v8::Handle<v8::Value> value);
    
protected:
    static StickerMapObj * ProcessThisArgument(const v8::Arguments& args);
    
    StickerMap * stickerMap;
};

#endif