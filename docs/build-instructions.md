# Build instructions

## Dependencies

### Installing dependencies

You'll need:
- `cmake`
- `gcc`, `g++`
- `libiconv`
- `libboost-iostreams-dev`
- `libboost-program-options`
- `zlib`


### MSYS2 (MingW64) on Windows
Download and follow the install instructions for [MSYS2](https://www.msys2.org/)
  - if you have [Chocolatey](chocolatey.org/) installed you can install MSYS2 as follows

    `choco install msys2`

After installing start a MSYS2-shell from `<MSYS2-directory>/msys2_shell.cmd`

Update the preinstalled packages: `pacman -Syuu`

Install gcc: `pacman -Syu --needed mingw-w64-x86_64-gcc `

And these dependencies:
`pacman -Syu --needed mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-libiconv mingw-w64-x86_64-boost mingw-w64-x86_64-zlib`

### WSL (Ubuntu) on Windows

Please [install WSL on your Windows 10 System](https://docs.microsoft.com/en-us/windows/wsl/install-win10), install the Ubuntu distribution from the Microsoft Store and continue with [Ubuntu](#ubuntu)

### Ubuntu

```sh
sudo apt update 
sudo apt install --fix-missing gcc g++ cmake \
libboost-iostreams-dev libboost-program-options-dev \
zlib1g-dev
```

## Compiling

Inside the `repository root` use the following commands:

```sh
./patchGenieutils.sh
mkdir build
cd build
cmake ..
// Note: if you are using MingW64 use the following command instead
// cmake -G "CodeBlocks - MinGW Makefiles" .. 
// and continue with 
cmake --build .
```




You should now have an executable `create-data-mod` in the `build` folder. Hooray!
