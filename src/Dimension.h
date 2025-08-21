#ifndef NETCDF4JS_DIMENSION_H
#define NETCDF4JS_DIMENSION_H

#include <node.h>
#include <node_object_wrap.h>

namespace netcdf4js {

#define FN_GETTER(clsName, name) \
    v8::Local<v8::FunctionTemplate> getter_##name = v8::FunctionTemplate::New( \
        isolate, ##clsName::Get##name, v8::Local<v8::Value>(), v8::Signature::New(isolate, tpl));

#define FN_GETTER_AND_SETTER(clsName, name) \
    v8::Local<v8::FunctionTemplate> getter_##name = v8::FunctionTemplate::New( \
        isolate, ##clsName::Get##name, v8::Local<v8::Value>(), v8::Signature::New(isolate, tpl)); \
    v8::Local<v8::FunctionTemplate> setter_##name = v8::FunctionTemplate::New( \
        isolate, ##clsName::Set##name, v8::Local<v8::Value>(), v8::Signature::New(isolate, tpl));

class Dimension : public node::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports);
    Dimension(const int& id_, const int& parent_id_);
    bool get_name(char* name) const;
    size_t getLen();

  private:
    static v8::Persistent<v8::Function> constructor;
    static void GetId(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetLength(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void GetName(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void SetName(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void Inspect(const v8::FunctionCallbackInfo<v8::Value>& args);
    int id;
    int parent_id;
};
}  // namespace netcdf4js

#endif
