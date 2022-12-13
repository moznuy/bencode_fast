from distutils.core import setup, Extension

c_ext = Extension(
    "bencode_fast.bencodemodule",
    extra_compile_args=["-Werror"],
    sources=["bencode_fast/bencodemodule.c"],
)

setup(
    name="bencode_fast",
    version="0.1",
    description="Fast bencode parser",
    ext_modules=[c_ext],
    packages=["bencode_fast"],
    python_requires=">=3.9.0",  # TODO: because of: Py_EnterRecursiveCall, Py_LeaveRecursiveCall
    # TODO: if solved, then >=3.6.0
)
