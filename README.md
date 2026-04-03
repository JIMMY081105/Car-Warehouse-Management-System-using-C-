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

## How to Run (Windows)

There are **two options**. Choose whichever is easier for you.

| | Option A — Quick Start | Option B — Build from Source |
|---|---|---|
| Install anything? | No | Yes (MSYS2) |
| Time needed | ~10 seconds | ~10 minutes |
| What you run | Pre-built `.exe` with bundled DLLs | Compile the source code yourself |

---

### Option A — Quick Start (no install needed)

The `prebuilt/` folder contains the compiled executable and all required DLLs. No compiler, no MSYS2, no CMake needed.

1. Open the `prebuilt/` folder in File Explorer
2. Double-click **`car_warehouse_gui.exe`**
3. That's it — the application should open

> **Note:** The `prebuilt/` folder must stay inside the project folder (next to `CMakeLists.txt`, `src/`, `env/`, etc.) because the application looks for `env/gemini_api_key.txt` and `database/` relative to the project root.

If you get a Windows SmartScreen warning, click **"More info"** then **"Run anyway"**.

---

### Option B — Build from Source

If you want to compile the project yourself, follow the steps below. All source code, third-party libraries, fonts, and the API key are included in the submitted files. You only need to install the build tools.

#### Step 1 — Install MSYS2 (C++ compiler + build tools)

1. Go to https://www.msys2.org/
2. Download the installer (the big button at the top)
3. Run the installer — install to the default path `C:\msys64`
4. When it finishes, it opens an MSYS2 terminal. In that terminal, run these commands one by one:

   ```
   pacman -Syu
   ```
   (If it asks to close the terminal, close it, reopen **MSYS2 UCRT64** from the Start Menu, then run `pacman -Syu` again.)

   Then install the required packages:
   ```
   pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-openssl
   ```

   > **Tip:** Ctrl+C / Ctrl+V may not work in the MSYS2 terminal. If you cannot paste, **right-click** inside the terminal window and select **Paste**, or click the terminal's title bar icon (top-left) > **Edit > Paste**.

5. Close the MSYS2 terminal — you will NOT need it again. Everything else is done from the VSCode terminal or CMD.

#### Step 2 — Add MSYS2 to your system PATH

This lets the compiler and CMake work from any terminal (VSCode, CMD, PowerShell).

1. Press **Win + S**, search for **"Environment Variables"**, click **"Edit the system environment variables"**
2. Click the **"Environment Variables..."** button at the bottom
3. Under **System variables** (bottom section), find and select **Path**, then click **Edit**
4. Click **New** and add this path:
   ```
   C:\msys64\mingw64\bin
   ```
5. **Important:** If you have an older MinGW or other C++ compiler already installed, make sure `C:\msys64\mingw64\bin` is **above** it in the list. Select the entry and click **Move Up** until it is at or near the top. This ensures the correct version of `gcc`, `cmake`, and `mingw32-make` is used.
6. Click **OK** on all dialogs to save
7. **Close and reopen** any open terminals for the change to take effect
8. Verify: open a new CMD or PowerShell and run:
   ```
   gcc --version
   cmake --version
   mingw32-make --version
   ```
   - `gcc` should show version **14.x or higher** (not 6.x)
   - `cmake` should show version **3.x or higher**
   - `mingw32-make` should show **Built for x86_64-w64-mingw32** (not i686)

   If `gcc` still shows an old version or `cmake` is not recognized, the MSYS2 path is not at the top. Either move it up, or as a quick fix run one of these before building:

   **CMD (Command Prompt):**
   ```
   set PATH=C:\msys64\mingw64\bin;%PATH%
   ```

   **PowerShell (default VSCode terminal):**
   ```
   $env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
   ```

#### Step 3 — Build the project

