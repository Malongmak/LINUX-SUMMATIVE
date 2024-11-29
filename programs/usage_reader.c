#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *read_usage_file(PyObject *self, PyObject *args) {
    const char *file_path;
    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    FILE *file = fopen(file_path, "r");
    if (!file) {
        PyErr_SetString(PyExc_FileNotFoundError, "File not found");
        return NULL;
    }

    PyObject *result = PyList_New(0);
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        PyObject *entry = PyList_New(0);
        char timestamp[9];
        double cpu, mem;
        long rx, tx;

        sscanf(line, "%8s %lf %lf %ld %ld", timestamp, &cpu, &mem, &rx, &tx);
        PyList_Append(entry, PyUnicode_FromString(timestamp));
        PyList_Append(entry, PyFloat_FromDouble(cpu));
        PyList_Append(entry, PyFloat_FromDouble(mem));
        PyList_Append(entry, PyLong_FromLong(rx));
        PyList_Append(entry, PyLong_FromLong(tx));
        PyList_Append(result, entry);
    }

    fclose(file);
    return result;
}

static PyMethodDef UsageReaderMethods[] = {
    {"read_usage_file", read_usage_file, METH_VARARGS, "Read metrics from usage.txt"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef usageReaderModule = {
    PyModuleDef_HEAD_INIT,
    "usage_reader",
    NULL,
    -1,
    UsageReaderMethods
};

PyMODINIT_FUNC PyInit_usage_reader(void) {
    return PyModule_Create(&usageReaderModule);
}
