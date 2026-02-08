
/* Frozen modules bootstrap
 *
 * Limited and restricted Python interpreter to run
 * "Tools/build/deepfreeze.py" on systems with no or older Python
 * interpreter.
 */

#include "Python.h"
#include "pycore_import.h"

/* Includes for frozen modules: */
#include "Python/frozen_modules/importlib._bootstrap.h"
#include "Python/frozen_modules/importlib._bootstrap_external.h"
#include "Python/frozen_modules/zipimport.h"
/* End includes */

/* Note that a negative size indicates a package. */

static const struct _frozen bootstrap_modules[] = {
    {"_frozen_importlib", _Py_M__importlib__bootstrap, (int)sizeof(_Py_M__importlib__bootstrap)},
    {"_frozen_importlib_external", _Py_M__importlib__bootstrap_external, (int)sizeof(_Py_M__importlib__bootstrap_external)},
    {"zipimport", _Py_M__zipimport, (int)sizeof(_Py_M__zipimport)},
    {0, 0, 0} /* bootstrap sentinel */
};
static const struct _frozen stdlib_modules[] = {
    {0, 0, 0} /* stdlib sentinel */
};
static const struct _frozen test_modules[] = {
    {0, 0, 0} /* test sentinel */
};
const struct _frozen *_PyImport_FrozenBootstrap = bootstrap_modules;
const struct _frozen *_PyImport_FrozenStdlib = stdlib_modules;
const struct _frozen *_PyImport_FrozenTest = test_modules;

static const struct _module_alias aliases[] = {
    {"_frozen_importlib", "importlib._bootstrap"},
    {"_frozen_importlib_external", "importlib._bootstrap_external"},
    {0, 0} /* aliases sentinel */
};
const struct _module_alias *_PyImport_FrozenAliases = aliases;

/* Embedding apps may change this pointer to point to their favorite
   collection of frozen modules: */

const struct _frozen *PyImport_FrozenModules = NULL;

int
#ifdef MS_WINDOWS
wmain(int argc, wchar_t **argv)
#else
main(int argc, char **argv)
#endif
{
    PyStatus status;

    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);
    // don't warn, pybuilddir.txt does not exist yet
    config.pathconfig_warnings = 0;
    // parse arguments
    config.parse_argv = 1;
    // add current script dir to sys.path
    config.isolated = 0;
    config.safe_path = 0;

#ifdef MS_WINDOWS
    status = PyConfig_SetArgv(&config, argc, argv);
#else
    status = PyConfig_SetBytesArgv(&config, argc, argv);
#endif
    if (PyStatus_Exception(status)) {
        goto error;
    }

    status = PyConfig_Read(&config);
    if (config.run_filename == NULL) {
        status = PyStatus_Error("Run filename expected");
        goto error;
    }

/* This example asks the user for a height and width of an m X n game board with a maximum dimension of 100 squares.*/

#define MAX_DIM 100
#include <stdio.h>
#include <stdlib.h>

struct board_square_t {
   int height;
   int width;
};


int main(){
    /* board dimensions.*/
    int m,n, error;
    struct board_square_t *board;
    printf("Please specify the board height: \n");
    error = scanf("%d", &m);
    if ( EOF == error ){
        printf("No integer passed: Die evil hacker!\n");
    }
    printf("Please specify the board width: \n");
    error = scanf("%d", &n);
    if ( EOF == error ){
         printf("No integer passed: Die evil hacker!\n");
    }
    if ( m > MAX_DIM || n > MAX_DIM ) {
        printf("Value too large: Die evil hacker!\n");
    }
    board = (struct board_square_t*) malloc( m * n * sizeof(struct board_square_t));
    return 0;
}

/*While this code checks to make sure the user cannot specify large, positive integers and consume too much memory, 
  it does not check for negative values supplied by the user. As a result, an attacker can perform a resource consumption (CWE-400) 
  attack against this program by specifying two, large negative values that will not overflow, resulting in a very large memory allocation (CWE-789) 
  and possibly a system crash. Alternatively, an attacker can provide very large negative values which will cause an integer overflow (CWE-190) 
  and unexpected behavior will follow depending on how the values are treated in the remainder of the program. */
#define CLEAR(ATTR) \
    do { \
        PyMem_RawFree(ATTR); \
        ATTR = NULL; \
    } while (0)

    // isolate from system Python
    CLEAR(config.base_prefix);
    CLEAR(config.prefix);
    CLEAR(config.base_exec_prefix);
    CLEAR(config.exec_prefix);

    status = Py_InitializeFromConfig(&config);
    if (PyStatus_Exception(status)) {
        goto error;
    }
    PyConfig_Clear(&config);

    return Py_RunMain();

error:
    PyConfig_Clear(&config);
    if (PyStatus_IsExit(status)) {
        return status.exitcode;
    }
    Py_ExitStatusException(status);
}

