# rdf-canonize-native ChangeLog

### Fixed
- **BREAKING**: N-Quad canonical serialized output.
  - Only escape 4 chars.

### Changed
- N-Quad native serialization optimization.
  - Varies based on input by rougly ~1-5x.
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
