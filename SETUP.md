# Setup (do this before the meeting)

You need: Git, a C compiler, CMake, and an editor. About 15–20 minutes.

## Windows

Easiest: install WSL (Ubuntu), then follow the Linux steps inside it.

1. Open PowerShell as admin: `wsl --install`, then restart.
2. Open Ubuntu and follow the Linux section below.
3. Install VS Code and its "WSL" and "C/C++" extensions.

## macOS

1. `xcode-select --install` (gives you clang and git)
2. Install Homebrew from brew.sh, then `brew install cmake`
3. Install VS Code with the "C/C++" and "CMake Tools" extensions.

## Linux / WSL

```
sudo apt update
sudo apt install -y build-essential cmake git
```

## Check it works

```
git --version
cmake --version
gcc --version      # or: clang --version
```

All three should print a version number. If one fails, bring your laptop to the meeting and we'll fix it there.

## Optional, for the drone demo

- Betaflight Configurator (only the person bringing the Air65 needs it)
- A USB-C **data** cable (not charge-only)
