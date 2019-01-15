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

### `DumpObjects(<js_object>,<lines>)`

```
$ d8 --allow-natives-syntax
d8> let a = new Array(1,2,3,4); let b = new Array(5,6,7,8);
undefined
d8> %DumpObjects(a,30)
----- [ JS_ARRAY_TYPE : 0x20 ] -----
0x00001f36de20aef0    0x00003a3eb2782bb1    MAP_TYPE    
0x00001f36de20aef8    0x000037d2ea380c19    FIXED_ARRAY_TYPE    
0x00001f36de20af00    0x00001f36de20af21    FIXED_ARRAY_TYPE    
0x00001f36de20af08    0x0000000400000000    
----- [ ALLOCATION_MEMENTO_TYPE : 0x10 ] -----
0x00001f36de20af10    0x000037d2ea385209    MAP_TYPE    
0x00001f36de20af18    0x00003f471929db69    ALLOCATION_SITE_TYPE    
----- [ FIXED_ARRAY_TYPE : 0x30 ] -----
0x00001f36de20af20    0x000037d2ea3807a9    MAP_TYPE    
0x00001f36de20af28    0x0000000400000000    
0x00001f36de20af30    0x0000000100000000    
0x00001f36de20af38    0x0000000200000000    
0x00001f36de20af40    0x0000000300000000    
0x00001f36de20af48    0x0000000400000000    
----- [ JS_ARRAY_TYPE : 0x20 ] -----
0x00001f36de20af50    0x00003a3eb2782bb1    MAP_TYPE    
0x00001f36de20af58    0x000037d2ea380c19    FIXED_ARRAY_TYPE    
0x00001f36de20af60    0x00001f36de20af81    FIXED_ARRAY_TYPE    
0x00001f36de20af68    0x0000000400000000    
----- [ ALLOCATION_MEMENTO_TYPE : 0x10 ] -----
0x00001f36de20af70    0x000037d2ea385209    MAP_TYPE    
0x00001f36de20af78    0x00003f471929db99    ALLOCATION_SITE_TYPE    
----- [ FIXED_ARRAY_TYPE : 0x30 ] -----
0x00001f36de20af80    0x000037d2ea3807a9    MAP_TYPE    
0x00001f36de20af88    0x0000000400000000    
0x00001f36de20af90    0x0000000500000000    
0x00001f36de20af98    0x0000000600000000    
0x00001f36de20afa0    0x0000000700000000    
0x00001f36de20afa8    0x0000000800000000    
----- [ ONE_BYTE_STRING_TYPE : 0xaf0 ] -----
0x00001f36de20afb0    0x000037d2ea3808e9    MAP_TYPE    
0x00001f36de20afb8    0x00000adc8ec1bad2    
0x00001f36de20afc0    0x6974636e7566280a    
0x00001f36de20afc8    0x220a7b2029286e6f    
0x00001f36de20afd0    0x6972747320657375    
0x00001f36de20afd8    0x2f2f0a0a3b227463    
[1, 2, 3, 4]
```
