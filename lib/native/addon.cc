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
    const Napi::Function& callback
  ) : AsyncWorker(callback), urdna2015(urdna2015), dataset(dataset) {}
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

    Callback().MakeCallback(
      Receiver().Value(),
      {
        env.Null(),
        Napi::String::New(env, output.c_str())
      }
    );

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
};

static bool fillDataset(
  Dataset& dataset, const Napi::Array& datasetArray);
static bool createTerm(
  Term*& term,
  const Napi::Object& object,
  const Napi::String& termTypeKey,
  const Napi::String& valueKey,
  const Napi::String& datatypeKey,
  const Napi::String& languageKey);

Napi::Value Main(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  // ensure first argument is an object
  if(!info[0].IsObject()) {
    Napi::TypeError::New(env, "'options' must be an object").ThrowAsJavaScriptException();
    return env.Null();
  }
  // ensure second argument is a callback
  if(!info[1].IsFunction()) {
    Napi::TypeError::New(env, "'callback' must be a function").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::Function callback = info[1].As<Napi::Function>();
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
  if(!fillDataset(*dataset, datasetArray)) {
    delete dataset;
    // TODO: call `callback` with the error?
    return env.Null();
  }

  // AsyncQueueWorker(new Urdna2015Worker(urdna2015, dataset, callback));
  // napi_queue_async_work(env, new Urdna2015Worker(urdna2015, dataset, callback));
  (new Urdna2015Worker(urdna2015, dataset, callback))->Queue();
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
  if(!fillDataset(dataset, datasetArray)) {
    return env.Null();
  }

  std::string output = urdna2015.main(dataset);
  return New(output.c_str());
}

static bool fillDataset(Dataset& dataset, const Handle<Array>& datasetArray) {
  Napi::String subjectKey = New("subject");
  Napi::String predicateKey = New("predicate");
  Napi::String objectKey = New("object");
  Napi::String graphKey = New("graph");

  Napi::String termTypeKey = New("termType");
  Napi::String valueKey = New("value");
  Napi::String datatypeKey = New("datatype");
  Napi::String languageKey = New("language");

  // TODO: check for valid structure
  for(size_t di = 0; di < datasetArray->Length(); ++di) {
    Handle<Object> quad = datasetArray->Get(di.As<Handle<Object>>());

    // TODO: ensure all keys are present and represent objects

    Handle<Object> subject =
      quad->Get(subjectKey.As<Handle<Object>>());
    Handle<Object> predicate =
      quad->Get(predicateKey.As<Handle<Object>>());
    Handle<Object> object =
      quad->Get(objectKey.As<Handle<Object>>());
    Handle<Object> graph =
      quad->Get(graphKey.As<Handle<Object>>());

    Quad* q = new Quad();

    if(!(
      createTerm(
        q->subject, subject,
        termTypeKey, valueKey, datatypeKey, languageKey) &&
      createTerm(
        q->predicate, predicate,
        termTypeKey, valueKey, datatypeKey, languageKey) &&
      createTerm(
        q->object, object,
        termTypeKey, valueKey, datatypeKey, languageKey) &&
      createTerm(
        q->graph, graph,
        termTypeKey, valueKey, datatypeKey, languageKey))) {
      delete q;
      return false;
    }

    // TODO: ensure q is valid (term types all valid for s, p, o, g, etc.)

    dataset.quads.push_back(q);
  }

  return true;
}

static bool createTerm(
  Term*& term,
  const Handle<Object>& object,
  const Napi::String& termTypeKey,
  const Napi::String& valueKey,
  const Napi::String& datatypeKey,
  const Napi::String& languageKey) {
  if(!(object->Has(termTypeKey) && object->Get(termTypeKey).IsString())) {
    Napi::ThrowTypeError(
      "'termType' must be 'BlankNode', 'NamedNode', " \
      "'Literal', or 'DefaultGraph'.");
    return false;
  }

  Utf8String termType(object->Get(termTypeKey));

  if(strcmp(*termType, "BlankNode") == 0) {
    term = new BlankNode();
  } else if(strcmp(*termType, "NamedNode") == 0) {
    term = new NamedNode();
  } else if(strcmp(*termType, "Literal") == 0) {
    Literal* literal = new Literal();
    term = literal;
    if(object->Has(datatypeKey)) {
      Handle<Object> datatype =
        object->Get(datatypeKey.As<Handle<Object>>());
      if(!datatype.IsObject() || datatype->IsNull()) {
        Napi::ThrowError(
          "'termType' must be 'BlankNode', 'NamedNode', " \
          "'Literal', or 'DefaultGraph'.");
        return false;
      }
      Term* dataTypeTerm;
      if(!createTerm(
        dataTypeTerm, datatype,
        termTypeKey, valueKey, datatypeKey, languageKey)) {
        return false;
      }
      if(dataTypeTerm->termType != TermType::NAMED_NODE) {
        Napi::Error::New(env, "datatype 'termType' must be 'NamedNode'.").ThrowAsJavaScriptException();

        delete dataTypeTerm;
        return false;
      }
      literal->datatype = (NamedNode*)dataTypeTerm;
    }
    if(object->Has(languageKey)) {
      literal->language = *Utf8String(object->Get(languageKey));
    }
  } else if(strcmp(*termType, "DefaultGraph") == 0) {
    term = new DefaultGraph();
  } else {
    Napi::ThrowError(
      "'termType' must be 'BlankNode', 'NamedNode', " \
      "'Literal', or 'DefaultGraph'.");
    return false;
  }

  term->value = *Utf8String(object->Get(valueKey));

  return true;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  Set(
    target, New<String>("main"),
    GetFunction(New<Napi::FunctionReference>(Main)));
  Set(
    target, New<String>("mainSync"),
    GetFunction(New<Napi::FunctionReference>(MainSync)));
}

NODE_API_MODULE(addon, InitAll)
