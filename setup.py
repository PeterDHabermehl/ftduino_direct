#!/usr/bin/env python

import os
from setuptools import setup, find_packages

setup(
    name='ftduino_direct',
    version='1.0.0',
    packages=find_packages(),
    install_requires=['pyserial'],
    zip_safe=False,
    include_package_data=True,
    author="Peter Habermehl",
    author_email="peter.habermehl@gmx.de",
    description="A python library to access the ftduino controller via USB",
    license="GPL",
    keywords="fischertechnik ftduino python",
    url="https://github.com/PeterDHabermehl/ftduino_direct/",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Education",
        "License :: OSI Approved :: GNU General Public License v2 (GPLv2)",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.2",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5"]
)