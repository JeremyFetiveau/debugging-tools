# v8 runtime helper

The file `runtime-doare-helpers.cc` is a C++ runtime function for [v8](https://v8.dev/) that exposes custom debugging functions to javascript.

Runtime functions can be called using the native syntax `%FunctionName`.

You need to enable this syntax using the flag `--allow-natives-syntax` in order to make in work in the [d8](https://cs.chromium.org/chromium/src/v8/src/d8.cc) shell.

If you want to pass flags to v8 from the browser, you also need to use the flag `--js-flags`.

Be aware that the code is not safe to use and is only a debugging helper.
Many runtime functions are not safe to call directly from javascript.

## Build

The helpers are implemented in `runtime-doare-helpers.cc`. 

You need to add it to `BUILD.gn` so that it will build and also declare your runtime function (implemented using the `RUNTIME_FUNCTION` macro) in the file `src/runtime/runtime.h`.

Long story short : 
- Apply `runtime.diff`
- Copy `runtime-doare-helpers.cc` to the `src/runtime/` folder.

## Examples

### `%DumpObjects()`

Usage is the following : 

* `%DumpObjects(<jsObject>, <lines|0>)`
* `%DumpObjects(<objectAddress|0>, <lines|1>)`
* `%DumpObjects(<objectAddress|1>, <lines|1>)`

This example shows how to examine memory of a WebAssembly exported function and explore it so as to find a [pointer to WASM RWX memory](https://abiondo.me/2019/01/02/exploiting-math-expm1-v8/#code-execution).

```
d8> load("sample_wasm.js")
d8> %DumpObjects(global_test,10)
----- [ JS_FUNCTION_TYPE : 0x38 ] -----
0x00002fac7911ed10    0x00001024ebc84191    MAP_TYPE    
0x00002fac7911ed18    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
0x00002fac7911ed20    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
0x00002fac7911ed28    0x00002fac7911ecd9    SHARED_FUNCTION_INFO_TYPE    
0x00002fac7911ed30    0x00002fac79101741    NATIVE_CONTEXT_TYPE    
0x00002fac7911ed38    0x00000d1caca00691    FEEDBACK_CELL_TYPE    
0x00002fac7911ed40    0x00002dc28a002001    CODE_TYPE    
----- [ TRANSITION_ARRAY_TYPE : 0x30 ] -----
0x00002fac7911ed48    0x00000cdfc0080b69    MAP_TYPE    
0x00002fac7911ed50    0x0000000400000000    
0x00002fac7911ed58    0x0000000000000000    
function 1() { [native code] }
d8> %DumpObjects(0x00002fac7911ecd9,11)
----- [ SHARED_FUNCTION_INFO_TYPE : 0x38 ] -----
0x00002fac7911ecd8    0x00000cdfc0080989    MAP_TYPE    
0x00002fac7911ece0    0x00002fac7911ecb1    WASM_EXPORTED_FUNCTION_DATA_TYPE    
0x00002fac7911ece8    0x00000cdfc00842c1    ONE_BYTE_INTERNALIZED_STRING_TYPE    
0x00002fac7911ecf0    0x00000cdfc0082ad1    FEEDBACK_METADATA_TYPE    
0x00002fac7911ecf8    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911ed00    0x000000000000004f    
0x00002fac7911ed08    0x000000000000ff00    
----- [ JS_FUNCTION_TYPE : 0x38 ] -----
0x00002fac7911ed10    0x00001024ebc84191    MAP_TYPE    
0x00002fac7911ed18    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
0x00002fac7911ed20    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
0x00002fac7911ed28    0x00002fac7911ecd9    SHARED_FUNCTION_INFO_TYPE    
52417812098265
d8> %DumpObjects(0x00002fac7911ecb1,11)
----- [ WASM_EXPORTED_FUNCTION_DATA_TYPE : 0x28 ] -----
0x00002fac7911ecb0    0x00000cdfc00857a9    MAP_TYPE    
0x00002fac7911ecb8    0x00002dc28a002001    CODE_TYPE    
0x00002fac7911ecc0    0x00002fac7911eb29    WASM_INSTANCE_TYPE    
0x00002fac7911ecc8    0x0000000000000000    
0x00002fac7911ecd0    0x0000000100000000    
----- [ SHARED_FUNCTION_INFO_TYPE : 0x38 ] -----
0x00002fac7911ecd8    0x00000cdfc0080989    MAP_TYPE    
0x00002fac7911ece0    0x00002fac7911ecb1    WASM_EXPORTED_FUNCTION_DATA_TYPE    
0x00002fac7911ece8    0x00000cdfc00842c1    ONE_BYTE_INTERNALIZED_STRING_TYPE    
0x00002fac7911ecf0    0x00000cdfc0082ad1    FEEDBACK_METADATA_TYPE    
0x00002fac7911ecf8    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911ed00    0x000000000000004f    
52417812098225
d8> %DumpObjects(0x00002fac7911eb29,41)
----- [ WASM_INSTANCE_TYPE : 0x118 : REFERENCES RWX MEMORY] -----
0x00002fac7911eb28    0x00001024ebc89411    MAP_TYPE    
0x00002fac7911eb30    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
0x00002fac7911eb38    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
0x00002fac7911eb40    0x00002073d820bac1    WASM_MODULE_TYPE    
0x00002fac7911eb48    0x00002073d820bcf1    JS_OBJECT_TYPE    
0x00002fac7911eb50    0x00002fac79101741    NATIVE_CONTEXT_TYPE    
0x00002fac7911eb58    0x00002fac7911ec59    WASM_MEMORY_TYPE    
0x00002fac7911eb60    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911eb68    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911eb70    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911eb78    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911eb80    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911eb88    0x00002073d820bc79    FIXED_ARRAY_TYPE    
0x00002fac7911eb90    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911eb98    0x00002073d820bc69    FOREIGN_TYPE    
0x00002fac7911eba0    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911eba8    0x00000cdfc00804c9    ODDBALL_TYPE    
0x00002fac7911ebb0    0x00000cdfc00801d1    ODDBALL_TYPE    
0x00002fac7911ebb8    0x00002dc289f94d21    CODE_TYPE    
0x00002fac7911ebc0    0x0000000000000000    
0x00002fac7911ebc8    0x00007f9f9cf60000    
0x00002fac7911ebd0    0x0000000000010000    
0x00002fac7911ebd8    0x000000000000ffff    
0x00002fac7911ebe0    0x0000556b3a3e0c00    
0x00002fac7911ebe8    0x0000556b3a3ea630    
0x00002fac7911ebf0    0x0000556b3a3ea620    
0x00002fac7911ebf8    0x0000556b3a47c210    
0x00002fac7911ec00    0x0000000000000000    
0x00002fac7911ec08    0x0000556b3a47c230    
0x00002fac7911ec10    0x0000000000000000    
0x00002fac7911ec18    0x0000000000000000    
0x00002fac7911ec20    0x0000087e7c50a000    JumpTableStart [RWX]
0x00002fac7911ec28    0x0000556b3a47c250    
0x00002fac7911ec30    0x0000556b3a47afa0    
0x00002fac7911ec38    0x0000556b3a47afc0    
----- [ TUPLE2_TYPE : 0x18 ] -----
0x00002fac7911ec40    0x00000cdfc00827c9    MAP_TYPE    
0x00002fac7911ec48    0x00002fac7911eb29    WASM_INSTANCE_TYPE    
0x00002fac7911ec50    0x00002073d820b849    JS_FUNCTION_TYPE    
----- [ WASM_MEMORY_TYPE : 0x30 ] -----
0x00002fac7911ec58    0x00001024ebc89e11    MAP_TYPE    
0x00002fac7911ec60    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
0x00002fac7911ec68    0x00000cdfc0080c19    FIXED_ARRAY_TYPE    
52417812097833
```
