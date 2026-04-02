# Car Warehouse Blockchain System

COMP2034 CW1 — A GUI application that tracks the full lifecycle of vehicles (production, warehouse intake, quality check, dealer dispatch, customer sale) using an immutable blockchain backed by SQLite, with SHA-256 and SHA-3 integrity verification.

---

## Features

- Blockchain ledger for car lifecycle records (5 supply-chain stages)
- SHA-256 + SHA-3 dual-hash integrity verification with tamper simulation
- SQLite persistence — data survives restarts
- Role-based access control (Admin, Warehouse Staff, QC Inspector, Dealer, Auditor)
- Pending approval workflow for block submissions
- Soft delete (tombstone) and restore for records
- Fuel price intelligence module with forecasting
- Audit log and linked-list traversal view of the chain
- AI Assistant chatbot (Gemini API) with Online and Project modes
- Dear ImGui dark-themed GUI (OpenGL 3.3 + GLFW)

---

## Step-by-Step Setup (Windows, from scratch)

This guide assumes you have a fresh Windows PC with nothing installed — just follow each step in order.

### Step 1 — Install VSCode

1. Go to https://code.visualstudio.com/
2. Download and install **Visual Studio Code**
3. Open it once to make sure it works, then close it

### Step 2 — Install Git

1. Go to https://git-scm.com/download/win
2. Download and run the installer
3. Use all default options, just keep clicking **Next** then **Install**
4. Verify: open a new terminal (Win+R, type `cmd`, press Enter) and run:
   ```
   git --version
   ```
   You should see something like `git version 2.x.x`

### Step 3 — Install MSYS2 (C++ compiler + build tools)

MSYS2 gives you the C++ compiler, CMake, and OpenSSL — everything needed to build this project.

1. Go to https://www.msys2.org/
2. Download the installer (the big button at the top)
3. Run the installer — install to the default path `C:\msys64`
4. When it finishes, it opens an MSYS2 terminal. In that terminal, run these commands one by one:

   ```
   pacman -Syu
   ```
   (If it asks to close the terminal, close it, reopen **MSYS2 UCRT64** from the Start Menu, then run `pacman -Syu` again.)

   ```
   pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-openssl
   ```

5. Close the MSYS2 terminal — you will NOT need it again. Everything else is done from the VSCode terminal.

### Step 4 — Add MSYS2 to your system PATH

This lets the compiler and CMake work from any terminal (VSCode, CMD, PowerShell).

1. Press **Win + S**, search for **"Environment Variables"**, click **"Edit the system environment variables"**
2. Click the **"Environment Variables..."** button at the bottom
3. Under **System variables** (bottom section), find and select **Path**, then click **Edit**
4. Click **New** and add this path:
   ```
   C:\msys64\mingw64\bin
   ```
5. Click **OK** on all dialogs to save
6. **Close and reopen** any open terminals for the change to take effect
7. Verify: open a new CMD or PowerShell and run:
   ```
   gcc --version
   cmake --version
   mingw32-make --version
   ```
   All three should print version info without errors.

### Step 5 — Clone the project

Open a terminal (CMD, PowerShell, or VSCode terminal) and run:

```
git clone <your-repo-url> cw1
cd cw1
```

### Step 6 — Set up third-party dependencies

GLFW and Dear ImGui need to be cloned into the `third_party/` folder. SQLite and fonts are already bundled.

```
git clone https://github.com/glfw/glfw.git third_party/glfw
git clone https://github.com/ocornut/imgui.git third_party/imgui
```

### Step 7 — Set up the AI chatbot API key (optional)

The AI Assistant feature requires a free Gemini API key. Skip this step if you do not need the chatbot.

1. Go to https://aistudio.google.com/apikey
2. Sign in with a Google account and create an API key
3. In the project root, create a folder and file:
   ```
   mkdir env
   ```
4. Create the file `env/gemini_api_key.txt` and paste your API key inside (just the key, nothing else)

The chatbot works without this — you just cannot use the AI Assistant panel.

### Step 8 — Build the project

From the project root folder:

```
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j4
```

This compiles everything and produces `car_warehouse_gui.exe` inside the `build/` folder.

### Step 9 — Run the application

From the `build/` folder:

```
./car_warehouse_gui.exe
```

