# Watcher -  File Integrity Checker 

C++ file integrity checker using SHA-256 hashing. This program scans directories recursively, calculates file hashes, and maintains a database for future comparisons. It supports two modes:

- `Create`: Initializes the hash database for a specified directory.
- `Check` : Verifies the integrity of files against the stored database.


## Key features:

- Uses SHA-256 for robust file hash calculation
- Supports two modes:

  * `create` - Generate an initial hash database
  * `check` - Verify file system integrity against the stored database


- Detects:

  - File modifications
  - New files added
  - Files deleted


- Recursively scans directories
- Stores relative file paths for portability

## Dependencies
You'll need to install OpenSSL development libraries. On different systems:

- Ubuntu/Debian: sudo apt-get install libssl-dev
- macOS (Homebrew): brew install openssl
- Windows: Use appropriate OpenSSL development package

## Project structure:
```
project/
├── CMakeLists.txt
├── Watcher.cpp
└── build/
```

### Build commands:

```bash
mkdir build && cd build
cmake ..
make
```

## How It Works

### In create mode:

- Scans the specified directory recursively
- Calculates SHA-256 hash for each file
- Saves hashes to a database file


### In check mode:

- Loads previous hash database
- Recalculates current file hashes
- Compares with stored hashes
- Reports any discrepancies

## Limitations:
  - Does not handle very large files with constant memory consumption
  - No support for file exclusion patterns


