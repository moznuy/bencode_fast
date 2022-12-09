from distutils.core import setup, Extension

c_ext = Extension(
    "bencode_fast.bencodemodule",
    # extra_compile_args=["-O0", "-g"],
    sources=["bencode_fast/bencodemodule.c"],
)

setup(
    name="bencode_fast",
    version="0.1",
    description="Fast bencode parser",
    ext_modules=[c_ext],
    packages=["bencode_fast"],
)