Or double-click `car_warehouse_gui.exe` in File Explorer.

Default login credentials:
| Username | Password | Role |
|----------|----------|------|
| admin | admin123 | Admin (full access) |
| warehouse1 | pass123 | Warehouse Staff |
| qc1 | pass123 | QC Inspector |
| dealer1 | pass123 | Dealer |
| auditor1 | pass123 | Auditor (read-only) |

---

## Opening in VSCode

1. Open VSCode
2. Go to **File > Open Folder** and select the `cw1` project folder
3. Open the integrated terminal: **Terminal > New Terminal** (or press Ctrl+`)
4. Build and run from there using the same commands:
   ```
   cd build
   mingw32-make -j4
   ./car_warehouse_gui.exe
   ```

If this is the first time building, run the full build steps from Step 8 first.

---

## Rebuilding after code changes

You only need to run `cmake ..` once. After editing source files, just:

```
cd build
mingw32-make -j4
```

If `car_warehouse_gui.exe` is still running, close it first or you will get a "Permission denied" error.

---

## Project Structure

```
cw1/
├── src/
│   ├── main_gui.cpp                  # Application entry point & ImGui loop
│   ├── ai/
│   │   └── ChatbotService.cpp        # AI chatbot (Gemini API via WinHTTP)
│   ├── blockchain/
│   │   ├── Block.cpp                 # Single block: hashing, rehashing
│   │   ├── Blockchain.cpp            # Chain management, search, persistence
│   │   ├── BlockStage.cpp            # Lifecycle stage enum helpers
│   │   ├── LinkedBlockchainView.cpp  # Linked-list traversal view
│   │   └── Validation.cpp            # Chain integrity checker
│   ├── models/
│   │   └── CarRecord.cpp             # Vehicle payload serialisation
│   ├── security/
│   │   ├── Auth.cpp                  # Login / authentication
│   │   ├── AccessControl.cpp         # Role-based permission checks
│   │   ├── PendingApprovalManager.cpp# Approval workflow
│   │   └── SecurityUtil.cpp          # Password hashing
│   └── utils/
│       ├── AuditLog.cpp              # Append-only operation log
│       ├── BranchMap.cpp             # Manufacturer/dealer branch map
│       ├── DatabaseManager.cpp       # SQLite read/write layer
│       ├── FuelPriceManager.cpp      # Fuel price tracking & forecasting
│       ├── HashUtil.cpp              # SHA-256 & SHA-3 wrappers (OpenSSL)
│       ├── StringUtil.cpp            # Case-insensitive search helper
│       ├── TimeUtil.cpp              # Timestamp utilities
│       └── VehicleData.cpp           # Static lookup tables
├── include/                          # Header files (mirrors src/ layout)
├── third_party/
│   ├── imgui/                        # Dear ImGui (clone manually)
│   ├── glfw/                         # GLFW (clone manually)
│   ├── sqlite/                       # SQLite amalgamation (bundled)
│   └── fonts/                        # JetBrainsMono-Regular.ttf (bundled)
├── env/                              # API key file (not committed to git)
├── database/                         # SQLite DB file created at runtime
├── CMakeLists.txt                    # Build configuration
└── README.md
```

---

## Troubleshooting

**`gcc` / `cmake` / `mingw32-make` is not recognized**
- Make sure `C:\msys64\mingw64\bin` is in your system PATH (Step 4)
- Close and reopen your terminal after editing PATH

**`Could not find OpenSSL`**
- Run this in the MSYS2 terminal: `pacman -S mingw-w64-x86_64-openssl`
- Make sure you are using `-G "MinGW Makefiles"` in the cmake command

**`Permission denied` when building**
- Close the running `car_warehouse_gui.exe` before rebuilding

**`third_party/glfw` or `third_party/imgui` is empty**
- Run the two `git clone` commands from Step 6

**Window does not open / OpenGL errors**
- Make sure your GPU drivers are up to date and support OpenGL 3.3+

**AI Assistant says "Missing API Key"**
- Create `env/gemini_api_key.txt` with a valid Gemini API key (Step 7)

**Font not found at startup**
- The build automatically copies `third_party/fonts/JetBrainsMono-Regular.ttf` next to the exe. If you move the exe, copy the `fonts/` folder with it.
