#define PY_SSIZE_T_CLEAN
#include <Python.h>

static const int BUFF_SIZE = 20;
static PyObject *_decode(const char **beg, const char *end);

// We can't change src bytes, because they belong to Python Bytes Object
// and \0 terminated string is required for PyLong_FromString
// Hence this function
//
// TODO: Forget about PyLong_FromString and buff and just construct number by
// self
static PyObject *PyLong_FromStringPrefix(const char **source_beg,
                                         const char *source_end) {
  char buff[BUFF_SIZE];
  const char *source_pos = *source_beg;

  while ((source_pos < source_end && isdigit(*source_pos)) ||
         (source_pos == *source_beg && *source_pos == '-')) {
    ++source_pos;
  }

  // Zero digits
  if (source_pos == *source_beg ||
      (source_pos == *source_beg + 1 && **source_beg == '-')) {
    PyErr_SetString(PyExc_ValueError, "Integer expected");
    return NULL;
  }

  if (source_pos - *source_beg + 1 > BUFF_SIZE) {
    PyErr_SetString(PyExc_ValueError, "Integer is too large");
    return NULL;
  }
  memcpy(buff, *source_beg, source_pos - *source_beg);
  buff[source_pos - *source_beg] = 0;

  *source_beg = source_pos;
  return PyLong_FromString(buff, NULL, 10);
}

static PyObject *decode_string(const char **beg, const char *end) {
  // Parse the length
  PyObject *plength = NULL, *result = NULL;

  plength = PyLong_FromStringPrefix(beg, end);
  if (plength == NULL) {
    goto error;
  }

  long length = PyLong_AsLong(plength);
  if (length == -1 && PyErr_Occurred() != NULL) {
    goto error;
  }

  if (length < 0) {
    PyErr_SetString(PyExc_ValueError, "String length can't be negative");
    goto error;
  }

  if (*beg >= end || *(*beg)++ != ':') {
    PyErr_SetString(PyExc_ValueError, "Missing \":\" after string length");
    goto error;
  }

  // Check again if correct:
  if (*beg + length > end) {
    PyErr_SetString(PyExc_ValueError,
                    "Missing required number of bytes after \":\"");
    goto error;
  }

  result = PyUnicode_Decode(*beg, length, "utf-8", "strict");
  *beg += length;

error:

  Py_XDECREF(plength);
  return result;
}

static PyObject *decode_integer(const char **beg, const char *end) {
  PyObject *result = NULL;

  // Check for 'i'
  if (*beg >= end || *(*beg)++ != 'i') {
    PyErr_SetString(PyExc_ValueError, "Missing \"i\" before integer");
    goto error;
  }

  // Convert to Python int
  result = PyLong_FromStringPrefix(beg, end);
  if (result == NULL) {
    goto error;
  }

  // Check for 'e'
  if (*beg >= end || *(*beg)++ != 'e') {
    PyErr_SetString(PyExc_ValueError, "Missing \"e\" after integer");
    goto error;
  }

  return result;
error:

  Py_CLEAR(result);
  return result;
}

static PyObject *decode_list(const char **beg, const char *end) {
  PyObject *result = NULL, *item = NULL;

  // Check for 'l'
  if (*beg >= end || *(*beg)++ != 'l') {
    PyErr_SetString(PyExc_ValueError, "Missing \"l\" before list");
    goto error;
  }

  result = PyList_New(0);
  if (result == NULL) {
    goto error;
  }

  while (1) {
    if (*beg >= end) {
      PyErr_SetString(PyExc_ValueError, "Missing \"e\" after list");
      goto error;
    }

    // Check for 'e'
    if (**beg == 'e') {
      (*beg)++;
      break;
    }

    item = _decode(beg, end);
    if (item == NULL) {
      goto error;
    }
    if (PyList_Append(result, item) != 0) {
      goto error;
    }
    Py_CLEAR(item);
  }

  return result;

error:

  Py_CLEAR(item);
  Py_CLEAR(result);
  return result;
}

static PyObject *decode_dict(const char **Py_UNUSED(beg),
                             const char *Py_UNUSED(end)) {
  PyErr_SetString(PyExc_NotImplementedError, "");
  return NULL;
}

static PyObject *_decode(const char **beg, const char *end) {
  if (*beg >= end) {
    PyErr_SetString(PyExc_AssertionError, "Decode error");
    return NULL;
  }

  if (**beg == 'i') {
    return decode_integer(beg, end);
  }
  if (isdigit(**beg)) {
    return decode_string(beg, end);
  }
  if (**beg == 'l') {
    return decode_list(beg, end);
  }
  if (**beg == 'd') {
    return decode_dict(beg, end);
  }

  PyErr_SetString(PyExc_ValueError,
                  "Unknown character found at the beginning of new value");
  return NULL;
}

static PyObject *decode(PyObject *Py_UNUSED(self), PyObject *input_bytes) {
  const char *bytes;

  bytes = PyBytes_AsString(input_bytes);
  if (bytes == NULL) {
    return NULL;
  }
  Py_ssize_t size = PyBytes_Size(input_bytes);

  const char *beg = bytes;
  PyObject *result = _decode(&bytes, bytes + size);
  if (bytes != beg + size && PyErr_Occurred() == NULL) {
    Py_CLEAR(result);
    PyErr_SetString(PyExc_ValueError,
                    "Supported only 1 value in the top level");
  }
  // fprintf(stderr, "\nprevios size: %zu, remaining size: %zu", size,
  //         size - (bytes - beg));
  return result;
}

static PyMethodDef DecodeMethods[] = {
    {"decode", decode, METH_O, "Decode bencode"}, {NULL, NULL, 0, NULL}};

static struct PyModuleDef bencodemodule = {
    PyModuleDef_HEAD_INIT,
    "bencodemodule",
    "bencodemodule docs",
    -1,
    DecodeMethods,
    NULL,
    NULL,
    NULL,
    NULL,
};

PyMODINIT_FUNC PyInit_bencodemodule(void) {
  return PyModule_Create(&bencodemodule);
}
