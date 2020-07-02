from setuptools import setup

with open("README.md", "r") as fh:
    long_description = fh.read()

setup(
    name='sdos',
    version='0.1.3',
    packages=['sdos'],
    url='https://github.com/sdos-lehr/sdos',
    license='MIT',
    author='Markus Lehr',
    author_email='sdos.lehr@gmail.com',
    description='Python library for easy use of the SDOS API.',
    long_description=long_description,
    long_description_content_type="text/markdown",
)
