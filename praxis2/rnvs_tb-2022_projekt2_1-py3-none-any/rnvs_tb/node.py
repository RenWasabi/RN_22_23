import traceback
import socket
import threading

import ctypes
import logging
import os

CLONE_NEWNET = 0x40000000


class NamespaceHandler:
    def __init__(self, pid):
        # For an unknown reason we lock ourselves up in the wrong namespace if we do not use the init process as base
        # Presumably, because we initialize the Handler at the wrong moment, somewhere.
        self.base = 1  # os.getpid()
        self.pid = pid

    def switch_namespace(self, netns_pid):
        libc = ctypes.CDLL('libc.so.6', use_errno=True)
        log = logging.getLogger(__name__)

        def errcheck(ret, func, args):
            if ret == -1:
                e = ctypes.get_errno()
                raise OSError(e, os.strerror(e))

        libc.setns.errcheck = errcheck
        try:
            with open('/proc/{:d}/ns/net'.format(netns_pid)) as fd:
                libc.setns(fd.fileno(), CLONE_NEWNET)
        except OSError:
            log.exception('Failed to enter network namespace of pid {}'.format(netns_pid))
            return

    def __enter__(self):
        self.switch_namespace(self.pid)

    def __exit__(self, exc_type, exc_value, tb):
        self.switch_namespace(self.base)
        if exc_type is not None:
            traceback.print_exception(exc_type, exc_value, tb)
            return False


class DockerThread(threading.Thread):
    def __init__(self, container, *args, **kwargs):
        """
        A Thread subclass that runs in the network namespace of the given
        container. NOTE: It will only share the network stack, not the
        filesystem, hence NameResolution of containers will not work! 
        """
        super().__init__(*args, **kwargs)
        self.container = container
        self.container.reload()
        self.pid = self.container.attrs['State']['Pid']
        self.ns = NamespaceHandler(self.pid)

    def run(self):
        with self.ns:
            self._target(*self._args, **self._kwargs)

    def get_ip(self, name):
        """
        Returns the IP for a given host as viewed from the container. Useful
        for obtaining the IP of a named container in the same user-defined
        network.
        """
        status, ips = self.container.exec_run(f'getent ahosts {name}')

        if status != 0:
            return None

        ips = ips.decode('utf-8')

        for line in ips.splitlines():
            cols = line.split()

            if not name in cols:
                continue

            return cols[0]

        return None
