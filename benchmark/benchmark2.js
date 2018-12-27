/*!
 * Copyright (c) 2018 Digital Bazaar, Inc. All rights reserved.
 */
'use strict';

const canonize = require('..');
const fs = require('fs');
const jsonld = require('jsonld');
const uuid = require('uuid/v4');
const Benchmark = require('benchmark');

const suite = new Benchmark.Suite();

const sampleData = fs.readFileSync('./test-recipe.jsonld');

const documents = [];
const nquads = [];

const numDocuments = 1000;
const numIngredients = 10;

const run = async () => {
  for(let n = 0; n < numDocuments; ++n) {
    const d = JSON.parse(sampleData);
    for(let i = 0; i < numIngredients; ++i) {
      d.ingredient.push(uuid());
    }
    documents.push(d);
    const nquad = await jsonld.toRDF(d);
    nquads.push(nquad);
  }
};

async function a() {
  for(const n of nquads) {
    await canonize.canonize(n, {
      algorithm: 'URDNA2015',
      inputFormat: 'application/nquads',
      format: 'application/nquads',
      usePureJavaScript: false,
    });
  }
}

function b() {
  for(const n of nquads) {
    canonize.canonizeSync(n, {
      algorithm: 'URDNA2015',
      inputFormat: 'application/nquads',
      format: 'application/nquads',
      usePureJavaScript: false,
    });
  }
}

run().then(() => {
  return new Promise(resolve => {
    suite
      .add('Async', {
        name: 'Async',
        defer: true,
        fn: function(deferred) {
          a().then(() => deferred.resolve());
        }
      })
      .add('Sync', {
        name: 'Sync',
        false: true,
        fn: b
      })
      .on('start', () => {
        console.log('Benchmarking...');
      })
      .on('cycle', event => {
        console.log(String(event.target));
      })
      .on('complete', () => {
        console.log('Done.');
        resolve();
      })
      .run({async: true});
  });
}).catch(e => {
  console.error(e);
  process.exit(1);
});
