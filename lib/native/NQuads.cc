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
#include <regex>
#include <sstream>
#include <vector>

#include <iostream>

using namespace std;
using namespace RdfCanonize;

string unescape(std::string const& s);

static string RDF_LANGSTRING =
  "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString";
static string XSD_STRING = "http://www.w3.org/2001/XMLSchema#string";
static regex REGEX_BACK_SLASH("\\\\");
static regex REGEX_TAB("\\\t");
static regex REGEX_LF("\\\n");
static regex REGEX_CR("\\\r");
static regex REGEX_QUOTE("\\\"");

string NQuads::serialize(const QuadSet& quadset) {
  // serialize each quad
  vector<string> nquads;
  for(const Quad* q : quadset) {
    nquads.push_back(NQuads::serializeQuad(*q));
  }

  // sort and join nquads
  sort(nquads.begin(), nquads.end());
  ostringstream joined;
  copy(
    nquads.begin(), nquads.end(),
    ostream_iterator<string>(joined, ""));
  return joined.str();
}

string NQuads::serializeQuad(const Quad& quad) {
  Term* s = quad.subject;
  Term* p = quad.predicate;
  Term* o = quad.object;
  Term* g = quad.graph;

  // ostringstream nquad;
  string nquad;

  // subject and predicate can only be named or blank nodes, not literals
  for(Term* t : {s, p}) {
    if(t->termType == TermType::NAMED_NODE) {
      // nquad << "<" << t->value << ">";
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
    // TODO: optimize
    // string escaped = o->value;
    // escaped = regex_replace(escaped, REGEX_BACK_SLASH, "\\\\");
    // escaped = regex_replace(escaped, REGEX_TAB, "\\t");
    // escaped = regex_replace(escaped, REGEX_LF, "\\n");
    // escaped = regex_replace(escaped, REGEX_CR, "\\r");
    // escaped = regex_replace(escaped, REGEX_QUOTE, "\\\"");

    // string escaped = o->value;
    // string escaped = unescape(o->value);

    // cout << escaped;
    nquad += "\"" + unescape(o->value) + "\"";
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

string unescape(std::string const& s) {
  string res;
  string::const_iterator it = s.begin();
  while (it != s.end())
  {
    char c = *it++;
    if (c == '\\' && it != s.end())
    {
      switch (*it++) {
        case '\\': c = '\\'; break;
        case 'n': c = '\n'; break;
        case 'r': c = '\r'; break;
        case 't': c = '\t'; break;
        // all other escapes
        default:
        // invalid escape sequence - skip it. alternatively you can copy it
        // as is, throw an exception...
        continue;
      }
    }
    res += c;
  }

  return res;
}
