# SDOS
SDOS stands for Standardized Distributed Object Storage and is an alternative way of handling data transfer in the field of High Performance Computing.

It **is not** an alternative implementation of a file system.

It **is** a proposal for a standardized API using existing technologies.

## Installation

SDOS should allow users to transfer data between them.
Therefore we recommend installing sdos not user-wise, but once per system.
To guarantee interoperability, we also recommend installing the libraries system-wide.

Our provided HTTP server is a flask server, written in python 3. It can be found under `sdos_management/app.py`.

The directories `sdos_c_lib` and `sdos_python_lib` contain the source code for the C and Python library respectively. 

Running the management server requires flask to be installed and building the C library requires `json-c` and `curl` to be installed.

#### Installation via dpkg

We also provide a `.deb` file, which creates a virtual python environment for the management server and runs it as a systemd service named sdos.service.

It also adds the sdos library to `/usr/lib` (header in `/usr/include`) for easy use and linking in C.

Download the .deb file from TODO. And install it with
`sudo apt install path/to/sdos_version.deb`.

## Motivation
In HPC and other fields of Computer Science, many applications have to be benchmarked.
Usually some input instances are generated via script (shell, python,...) or by hand  and then are passed to the application to run these instances.
The application returns some output, which is then processed in some other script (for validation or generation of tables or figures).

#### Existing Solutions
Usually these data transfers are done via files. E.g., a Python script writes some integers to a `.txt` file, which is then `scp`ed to the server for benchmarking.
The C application reads the file line by line and parses the values into some custom `struct`.
On distributed systems using, e.g. `MPI` for communication, the data has to be distributed between workers with some custom logic.
The C application writes its results or runtimes to some other `.txt` file, which is then `scp`ed back for further analysis or processing.
Finally, some Python script reads the file again and generates some figures.

Regardless of the specific technologies in use, there essentially are 3 data transfers between applications:
 * input generator -> application
 * main thread -> workers
 * workers -> output processor

There are plenty solutions for handling file distribution.
They range from distributed file systems (low level), to proprietary cloud solutions, like dropbox (high level).

#### Remaining Problems
Although there exist good solutions for distributing files, their management still remains tedious.
Dumping a python dictionary to a file is easy - it can be done in ~3 lines of code.
Reading the file in C or any other language besides Python becomes hard, since Pythons representation of data is not easily processable by C, without libraries or precise insight of how the data is structured within the file.

Usually when input is transferred between applications, programmers do not care about **files**, but about the **data** itself.

## Contributions

With SDOS we aim to bring the focus back from file-management to data transfer.
Using the existing standards (or 'de-facto standards') of HTTP and REST, we handle both the data transfer between different applications and the data transfer between different machines.

What we do **not**:
* We do **not** propose a feature rich data description language, able to represent any datatype.
* We do **not** introduce any new file format, connection type or web technology.
* We do **not** support every REST method and HTTP flavour.

What we **do**:
* We propose a subset of REST methods, which can be used for all relevant data transfers.
* We propose a subset of necessary HTTP properties.
* We propose a specific API, for storing and querying data.