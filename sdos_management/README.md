# Example management implementation for SDOS
This project is a python implementation of the suggested SDOS API. 
It essentially is a webserver, which listens for data to be put in or data to be queried.

## Dependencies

This project uses `flask` for conveniently handling tcp requests and to serve some simple html pages.

Proxying (internal forwarding to other management nodes) is done using `urllib.request`.

## Installation

We recommend installing SDOS using the provided debian package.
Mainly because we put a lot of work into it, not because it is better than installing it yourself in any way.

If you want to install the package manually, look at the `create_deb.sh` file, which contains the code we use to generate the aforementioned `.deb` file and therefore should give you an idea of the necessary steps.

After installing sdos, you should have a service called `sdos` already running and enabled to start after rebooting with the hook `default.target`.
Check the status with `systemctl status sdos`.

When deploying sdos in a network, **every** machine has to run this management application **once**.
For example when you have a HPC cluster running MPI on 10 nodes with 20 processes each. Each node has to run an instance of sdos, but not one for every MPI process.
Each process communicates with the "localhost" accessible TCP interface.

Data will not be replicated on all instances, but only when data/file management is explicitly demanded.

The locally running management application needs to know the "parent" management application's IP to proxy all requests to there.
Configure it using debconf by calling `sudo dpkg-reconfigure sdos`.
Restart your management application using `systemctl restart sdos`.

### Non-interactive installation

If you deploy sdos on multiple machines (which, kind of, is the whole point) you probably do not want to use `sudo dpkg-reconfigure sdos` manually on every machine.

You can set debconf's database entries before calling reconfigure by calling:

`sudo echo "set sdos/management-ip YOUR_IP_OR_HOSTNAME_WITHOUT_PORT_OR_QOUTES" | sudo debconf-communicate
`

And then call reconfigure non interactively using:

`sudo dpkg-reconfigure --frontend noninteractive sdos`

Restart the service for the changes to take effect (`systemctl restart sdos`) on EVERY machine running the management application.

## Usage

Run the flask application either with the flask module (recommended) or simply call `python3 app.py`.
If you installed the endpoint using the provided debian package, you probably already have the backend running as the service called `sdos`.
Check with `systemctl status sdos`. TCP port 5006 needs to be available for the service to run.

All queries will be proxied to the endpoint, specified by the ip (or hostname)
 contained in `/usr/share/sdos/management-ip`. See the installation section above for configuration alternatives.
 
## API Documentation

This endpoint offers the described API from the `API.md` file of the root project (`../API.md`).
TODO: It also offers a simple GUI for viewing data entries and adding new values