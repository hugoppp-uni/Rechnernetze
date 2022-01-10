# Mininet Experiment
This experiment is designed to test reliable network protocols, especially the
BFT protocol, over a noisy channel.
This experiment is part of the *computer networks* lecture at the *University
of Applied Sciences Hamburg*.

## Experiment setup
The experiment is based on the network emulator [mininet](http://mininet.org/).

Mininet is a network emulator, or perhaps more precisely a network emulation
orchestration system. It runs a collection of end-hosts, switches, routers,
and links on a single Linux kernel. It uses lightweight virtualization to make
a single system look like a complete network, running the same kernel, system,
and user code. A Mininet host behaves just like a real machine; you can run
arbitrary programs (including anything that is installed on the underlying
Linux system.)

The [mininet walkthrough](http://mininet.org/walkthrough/) can help you to get
acquainted with the mininet CLI.
Mininet is almost completely build on python, but phyton-specific knowlegde
is not really required to use mininet.

The network used in this experiment is illustrated below.

```
client ===== switch ===== server
```

## Software Requirements
The following UNIX/linux packages are required.
A successful build was tested with the following versions.
However, deviations in package versions might also work.

| Package         | Version       |
|-----------------|---------------|
| `mininet`       | 2.3.0         |
| `python`        | 3.9.5         |
| `tshark`        | 3.4.5         |
| `diff`          | 3.7           |

The package [`tskark`](https://www.wireshark.org/docs/man-pages/tshark.html) is part of your wireshark installation.

Alternatively you can use the *mininet vm* or the *bits-vm* provided by CaDS.

## Setup & Run
Before you can run the experiment place your java class files in the `bft`
folder. Run the experiment by calling the `experiment.py` script.

The `experiment.py` script will start the server and sequentially upload files
from the `testfiles` directory.

Use `sudo mn -c` to reset the *mininet* environment if necessary.

## Project Overview

```
.
├── bft                   (1) Place your executables here! (bft_server and bft_client)
├── experiment.py         (2) call to setup and run experiment
├── logs
├── README.md
└── testfiles
    ├── asciitest.txt
    ├── test-1KiB.data
    ├── test-1MiB.data
    └── test-16MiB.data
```

## Example Output
This is an example of an experiment's output.
Each line represent an experiment, i.e. upload a specific file.
If the line ends with `OK` the server successfully created the file.
If the line ends with `ERROR` the server failed to created the file or the
content of the test file does not match the file in the server's storage folder.

```bash
$ sudo ./experiment.py
testfiles/asciitest.txt        => PASSED
testfiles/rfc768.txt           => PASSED
testfiles/saltzer.pdf          => PASSED
testfiles/test-1KiB.data       => PASSED
testfiles/test-1MiB.data       => FAILED
```

## Links
- [Mininet - http://mininet.org/](http://mininet.org/)
- [Mininet Walkthrough - http://mininet.org/walkthrough/](http://mininet.org/walkthrough/)
- [tshark - https://www.wireshark.org/docs/man-pages/tshark.html](https://www.wireshark.org/docs/man-pages/tshark.html)
