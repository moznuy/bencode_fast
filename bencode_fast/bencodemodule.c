#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define UNUSED(x) (void)(x)
static const int BUFF_SIZE = 20;

static PyObject *
decode_string(const char *bytes, Py_ssize_t size, Py_ssize_t *remaining_size) {
    const char *pos = bytes;  // Redundant?
    Py_ssize_t used_size = 0;
    PyObject *plength = NULL, *result = NULL;
    char buff[BUFF_SIZE];
    
    // Find smth like strcpy_s
    while (used_size < size && isdigit(*pos)) {
        if (used_size >= BUFF_SIZE - 1) {
            PyErr_SetString(PyExc_ValueError, "String is too long");
            goto error;
        }
        buff[used_size++] = *pos++;
    }
    buff[used_size] = 0;

    plength = PyLong_FromString(buff, NULL, 10);
    if (plength == NULL) {
        goto error;
    }
    long length = PyLong_AsLong(plength);
    if (length == -1 && PyErr_Occurred() != NULL) {
        goto error;
    }

    if (used_size++ > size || *pos++ != ':') {
        PyErr_SetString(PyExc_ValueError, "Missing \":\" after string length");
        goto error;
    }
    if (used_size + length > size) {
        PyErr_SetString(PyExc_ValueError, "Missing required number of bytes after \":\"");
        goto error;
    }
    result = PyUnicode_Decode(pos, length, "utf-8", "strict");
    used_size += length;
    if (remaining_size != NULL) {
        *remaining_size = size - used_size;
    }

error:

    Py_XDECREF(plength);
    return result;
}


static PyObject *
decode_integer(const char *bytes, Py_ssize_t size, Py_ssize_t *remaining_size) {
    const char *pos = bytes;  // Redundant?
    Py_ssize_t used_size = 0, buff_index = 0;
    PyObject *result = NULL;
    char buff[BUFF_SIZE];
    
    pos = bytes;
    // Check for 'i'
    if (used_size++ > size || *pos++ != 'i') {
        PyErr_SetString(PyExc_ValueError, "Missing \"i\" before integer");
        goto error;
    }

    // Optional '-'
    if (used_size < size && *pos == '-') {
        buff[buff_index++] = *pos++;
        used_size++;
    }

    // TODO: Same as above
    // Copy digits to \0 terminated string
    while (used_size < size && isdigit(*pos)) {
        if (buff_index >= BUFF_SIZE - 1) {
            PyErr_SetString(PyExc_ValueError, "Integer is too long");
            goto error;
        }
        buff[buff_index++] = *pos++;
        used_size++;
    }
    buff[buff_index] = 0;

    // Convert to Python int
    result = PyLong_FromString(buff, NULL, 10);
    if (result == NULL) {
        goto error;
    }
    
    // Check for 'e'
    if (used_size++ > size || *pos++ != 'e') {
        PyErr_SetString(PyExc_ValueError, "Missing \"e\" after integer");
        goto error;
    }

    if (remaining_size != NULL) {
        *remaining_size = size - used_size;
    }

    return result;
error:

    Py_CLEAR(result);
    return result;
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

    // return decode_string(bytes, size, NULL);
    Py_ssize_t tmp = size;
    PyObject *result = decode_integer(bytes, size, &tmp);
    fprintf(stderr, "\nprevios size: %zu, remaining size: %zu", size, tmp);
    return result;
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
