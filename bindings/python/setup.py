# Python setup.py compilation
# Inspired by pythran's one (https://github.com/serge-sans-paille/pythran/blob/master/setup.py)

from distutils.core import setup, Command
from distutils.command.build import build
from unittest import TextTestRunner, TestLoader
import os
import sys
import shutil
from subprocess import check_call
import tempfile

tmp_dir = tempfile.mkdtemp()

class Build(build):
    def run(self, *args, **kwargs):
        # Build with cmake
        leeloo_build_dir = os.path.join(self.build_temp, 'leeloo')
        print(leeloo_build_dir)
        if os.path.exists(leeloo_build_dir):
            shutil.rmtree(leeloo_build_dir)
        py_version = "%d.%d" % (sys.version_info.major, sys.version_info.minor)
        os.makedirs(leeloo_build_dir)
        cwd = os.getcwd()
        os.chdir(leeloo_build_dir)
        check_call(['cmake',
                    '-DCMAKE_BUILD_TYPE=release',
                    '-DPYTHON_VERSION='+py_version,
                    cwd])
        check_call(['make','-j'])
        os.chdir(cwd)

        # Create the package directory
        leeloo_package_dir = os.path.join(tmp_dir, "pyleeloo")
        #leeloo_package_dir = os.path.join(self.build_temp, 'package/leeloo')
        os.makedirs(leeloo_package_dir)
        shutil.copy(os.path.join(leeloo_build_dir, "pyleeloo.so"), os.path.join(leeloo_package_dir, "pyleeloo.so"))
        with open(os.path.join(leeloo_package_dir, "__init__.py"), "w") as f:
            f.write("# leeloo python package\n")
            f.write("from .pyleeloo import *")

        build.run(self, *args, **kwargs)

setup(name='pyleeloo',
      version='1.0',
      description='Library to manage big sets of integers',
      author='Adrien Guinet',
      author_email='aguinet@quarkslab.com',
      url='https://github.com/quarkslab/libleeloo',
      package_dir={'': tmp_dir},
      packages=['pyleeloo'],
      package_data={'pyleeloo': ['*.so']},
      classifiers=[
      'Development Status :: 5 - Stable',
      'Environment :: Console',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: BSD License',
      'Natural Language :: English',
      'Operating System :: POSIX :: Linux',
      ],
      license="BSD 3-Clause",
      cmdclass={
      'build': Build,
      })
