from setuptools import setup, Extension # type: ignore

module = Extension("usage_reader", sources=["usage_reader.c"])

setup(
    name="usage_reader",
    version="1.0",
    description="C extension to read usage metrics",
    ext_modules=[module],
)
