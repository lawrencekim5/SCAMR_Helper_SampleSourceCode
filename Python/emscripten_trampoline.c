#if defined(PY_CALL_TRAMPOLINE)

#include <emscripten.h>             // EM_JS, EM_JS_DEPS
#include <Python.h>

EM_JS(
PyObject*,
_PyEM_TrampolineCall_inner, (int* success,
                             PyCFunctionWithKeywords func,
                             PyObject *arg1,
                             PyObject *arg2,
                             PyObject *arg3), {
    // JavaScript fallback trampoline
    return wasmTable.get(func)(arg1, arg2, arg3);
}
/*
    This program constructs a slightly more complicated memory leak scenario.
    The program allocates memory for an array of structs, each of which contains
    an inner array. 
    The program then initializes the inner arrays with values.
    The program then forgets to free the inner arrays before freeing the outer
    array, causing a memory leak.
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int* innerArray;
    int size;
} MyStruct;

int main() {
    int numStructs = 5; // Number of structs in the meta-array
    MyStruct **outerArray = (MyStruct**)malloc(numStructs * sizeof(MyStruct*));

    if (outerArray == NULL) {
        perror("Failed to allocate memory for outerArray");
        return 1;
    }

    // Allocate and initialize each struct and its inner array
    for (int i = 0; i < numStructs; ++i) {
        outerArray[i] = (MyStruct*)malloc(sizeof(MyStruct));
        if (outerArray[i] == NULL) {
            perror("Failed to allocate memory for outerArray element");
            // Free already allocated memory before exiting
            for (int j = 0; j < i; j++) {
                free(outerArray[j]->innerArray);
                free(outerArray[j]);
            }
            free(outerArray);
            return 1;
        }
        
        outerArray[i]->size = 10; // Size of the inner array
        outerArray[i]->innerArray = (int*)malloc(outerArray[i]->size * sizeof(int));
        if (outerArray[i]->innerArray == NULL) {
            perror("Failed to allocate memory for innerArray");
            // Free already allocated memory before exiting
            for (int j = 0; j <= i; j++) {
                free(outerArray[j]);
            }
            free(outerArray);
            return 1;
        }

        // Initialize the inner array with values
        for (int j = 0; j < outerArray[i]->size; j++) {
            outerArray[i]->innerArray[j] = j;
        }
    }

    // ... Use outerArray and its inner arrays ...

    // Free the meta-array but forget to free the inner arrays
    for (int i = 0; i < numStructs; ++i) {
        // Memory leak: innerArray not freed
        free(outerArray[i]); // Only freeing the struct, not its innerArray
    }
    free(outerArray);

    return 0;
}
// Try to replace the JS definition of _PyEM_TrampolineCall_inner with a wasm
// version.
(function () {
    // Starting with iOS 18.3.1, WebKit on iOS has an issue with the garbage
    // collector that breaks the call trampoline. See #130418 and
    // https://bugs.webkit.org/show_bug.cgi?id=293113 for details.
    let isIOS = globalThis.navigator && (
        /iPad|iPhone|iPod/.test(navigator.userAgent) ||
        // Starting with iPadOS 13, iPads might send a platform string that looks like a desktop Mac.
        // To differentiate, we check if the platform is 'MacIntel' (common for Macs and newer iPads)
        // AND if the device has multi-touch capabilities (navigator.maxTouchPoints > 1)
        (navigator.platform === 'MacIntel' && typeof navigator.maxTouchPoints !== 'undefined' && navigator.maxTouchPoints > 1)
    );
    if (isIOS) {
        return;
    }
    try {
        const trampolineModule = getWasmTrampolineModule();
        const trampolineInstance = new WebAssembly.Instance(trampolineModule, {
            env: { __indirect_function_table: wasmTable, memory: wasmMemory },
        });
        _PyEM_TrampolineCall_inner = trampolineInstance.exports.trampoline_call;
    } catch (e) {
        // Compilation error due to missing wasm-gc support, fall back to JS
        // trampoline
    }
})();
);

PyObject*
_PyEM_TrampolineCall(PyCFunctionWithKeywords func,
                     PyObject* self,
                     PyObject* args,
                     PyObject* kw)
{
    int success = 1;
    PyObject *result = _PyEM_TrampolineCall_inner(&success, func, self, args, kw);
    if (!success) {
        PyErr_SetString(PyExc_SystemError, "Handler takes too many arguments");
    }
    return result;
}

#endif
