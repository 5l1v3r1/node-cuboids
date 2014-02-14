#ifndef _REPRESENTATION_H_
#define _REPRESENTATION_H_

#include <node.h>
#include "util.h"

extern "C" {
#include "representation/cuboid.h"
}

class CuboidObj : public node::ObjectWrap {
public:
    static v8::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Handle<v8::Object> target);

    CuboidObj(Cuboid * object);
    ~CuboidObj();

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> Multiply(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> EdgeIndex(const v8::Arguments& args);
    static v8::Handle<v8::Value> CenterIndex(const v8::Arguments& args);
    static v8::Handle<v8::Value> CountEdgesForDedge(const v8::Arguments& args);
    static v8::Handle<v8::Value> CountEdges(const v8::Arguments& args);
    static v8::Handle<v8::Value> CountCentersForFace(const v8::Arguments& args);
    static v8::Handle<v8::Value> CountCenters(const v8::Arguments& args);

    static v8::Handle<v8::Value> GetCenter(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetEdge(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetCorner(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetDimensions(const v8::Arguments& args);
    
    // TODO: create *move* constructors here, etc.
    
    static v8::Handle<v8::Object> CreateCuboid(Cuboid * internal);
    static CuboidObj * CastToCuboid(v8::Handle<v8::Value> value);
    
    Cuboid * getCuboidData();
    
protected:
    static CuboidObj * ProcessThisArgument(const v8::Arguments& args);
    Cuboid * cuboidData;
};

#endif
