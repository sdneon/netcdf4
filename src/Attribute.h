#ifndef NETCDF4JS_ATTRIBUTE_H
#define NETCDF4JS_ATTRIBUTE_H

#include <node.h>
#include <node_object_wrap.h>
#include <string>

namespace netcdf4js {

class Attribute : public node::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports);
    Attribute(const char* name_, int var_id_, int parent_id_);
    Attribute(const char* name_, int var_id_, int parent_id_, int type_);
    void set_value(const v8::Local<v8::Value>& val);

  private:
    static v8::Persistent<v8::Function> constructor;
    static void Delete(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetName(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void SetName(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetValue(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void SetValue(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Inspect(const v8::FunctionCallbackInfo<v8::Value>& args);
    std::string name;
    int var_id;
    int parent_id;
    int type;
};
}  // namespace netcdf4js

#endif
