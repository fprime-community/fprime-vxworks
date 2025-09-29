""" fprime_ci_plugins/vxworks.py: vxworks CI implementation

This module supplies basic VxWorks CI plugins. These basic implementations will build and run VxWorks builds using
TFTP to provide boot modules and RSH to provide Downloadable Kernel Modules (DKMs). This plugin assumes certain
infrastructure is available on the host machine:

1. A docker container running with TFTP and RSH installed
2. The target hardware bootloader is configured for TFTP boot
"""
import os
import stat
import logging
import shutil
import subprocess
from enum import Enum
from pathlib import Path
from typing import Type
import serial

import fprime_gds.plugin.definitions
from fprime_ci.ci import Ci
from fprime_ci.plugin.definitions import plugin
from fprime_ci.utilities import IOLogger

LOGGER = logging.getLogger(__name__)

@plugin(Ci)
class VxWorksCi(Ci):
    """ VxWorks CI plugin implementation supporting DKMs """
    class Keys(Ci.Keys):
        """ Additional keys used during the execution of the VxWorks plugin

        These keys are used as constants when accessing context and used to validate context automatically. These keys
        are supplied in the initial supplied context. To fully understand these keys, see: CiPlugin.Keys for a better
        description of the usage and format.
        """
        VIP_PATH = "vip-path"
        WR_SHELL_PATH = "wr-shell-path"
        DTB_NAME = "dtb-name"
        REMOTE_DATA = "remote-data"

    def __init__(self, port, baud, flow:str="no"):
        """  """
        self.port = serial.Serial()
        self.port.port = port
        self.port.baudrate = baud
        self.port.rtscts = flow == "yes"
        self.monitor_fsw_thread = None

    def write_to_vxworks(self, message: bytes):
        """ Write to the serial port """
        assert self.port.is_open, "Serial port is not open"
        LOGGER.debug(">  " + message.decode("ascii").strip())
        self.port.write(message + b"\r\n")

    def wait_for_vxprompt(self, prompt="-> "):
        """ Wait for the vxprompt to be ready """
        assert self.port.is_open, "Serial port is not open"
        IOLogger.communicate(
            [self.port],
            [IOLogger(None, logging.DEBUG, logger_name=f"[VxConsole]")],
            timeout=20.0,
            end=lambda line, index: prompt in line,
            close=False
        )
    
    def monitor_fsw_run(self):
        """ Wait for the vxprompt to be ready """
        assert self.port.is_open, "Serial port is not open"
        self.monitor_fsw_thread = IOLogger.async_communicate(
            [self.port],
            [IOLogger(None, logging.DEBUG, logger_name=f"[VxConsole]")],
        )


    def build(self, context: dict) -> dict:
        """ Performs the VxWorks build before the standard F Prime build

        This build step will perform the VxWorks image build providing the uVxWorks and dtb files required for loading.
        It expects the bootloader is configured to use the uVxWorks and dtb files correctly.

        This implementation ops not to set any arguments as the settings.ini should be sufficient.

        Args:
            context: build context aggregated across all build steps
        Returns:
            context with optionally set platform, generated_arguments and build_argument
        """
        subprocess.run([context["wr-shell-path"], "make"], cwd=context[VxWorksCi.Keys.VIP_PATH]).check_returncode()
        return context

    def preload(self, context: dict):
        """ Load the software to target hardware before power-on

        This function may be overridden by platform developers to perform software loading actions in preparation for
        power-on. This is the most convenient place to set up files pulled-in via the boot process (like network boot
        files, etc). This step runs directly before power-on.

        TODO: list variables containing software set-up

        The default implementation does nothing.

        Args:
            context: build context aggregated across all build steps
        Returns:
            context optionally augmented with plugin-specific preload data
        """
        context["dkm_path"] = f"data/{context[VxWorksCi.Keys.DEPLOYMENT_NAME]}"

        for path in context[Ci.Keys.BUILD_OUTPUTS]:
            destination_path = Path(context[VxWorksCi.Keys.REMOTE_DATA]) / path.name
            if destination_path.exists():
                destination_path.unlink()
            shutil.copy(path, context[VxWorksCi.Keys.REMOTE_DATA])
            permissions = stat.S_IRUSR | stat.S_IWUSR | stat.S_IXUSR | \
                          stat.S_IRGRP | stat.S_IWGRP | stat.S_IXGRP | \
                          stat.S_IROTH
            os.chmod(destination_path, permissions)
        os.sync()
        return context

    def load(self, context: dict):
        """ Load the software to target hardware after power-on

        This function may be overridden by platform developers to perform software loading actions in preparation post
        power-on. This is the most convenient place to copy files via an active program like scp  This step runs
        directly after power-on.

        The default implementation does nothing.

        Note: platforms with long boot times should confirm a successful boot code before attempting load operations.

        TODO: list variables containing software set-up
        Args:
            context: build context aggregated across all build steps
        Returns:
            context optionally augmented with plugin-specific preload data
        """
        try:
            self.port.open()
            self.wait_for_vxprompt()
            load_string = f"ld < {context['dkm_path']}"
            self.write_to_vxworks(load_string.encode("ascii"))
        except serial.SerialException as exception:
            raise Exception(f"Failed to use serial port: {exception}")
        return context


    def launch(self, context: dict):
        """ Launch the software on the target hardware

        This function must be overridden by platform developers to perform software launching actions. This might
        include running the executable via SSH, passing launch codes to a serial console, restarting the hardware, or
        nothing.

        There is no default implementation for this function, platforms with no explicit launching steps must supply
        a no-op function.

        Args:
            context: build context aggregated across all build steps
        """
        #TODO: wait for acknowledge
        try:
            self.wait_for_vxprompt()
            load_string = f"sp fsw_main(\"0.0.0.0\", 50000)"
            self.write_to_vxworks(load_string.encode("ascii"))
            self.wait_for_vxprompt("Accepted client")
            self.monitor_fsw_run()
        except serial.SerialException as exception:
            raise Exception(f"Failed to use serial port: {exception}")
        return context

    def cleanup(self, context: dict):
        """ Cleanup """
        try:
            if self.monitor_fsw_thread is not None:
                IOLogger.join_communicate(self.monitor_fsw_thread)
        finally:
            self.port.close()
        return context

    @classmethod
    def get_name(cls):
        """ Returns the name of the plugin """
        return "vxworks-dkm"

    @classmethod
    def get_arguments(cls):
        """ Returns the arguments of the plugin """
        return {
            ("--port",): {
                "type": str,
                "default": "/dev/ttyUSB0",
                "help": "Serial port used to communicate with VxWorks",
            },
            ("--baud",): {
                "type": int,
                "default": 115200,
                "help": "Baud rate for the serial interface",
            },
            ("--flow",): {
                "default": "no",
                "type": str,
                "help": "Whether to enable flow control on the serial interface. Default: no",
            }

        }
