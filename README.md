# Car Warehouse Blockchain System

COMP2034 CW1 — A GUI application that tracks the full lifecycle of vehicles (production → warehouse → quality check → dealer dispatch → customer sale) using an immutable blockchain backed by SQLite, with SHA-256 and SHA-3 integrity verification.

---

## Features

- Blockchain ledger for car lifecycle records (5 stages per vehicle)
- SHA-256 + SHA-3 dual-hash integrity verification with tamper simulation
- SQLite persistence — data survives restarts
- Soft delete (tombstone) and restore for records
- Audit log, branch map, and fuel price manager panels
- Linked-list traversal view of the chain (data structure requirement)
- Dear ImGui dark-themed GUI

---

## Prerequisites

### Windows

| Tool | Notes |
|------|-------|
| [Visual Studio 2022](https://visualstudio.microsoft.com/) | Install the **Desktop development with C++** workload |
| [CMake 3.16+](https://cmake.org/download/) | Tick "Add to PATH" during install |
| [OpenSSL for Windows](https://slproweb.com/products/Win32OpenSSL.html) | Install the **Win64 OpenSSL v3.x** full installer to `C:\OpenSSL-Win64` |
| Git | Included with Git for Windows |

### Linux (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install -y build-essential cmake libssl-dev libgl-dev libx11-dev \
                    libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### macOS

```bash
brew install cmake openssl
```

---

## Clone & Set Up Third-Party Dependencies

> **Important:** GLFW and Dear ImGui are referenced as git submodule commits but the `.gitmodules` file is not included. You must clone them manually into the correct paths after cloning this repo.

```bash
# 1. Clone the project
git clone <your-repo-url> cw1
cd cw1

# 2. Clone GLFW into third_party/glfw
git clone https://github.com/glfw/glfw.git third_party/glfw

# 3. Clone Dear ImGui into third_party/imgui
git clone https://github.com/ocornut/imgui.git third_party/imgui
```

> SQLite and JetBrainsMono font are already bundled in the repo under `third_party/sqlite/` and `third_party/fonts/` — nothing extra needed for those.

---

## Build & Run

### Windows (Visual Studio / MSVC)

Open **Developer PowerShell for VS 2022** or a regular terminal with CMake on PATH:

```bash
# From the repo root
mkdir build
cd build

# Configure (adjust the OpenSSL path if you installed it elsewhere)
cmake .. -G "Visual Studio 17 2022" -A x64 \
         -DOPENSSL_ROOT_DIR="C:/OpenSSL-Win64"

# Build
cmake --build . --config Release

# Run (the exe is placed in build/Release/)
cd Release
./car_warehouse_gui.exe
```

If you prefer the CMake GUI: point "source" to the repo root and "build" to a new `build/` folder, click Configure, then Generate, then open the `.sln` in Visual Studio and build the `car_warehouse_gui` target.

---

### Linux

```bash
# From the repo root
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release

make -j$(nproc)

# Run
./car_warehouse_gui
```

---

### macOS

```bash
# From the repo root
mkdir build && cd build

cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DOPENSSL_ROOT_DIR="$(brew --prefix openssl)"

make -j$(nproc)

# Run
./car_warehouse_gui
```

---

## Project Structure

```
cw1/
├── src/
│   ├── main_gui.cpp               # Application entry point & ImGui loop
│   ├── blockchain/
│   │   ├── Block.cpp              # Single block: hashing, rehashing, debug tamper
│   │   ├── Blockchain.cpp         # Chain management, append, search, persistence
│   │   ├── BlockStage.cpp         # Lifecycle stage enum helpers
│   │   ├── LinkedBlockchainView.cpp  # Explicit linked-list traversal
│   │   └── Validation.cpp         # Chain integrity checker
│   ├── models/
│   │   └── CarRecord.cpp          # Vehicle payload serialisation
│   └── utils/
│       ├── AuditLog.cpp           # Append-only operation log
│       ├── BranchMap.cpp          # Manufacturer/dealer branch map
│       ├── DatabaseManager.cpp    # SQLite read/write layer
│       ├── FuelPriceManager.cpp   # Fuel price tracking panel
│       ├── HashUtil.cpp           # SHA-256 & SHA-3 wrappers (OpenSSL)
│       ├── StringUtil.cpp         # Case-insensitive search helper
│       ├── TimeUtil.cpp           # Timestamp utilities
│       └── VehicleData.cpp        # Static lookup tables
├── include/                       # Header files (mirrors src/ layout)
├── third_party/
│   ├── imgui/                     # Dear ImGui (clone manually — see above)
│   ├── glfw/                      # GLFW (clone manually — see above)
│   ├── sqlite/                    # SQLite amalgamation (bundled)
│   └── fonts/                     # JetBrainsMono-Regular.ttf (bundled)
├── database/                      # SQLite database file created at runtime
└── CMakeLists.txt
```

---

## Troubleshooting

**`Could not find OpenSSL`**
- Windows: pass `-DOPENSSL_ROOT_DIR="C:/OpenSSL-Win64"` to cmake (adjust path).
- Linux: run `sudo apt install libssl-dev`.
- macOS: run `brew install openssl` and pass `-DOPENSSL_ROOT_DIR=$(brew --prefix openssl)`.

**`third_party/glfw` or `third_party/imgui` is empty**
- Run the two `git clone` commands in the [Clone & Set Up](#clone--set-up-third-party-dependencies) section.

**Window doesn't open / OpenGL errors**
- Make sure your GPU drivers are up to date and support OpenGL 3.3+.
- On WSL2, use a native Windows build instead — WSL2 OpenGL support is limited.

**Font not found at startup**
- The CMake build step automatically copies `third_party/fonts/JetBrainsMono-Regular.ttf` next to the executable. If you move the exe, copy the `fonts/` folder with it.
