#!/usr/bin/env python

import os
from setuptools import setup, find_packages

setup(
    name='ftduino_direct',
    version='1.0.8',
    packages=['ftduino_direct'],
    install_requires=['pyserial'],
    include_package_data=False,
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
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.7",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.2",
        "Programming Language :: Python :: 3.4",
        "Programming Language :: Python :: 3.5"]
)
