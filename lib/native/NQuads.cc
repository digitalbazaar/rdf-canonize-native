/*********************************************************************
 * rdf-canonize urdna2015 for Node.js.
 *
 * Copyright (c) 2017 Digital Bazaar, Inc.
 *
 * BSD License
 * <https://github.com/digitalbazaar/rdf-canonize/blob/master/LICENSE>
 ********************************************************************/

#include "NQuads.h"
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

using namespace std;
using namespace RdfCanonize;

static string escape(std::string const& s);

static string RDF_LANGSTRING =
  "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString";
static string XSD_STRING = "http://www.w3.org/2001/XMLSchema#string";

string NQuads::serialize(const QuadSet& quadset) {
  // serialize each quad
  vector<string> nquads;
  // track size to allocate output string
  size_t size = 0;
  for(const Quad* q : quadset) {
    const string nquad = NQuads::serializeQuad(*q);
    nquads.push_back(nquad);
    size += nquad.size();
  }

  // sort and join nquads
  sort(nquads.begin(), nquads.end());
  string joined;
  joined.reserve(size);
  for(vector<string>::const_iterator i = nquads.begin();
    i != nquads.end();
    ++i) {
    joined += *i;
  }
  return joined;
}

string NQuads::serializeQuad(const Quad& quad) {
  Term* s = quad.subject;
  Term* p = quad.predicate;
  Term* o = quad.object;
  Term* g = quad.graph;

  string nquad;
  // TODO: optimization: estimate and reserve output size
  //nquad.reserve(...);

  // subject and predicate can only be named or blank nodes, not literals
  for(Term* t : {s, p}) {
    if(t->termType == TermType::NAMED_NODE) {
      nquad += "<" + t->value + ">";
    } else {
      nquad += t->value;
    }
    nquad += " ";
  }

  // object is named or blank node or literal
  if(o->termType == TermType::NAMED_NODE) {
    nquad += "<" + o->value + ">";
  } else if(o->termType == TermType::BLANK_NODE) {
    nquad += o->value;
  } else {
    Literal* literal = (Literal*)o;
    nquad += "\"" + escape(o->value) + "\"";
    if(literal->datatype != NULL) {
      if(literal->datatype->value == RDF_LANGSTRING) {
        if(literal->language.size() != 0) {
          nquad += "@" + literal->language;
        }
      } else if(literal->datatype->value != XSD_STRING) {
        nquad += "^^<" + literal->datatype->value + ">";
      }
    }
  }

  // graph can only be a NamedNode or a BlankNode (or DefaultGraph, but that
  // does not add to the `nquad`), not a literal
  if(g->termType == TermType::NAMED_NODE) {
    nquad += " <" + g->value + ">";
  } else if(g->termType == TermType::BLANK_NODE) {
    nquad += " " + g->value;
  }

  nquad += " .\n";
  return nquad;
}

// TODO: optimize
string escape(std::string const& s) {
  string res;
  // start with original size
  res.reserve(s.size());

  for(string::const_iterator it = s.begin(); it != s.end(); ++it) {
    char c = *it;
    switch (c) {
      case '"': res += "\\\""; break;
      case '\\': res += "\\\\"; break;
      case '\n': res += "\\n"; break;
      case '\r': res += "\\r"; break;
      // all other chars
      default: res += c;
    }
  }

  return res;
}
