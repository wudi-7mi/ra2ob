# Ra2ob

An Ra2 observer deeply inspired by [ra2viewer](https://github.com/chenguokai/ra2viewer)

## Quick Start

Run `ra2ob.exe` as Administrator.

## Todos

- [ ] Add Documents.
- [x] Add Producing List.
- [ ] Add Network Module.

## Develop

1. Clone the repository

2. CMake

```shell
mkdir build
cd build
cmake ..
```

3. Develop with your tools

If you're using Visual Studio, after `Step2` you can open `ra2ob.sln` project. Notice that you need to set ra2ob as start up project (see [Set as Startup Project](https://learn.microsoft.com/en-us/visualstudio/get-started/csharp/run-program?view=vs-2022#start-from-a-project)); Also, set `MANIFESTUAC` to `requireAdministrator` and `Working Folder` to `$(ProjectDir)..` before building this project.

If you're using Make, just run `make` in `./build` after `Step2` and run the executable file as Administrator.

4. Commit

This project uses pre-commit. Install and configure pre-commit:

```shell
pip install pipx cpplint pre-commit
pipx install clang-format
pre-commit install
```
