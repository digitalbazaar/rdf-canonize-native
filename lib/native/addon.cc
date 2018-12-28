/*********************************************************************
 * rdf-canonize addon for Node.js.
 *
 * Copyright (c) 2017 Digital Bazaar, Inc.
 *
 * BSD License
 * <https://github.com/digitalbazaar/rdf-canonize/blob/master/LICENSE>
 ********************************************************************/

#include <napi.h>
#include <uv.h>
#include "addon.h"   // NOLINT(build/include)
#include "urdna2015.h"  // NOLINT(build/include)

// using Napi::AsyncQueueWorker;
using Napi::AsyncWorker;
using Napi::FunctionReference;
// using Napi::GetFunction;
using Napi::HandleScope;
// using Napi::MaybeLocal;
// using Napi::New;
// using Napi::Null;
// using Napi::Set;
// using Napi::To;
using std::string;
using Napi::Array;
using Napi::Function;
using Napi::FunctionReference;
// using Napi::Handle;
// using Napi::Isolate;
using Napi::Number;
using Napi::Object;
using Napi::String;
using Napi::Value;

using namespace RdfCanonize;

class Urdna2015Worker : public AsyncWorker {
public:
  Urdna2015Worker(
    Urdna2015 urdna2015,
    Dataset* dataset,
    Napi::Promise::Deferred deferred,
    const Napi::Function& callback
  ) : AsyncWorker(callback), urdna2015(urdna2015), dataset(dataset),
  deferred(deferred) {}
  ~Urdna2015Worker() {
    delete dataset;
  }

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute () {
    output = urdna2015.main(*dataset);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void OnOK () {
    // HandleScope scope;
    Napi::Env env = Env();
    Napi::HandleScope scope(env);
    deferred.Resolve(String::New(env, output.c_str()));
    Callback().Call({});
    // Callback().MakeCallback(
    //   Receiver().Value(),
    //   {
    //     env.Null(),
    //     Napi::String::New(env, output.c_str())
    //   }
    // );

    // Napi::Value argv[] = {
    //   Null(),
    //   New(output.c_str())
    // };
    //
    // callback->Call(2, argv);
  }

private:
  Urdna2015 urdna2015;
  Dataset* dataset;
  std::string output;
  Napi::Promise::Deferred deferred;
};

static bool fillDataset(
  Napi::Env env,
  Dataset& dataset,
  const Napi::Array& datasetArray);
static bool createTerm(
  Napi::Env env,
  Term*& term,
  const Napi::Object& object);
Napi::Value EmptyCallback(const Napi::CallbackInfo& info);

Napi::Value Main(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  // ensure first argument is an object
  if(!info[0].IsObject()) {
    Napi::TypeError::New(env, "'options' must be an object").ThrowAsJavaScriptException();
    return env.Null();
  }
  // ensure second argument is a callback
  // if(!info[1].IsFunction()) {
  //   Napi::TypeError::New(env, "'callback' must be a function").ThrowAsJavaScriptException();
  //   return;
  // }
  //
  // Napi::Function callback = info[1].As<Napi::Function>();
  // Callback* callback = new Callback(info[1].As<Napi::Function>());

  Napi::Object object = info[0].As<Napi::Object>();
  Napi::Array datasetArray = object.Get("dataset").As<Napi::Array>();
  // Handle<Object> object = info[0].As<Handle<Object>>();
  /*
  Handle<Value> maxCallStackDepthValue =
    object->Get(New("maxCallStackDepth"));
  Handle<Value> maxTotalCallStackDepthValue =
    object->Get(New("maxTotalCallStackDepth"));
  */
  // Napi::Array datasetArray =
  //   object.Get("dataset")->Get(New("dataset".As<Napi::Array>()));

  /*
  const unsigned maxCallStackDepth =
    To<unsigned>(maxCallStackDepthValue);
  const unsigned maxTotalCallStackDepth =
    To<unsigned>(maxTotalCallStackDepthValue);
  */

  //Urdna2015 urdna2015(maxCallStackDepth, maxTotalCallStackDepth);
  Urdna2015 urdna2015(0, 0);

  Dataset* dataset = new Dataset();
  if(!fillDataset(env, *dataset, datasetArray)) {
    delete dataset;
    // TODO: call `callback` with the error?
    return env.Null();
  }
  Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
  Napi::Function callback = Napi::Function::New(env, EmptyCallback);
  // AsyncQueueWorker(new Urdna2015Worker(urdna2015, dataset, callback));
  // napi_queue_async_work(env, new Urdna2015Worker(urdna2015, dataset, callback));
  // (new Urdna2015Worker(urdna2015, dataset, callback))->Queue();
  (new Urdna2015Worker(urdna2015, dataset, deferred, callback))->Queue();
  return deferred.Promise();
}

Napi::Value MainSync(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  // ensure first argument is an object
  if(!info[0].IsObject()) {
    Napi::TypeError::New(env, "'options' must be an object").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::Object object = info[0].As<Napi::Object>();
  Napi::Array datasetArray = object.Get("dataset").As<Napi::Array>();

  // Handle<Object> object = info[0].As<Handle<Object>>();
  /*
  Handle<Value> maxCallStackDepthValue =
    object->Get(New("maxCallStackDepth"));
  Handle<Value> maxTotalCallStackDepthValue =
    object->Get(New("maxTotalCallStackDepth"));
  */
    // Handle<Array> datasetArray =
    //   object->Get(New("dataset".As<Handle<Array>>()));

  /*
  const unsigned maxCallStackDepth =
    To<unsigned>(maxCallStackDepthValue);
  const unsigned maxTotalCallStackDepth =
    To<unsigned>(maxTotalCallStackDepthValue);
  */

  //Urdna2015 urdna2015(maxCallStackDepth, maxTotalCallStackDepth);
  Urdna2015 urdna2015(0, 0);

  Dataset dataset;
  if(!fillDataset(env, dataset, datasetArray)) {
    return env.Null();
  }

  std::string output = urdna2015.main(dataset);
  return Napi::String::New(env, output.c_str());
}

static bool fillDataset(Napi::Env env, Dataset& dataset, const Napi::Array& datasetArray) {
  // TODO: check for valid structure
  for(size_t di = 0; di < datasetArray.Length(); ++di) {
    Napi::Object quad = datasetArray.Get(di).As<Napi::Object>();

    // TODO: ensure all keys are present and represent objects

    Napi::Object subject = quad.Get("subject").As<Napi::Object>();
    Napi::Object predicate = quad.Get("predicate").As<Napi::Object>();
    Napi::Object object = quad.Get("object").As<Napi::Object>();
    Napi::Object graph = quad.Get("graph").As<Napi::Object>();

    Quad* q = new Quad();

    if(!(
      createTerm(env, q->subject, subject) &&
      createTerm(env, q->predicate, predicate) &&
      createTerm(env, q->object, object) &&
      createTerm(env, q->graph, graph))) {
      delete q;
      return false;
    }

    // TODO: ensure q is valid (term types all valid for s, p, o, g, etc.)

    dataset.quads.push_back(q);
  }

  return true;
}

static bool createTerm(Napi::Env env, Term*& term, const Napi::Object& object) {
  if(!(object.Has("termType") && object.Get("termType").IsString())) {
    throw Napi::TypeError::New(
      env, "'termType' must be 'BlankNode', 'NamedNode', " \
      "'Literal', or 'DefaultGraph'.");
    return false;
  }

  string termType = object.Get("termType").As<Napi::String>().Utf8Value();

  if(termType.compare("BlankNode") == 0) {
    term = new BlankNode();
  } else if(termType.compare("NamedNode") == 0) {
    term = new NamedNode();
  } else if(termType.compare("Literal") == 0) {
    Literal* literal = new Literal();
    term = literal;
    // FIXME: is the casing here correct?
    if(object.Has("datatype")) {
      // FIXME: this code is probably never executed? Since we're casting it
      // as an object and only later testing to see if it's an object?
      Napi::Object datatype = object.Get("datatype").As<Napi::Object>();
      if(!datatype.IsObject() || datatype.IsNull()) {
        throw Napi::TypeError::New(
          env, "'termType' must be 'BlankNode', 'NamedNode', " \
          "'Literal', or 'DefaultGraph'.");
        return false;
      }
      Term* dataTypeTerm;
      if(!createTerm(env, dataTypeTerm, datatype)) {
        return false;
      }
      if(dataTypeTerm->termType != TermType::NAMED_NODE) {
        throw Napi::TypeError::New(
          env, "datatype 'termType' must be 'NamedNode'.");
        // Napi::Error::New(env, "datatype 'termType' must be 'NamedNode'.").ThrowAsJavaScriptException();

        delete dataTypeTerm;
        return false;
      }
      literal->datatype = (NamedNode*)dataTypeTerm;
    }
    if(object.Has("language")) {
      literal->language = object.Get("language").As<Napi::String>().Utf8Value();
    }
  } else if(termType.compare("DefaultGraph") == 0) {
    term = new DefaultGraph();
  } else {
    throw Napi::TypeError::New(
      env, "'termType' must be 'BlankNode', 'NamedNode', " \
      "'Literal', or 'DefaultGraph'.");
    return false;
  }

  term->value = object.Get("value").As<Napi::String>().Utf8Value();

  return true;
}

Napi::Value EmptyCallback(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "main"),
    Napi::Function::New(env, Main)
  );
  exports.Set(
    Napi::String::New(env, "mainSync"),
    Napi::Function::New(env, MainSync)
  );
  return exports;
}

NODE_API_MODULE(addon, Init)
