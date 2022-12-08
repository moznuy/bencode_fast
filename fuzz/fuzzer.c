// Intentional .c include!
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

  if (Size < 1) {
    return 0;
  }

  for (const uint8_t *pos = Data; pos < Data + Size - 1; pos++) {
    if (*pos == 0) {
      return 0;
    }
    // Check for ascii text
    if (!isprint(*pos)) { 
      return 0;
    }
  }
  if (Data[Size - 1] != 0) {
    return 0;
  }

  // fprintf(stderr, "\n\nExample string: %s\n\n", Data);
  // exit(3);
  PyObject *tmp = PyBytes_FromString((const char *)Data);
  if (tmp == NULL) {
    return 0;
  }
  PyObject *result = NULL;
  // Py_BEGIN_ALLOW_THREADS Nope :( Python memory allocator called without holding the GIL
  result = decode(NULL, tmp);
  // Py_END_ALLOW_THREADS
  // if (result != NULL && rand()%100 == 0) {
  //   PyObject_Print(result, stderr, 0);
  //   fprintf(stderr, "\n");
  // }
  PyErr_Clear();
  Py_CLEAR(result);
  Py_CLEAR(tmp);
  return 0;
}
