# rdf-canonize-native ChangeLog

## 0.3.0 - 2018-10-01

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
