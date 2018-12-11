Xilinx Radio over Ethernet Application Documentation

The sample application demonstrates how the software stack operates. It is not a complete implementation, and is not intended to be used in a production system. The application runs in the foreground or as a daemon, providing interfaces for serial, socket and filesystem FIFO communications. The sample application is composed of the following modules and libraries.

    C Library API

This library is ready for reuse in an unmodified form.

    Message Parsing Module

This module demonstrates how a human-readable message protocol can be used to configure and control the RoE Framer. It provides interfaces to the hardware API library and any protocol modules.

    Communications Module

This module provides a VFS interface for command line operation and TCP/IP and UDP/IP socket interfaces for remote operation.

    eCPRI Protocol Module

This module provides a sample implementation of the eCPRI user plane over IP message layer, including one-way delay measurements and remote memory access to a remote node.
