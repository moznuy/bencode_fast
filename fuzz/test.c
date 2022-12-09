#include <Python.h>
#include <stdio.h>

int main(void) {
  PyStatus status;
  PyConfig config;
  PyConfig_InitPythonConfig(&config);

  status = Py_InitializeFromConfig(&config);
  if (PyStatus_Exception(status)) {
    exit(2);
  }
  PyConfig_Clear(&config);
  //  PyRun_SimpleString("from time import time,ctime\n"
  //                     "print('Today is', ctime(time()))\n");

  char *t = "abc\0def";
  printf("%s\n\n", t);
  PyObject *t1 = PyBytes_FromString(t);
  PyObject *t2 = PyBytes_FromStringAndSize(t, 8);
  PyObject_Print(t1, stdout, 0);
  printf("\n\n");
  PyObject_Print(t2, stdout, 0);
  printf("\n\n");

  char *k1 = PyBytes_AsString(t1);
  char *k2;
  Py_ssize_t s2;
  PyBytes_AsStringAndSize(t2, &k2, &s2);
  printf("k1: %s\n\n", k1);
  printf("k2: %lu %s\n\n,", s2, k2);
  return 0;
}
