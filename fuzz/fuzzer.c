// Intentional .c include!
#define FUZZING
#include <bencodemodule.c>
#include <threads.h>

void DoInitialization(void) {
  PyStatus status;
  PyConfig config;
  PyConfig_InitPythonConfig(&config);

  status = Py_InitializeFromConfig(&config);
  if (PyStatus_Exception(status)) {
    exit(2);
  }
  PyConfig_Clear(&config);
  PyRun_SimpleString("from time import time,ctime\n"
                     "print('Today is', ctime(time()))\n");
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  static once_flag flag = ONCE_FLAG_INIT;
  call_once(&flag, DoInitialization);

  // fprintf(stderr, "\n\nExample string: %s\n\n", Data);
  // exit(3);
  PyObject *tmp = PyBytes_FromStringAndSize((const char *)Data, Size);
  if (tmp == NULL) {
    return 0;
  }
  //   PyObject_Print(tmp, stdout, 0);
  //   fputs("\n", stdout);
  PyObject *result = NULL;
  // Py_BEGIN_ALLOW_THREADS Nope :( Python memory allocator called without
  // holding the GIL
  result = decode(NULL, tmp);
  // Py_END_ALLOW_THREADS
  //   if (result != NULL) {
  //     PyObject_Print(result, stderr, 0);
  //     fprintf(stderr, "\n");
  //   }
  PyErr_Clear();
  Py_CLEAR(result);
  Py_CLEAR(tmp);
  return 0;
}
