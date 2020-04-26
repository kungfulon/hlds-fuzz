# hlds-fuzz

Harness for fuzzing GoldSrc file format with WinAFL. Currently supports BSP/MDL/SPR.

## Building

Open the solution in VS 2017 or higher and build. Make sure to clone `hlsdk` submodule.

## Running

After building, copy `hlds-fuzz.exe` to HLDS folder. Rename `swds.dll` to `sw.dll`. `hlds-fuzz.exe` will receive two parameters:

- Input file name (relative to mod directory, default is `valve`, can be changed by adding `-game <mod name here>` to command line in `hlds-fuzz.cpp`)
- Server port (for parallel fuzzing)

Example:

```
afl-fuzz -i fuzz_in -o fuzz_out -D C:\dynamorio\bin32 -f valve/fuzz -t 1000 -M master -- -coverage_module sw.dll -target_module hlds-fuzz.exe -target_offset 0x1190 -nargs 0 -fuzz_iterations 5000 -- hlds-fuzz.exe fuzz 27015
```

Since the input file that we feed to the process is `fuzz`, we must configure `-f` flag of WinAFL to point to `valve/fuzz`.

## License

This project is licensed under MIT license.
