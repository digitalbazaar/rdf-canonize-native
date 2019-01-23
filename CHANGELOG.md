# rdf-canonize-native ChangeLog

### Changed
- **BREAKING**: Remove `usePureJavaScript` option and checks. Use
  `rdf-canonize` module directly if you want to use JavaScript.

## 1.0.0 - 2019-01-20

### Notes
- **WARNING**: This release has a **BREAKING** change that could cause the
  canonical N-Quads output to differ from previous releases. Specifically, tabs
  in literals are no longer escaped. No backwards compatibility mode is
  provided at this time but if you believe it is needed, please file an issue.
- The code changed to not use C++11 functions. This now allows users of
  Travis-CI to not have to specifically upgrade the C++ compiler to use this
  module.

### Fixed
- **BREAKING**: N-Quad canonical serialized output.
  - Only escape 4 chars.
  - Now compatible with https://www.w3.org/TR/n-triples/#canonical-ntriples

### Changed
- N-Quad native serialization optimization.
  - Varies based on input by roughly ~1-5x.
- Native regex and stream use removed.
  - Simpler code more appropriate for these algorithms.
- **BREAKING**: Sync js parsing and serialization code from rdf-canonize.
  - Updates canonical serialized form.
  - Improves parsing.
  - Optimized by roughly ~1-2x.

### Removed
- **BREAKING**: Remove Node.js 6.x support.  Node.js 6.x users should use 0.3.x
  or transform the code on your own.

## 0.3.0 - 2018-11-01

### Notes
- Code initially cloned from [rdf-canonize][] to preserve the native binding
  history. Versioning will be started at 0.3.0 such that there is less
  confusion due to old release commit messages. This version will not track
  the [rdf-canonize][] versions going forward.

### Fixed
- Accept N-Quads upper case language tag.
- Improve acceptable N-Quads blank node labels.
- Missing space in named graph serialization.

### Changed
- Only build and export native implementations.

### Removed
- Pure JavaScript implementations.

### Added
- Copy git files and history from [rdf-canonize][].
- See git history for all changes.

[jsonld.js]: https://github.com/digitalbazaar/jsonld.js
[rdf-canonize]: https://github.com/digitalbazaar/rdf-canonize
