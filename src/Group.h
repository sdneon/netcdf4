#ifndef NETCDF4JS_GROUP_H
#define NETCDF4JS_GROUP_H

#include <node.h>
#include <node_object_wrap.h>

namespace netcdf4js {

class Variable;

class Group : public node::ObjectWrap {
  public:
    Group(const int& id);
    static void Init(v8::Local<v8::Object> exports);
    bool get_name(char* name) const;

  private:
    static v8::Persistent<v8::Function> constructor;
    static void GetId(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetVariables(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetDimensions(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetUnlimited(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetAttributes(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetSubgroups(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetName(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetFullname(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddDimension(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddSubgroup(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddVariable(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Inspect(const v8::FunctionCallbackInfo<v8::Value>& args);
    int id;
};
}  // namespace netcdf4js

#endif