1. Open the project folder in VSCode (**File > Open Folder** and select the extracted folder)
2. Open the terminal: **Terminal > New Terminal** (or press ``Ctrl+` ``)
3. **Make sure the terminal is in the project root** — it should show the folder that contains `CMakeLists.txt`, `src/`, `include/`, etc. If you are unsure, run `dir` and check that you see `CMakeLists.txt` in the output.

4. If `cmake` is not recognized, run this first:

   **PowerShell (default VSCode terminal):**
   ```
   $env:PATH = "C:\msys64\mingw64\bin;" + $env:PATH
   ```
   **CMD:**
   ```
   set PATH=C:\msys64\mingw64\bin;%PATH%
   ```

5. Then build:
   ```
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles"
   mingw32-make -j4
   ```

This compiles everything and produces `car_warehouse_gui.exe` inside the `build/` folder.

> **Do not run these commands from inside the `build/` folder.** You must start from the project root (the folder with `CMakeLists.txt`). If you accidentally created a `build/build/` folder, delete `build/` and start again from the project root.

#### Step 4 — Run the application

Still in the `build/` folder, run:

```
.\car_warehouse_gui.exe
```

Or double-click `car_warehouse_gui.exe` in File Explorer.

---

## Default Login Credentials

| Username | Password | Role |
|----------|----------|------|
| admin01 | admin123 | Admin (full access) |
| staff01 | staff123 | Warehouse Staff |
| qc01 | qc123 | QC Inspector |
| dealer01 | dealer123 | Dealer |
| auditor01 | audit123 | Auditor (read-only) |

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
│   ├── main_gui.cpp                  # Application entry point, startup, and frame orchestration
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
│   ├── ui/
│   │   ├── GuiShared.cpp             # Shared GUI state, styling, and helper utilities
│   │   ├── Layout.cpp                # Header, sidebar, and toast rendering
│   │   ├── DashboardPanel.cpp        # Dashboard analytics and summary widgets
│   │   ├── VehiclePanels.cpp         # Car detail and add-block workflow
│   │   ├── ChainPanels.cpp           # Global chain, integrity, audit, and delete/restore panels
│   │   ├── AuthPanels.cpp            # Login screen and pending approvals UI
│   │   ├── AIAssistantPanel.cpp      # Read-only AI assistant panel
│   │   └── ReportExporter.cpp        # PDF report export
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
│   └── ui/
│       └── GuiApp.hpp                # Shared UI declarations and panel interfaces
├── third_party/
│   ├── imgui/                        # Dear ImGui
│   ├── glfw/                         # GLFW
│   ├── sqlite/                       # SQLite amalgamation
│   └── fonts/                        # JetBrainsMono-Regular.ttf
├── prebuilt/                         # Pre-built exe + DLLs (Option A)
│   ├── car_warehouse_gui.exe
│   ├── libcrypto-3-x64.dll
│   ├── libgcc_s_seh-1.dll
│   ├── libstdc++-6.dll
│   ├── libwinpthread-1.dll
│   ├── env/gemini_api_key.txt
│   └── fonts/JetBrainsMono-Regular.ttf
├── env/                              # Gemini API key
├── database/                         # SQLite DB files (created at runtime)
├── reports/                          # Exported PDF reports (created at runtime)
├── CMakeLists.txt                    # Build configuration
└── README.md
```

---

## Troubleshooting

**`gcc` / `cmake` / `mingw32-make` is not recognized**
- Make sure `C:\msys64\mingw64\bin` is in your system PATH (Step 2)
- Close and reopen your terminal after editing PATH

**`Could not find OpenSSL`**
- Run this in the MSYS2 terminal: `pacman -S mingw-w64-x86_64-openssl`
- Make sure you are using `-G "MinGW Makefiles"` in the cmake command

**`Permission denied` when building**
- Close the running `car_warehouse_gui.exe` before rebuilding

**Window does not open / OpenGL errors**
- Make sure your GPU drivers are up to date and support OpenGL 3.3+

**Font not found at startup**
- The build automatically copies `third_party/fonts/JetBrainsMono-Regular.ttf` next to the exe. If you move the exe, copy the `fonts/` folder with it.

**Windows SmartScreen blocks the exe**
- Click **"More info"** then **"Run anyway"** — this is normal for unsigned executables.
