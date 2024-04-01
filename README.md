# Astroventure

![stability-wip](https://img.shields.io/badge/Stability-WIP-lightgrey.svg)
![Linux status](https://img.shields.io/badge/Linux-Supported-navajowhite)
![Windows status](https://img.shields.io/badge/Windows-Supported-lightblue)
![Android status](https://img.shields.io/badge/Android-WIP-lightgreen)

<a href="https://www.youtube.com/watch?v=x9qBZVg9VeQ">
<img src="https://i.imgur.com/EBhhix8.png" alt="Astroventure demo - YouTube" width="25%" height="25%">
<img src="https://i.imgur.com/VUuHgo4.png" alt="Astroventure demo - YouTube" width="25%" height="25%">
</a>

Cross-platform Low Poly Space Game written in C++20.

## Dependencies

1. Clone this project with submodules, i.e.
   ```bash
   git submodule update --init --recursive
   ```
2. Clone [algine](https://github.com/congard/algine)

> [!NOTE]
> You may also need to install additional libraries via your
> package manager depending on your distro.

### Fedora

```bash
sudo dnf install alsa-lib-devel
```

### Debian

```bash
sudo apt install libasound2-dev
```

## Structure

```
src         project sources
resources   project resources, i.e. audio, fonts, icons etc.
```

> [!NOTE]
> In order to launch the game on Linux,
> working directory must be set to `resources`.

## Toolchain

The recommended tools are listed in the table below.

| Description  | Tool    |
|--------------|---------|
| C compiler   | clang   |
| C++ compiler | clang++ |
| Debugger     | lldb    |
| Linker       | mold    |

## CMake options

| Option       | Description    | Required |
|--------------|----------------|----------|
| `algine_dir` | path to algine | Yes      |
