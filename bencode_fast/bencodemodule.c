#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define UNUSED(x) (void)(x)

static PyObject *
decode_string(const char *bytes, Py_ssize_t size) {
    UNUSED(size);
    const char *end = NULL;
    Py_ssize_t used_length = 0;
    
    // TODO: check buff overflow
    char buff[20];
    end = bytes;
    while (used_length < size && isdigit(*end)) {
        buff[used_length++] = *end++;
    }
    buff[end-bytes] = 0;

    PyObject *plength = PyLong_FromString(buff, NULL, 10);
    if (plength == NULL) {
        return NULL;
    }
    long length = PyLong_AsLong(plength);
    Py_XDECREF(plength);
    if (length == -1 && PyErr_Occurred() != NULL) {
        return NULL;
    }

    end = bytes + (end-bytes);
    if (used_length++ > size || *end++ != ':') {
        PyErr_SetString(PyExc_ValueError, "\":\" missing after string length");
        return NULL;
    }
    if (used_length + length > size) {
        PyErr_SetString(PyExc_ValueError, "missing required number of bytes after \":\"");
        return NULL;
    }
    return PyUnicode_Decode(end, length, "utf-8", "strict");
}

static PyObject *
decode(PyObject *self, PyObject *input_bytes) {
    UNUSED(self);
    const char *bytes;

    bytes = PyBytes_AsString(input_bytes);
    if (bytes == NULL) {
        return NULL;
    }
    Py_ssize_t size = PyBytes_Size(input_bytes);

    return decode_string(bytes, size);
}


static PyMethodDef SpamMethods[] = {
    {"decode", decode, METH_O, "Decode bencode"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef bencodemodule = {
    PyModuleDef_HEAD_INIT,
    "bencodemodule",      /* name of module */
    "bencodemodule docs",  /* module documentation, may be NULL */
    -1,          /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    SpamMethods,
    NULL,
    NULL,
    NULL,
    NULL,
};


PyMODINIT_FUNC
PyInit_bencodemodule(void)
{
   return PyModule_Create(&bencodemodule);
}
