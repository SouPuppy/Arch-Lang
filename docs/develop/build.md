# Building Document for Developing

It is strongly recommended to use Visual Studio Code as your development environment. All following instructions assume you use the [CMake Tools] extension and the provided editor configuration.

## Prerequisites

Make sure you have installed:
- [CMake Tools] (Microsoft)

## Setup VS Code Configuration

To apply the predefined build settings, create a symbolic link from the provided `.vscode` configuration. This will ensure that your project uses the same build presets and workflow described in this guide.

**Linux / macOS**

```bash
ln -s ./config/editor/.vscode .vscode
```

**Windows PowerShell**

```
mklink .vscode config\editor\.vscode
```

> You must run Command Prompt as Administrator to create a directory symbolic link on Windows. If `mklink` fails, you can manually copy the folder instead.

## Configure Path Environment

**Linux / macOS**

```
export PATH="$PWD/.arch/bin:$PATH"
```

**Windows PowerShell**

```
$env:Path = "$PWD\.arch\bin;$env:Path"
```

## Build with [CMake Tools]

1. Open the Command Palette:

   Press `Ctrl + Shift + P` and search for:

   ```
   CMake: Select Configure Preset
   ```

2. Select the desired build preset (e.g., release, dev, or debug).

3. Run the following CMake commands from the palette:

   ```
   CMake: Configure
   CMake: Build
   CMake: Install
   ```

You can also use the CMake Tools panel run more commands.

## Develop VS Code Extension

Press `f5` to launch the VS Code extension project under `./syntax/arch-vscode`

[CMake Tools]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools