import os
import unittest
import docker
import select
import time
import inspect
import sys
import tarfile
import io
import os
import requests
import argparse
import logging
import functools
import typing
import shutil

from docker.models.containers import Container
from docker.errors import ImageNotFound

from .node import NamespaceHandler
from .version import __version__
import subprocess
from subprocess import PIPE

MAKEFILE_SUCCESSFUL = 1
CMAKE_SUCCESSFUL = 2

RNVS_BUILDENV_VERSION = "v0.0.2"

def log_test(fun, lvl=logging.INFO):
    @functools.wraps(fun)
    def log_wrapper(*args, **kwargs):
        log = logging.getLogger(args[0].__class__.__name__)
        log.log(lvl, f"Running {fun.__name__}...")
        return fun(*args, **kwargs)

    return log_wrapper


def port_available(container, port):
    container.reload()
    pid = container.attrs['State']['Pid']
    ns = NamespaceHandler(pid)

    with ns:
        p = subprocess.run(['fuser', '-n', 'tcp', f'{port}'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    return not (p.returncode == 0)


def port_ready(container: Container, port: int, timeout=3):
    container.reload()
    pid = container.attrs['State']['Pid']
    ns = NamespaceHandler(pid)

    with ns:
        while True:
            res = subprocess.run(["lsof", f"-i:{port}", "-F", "cpT"], stdout=PIPE, stderr=PIPE)

            if res.returncode == 0: # port is open
                return True

            if timeout <= 0: # stop waiting
                return False

            timeout = timeout - 1
            time.sleep(1)


def volume_mountpoint(name):
    dcli = docker.from_env()
    vol = dcli.volumes.get(name)
    vol.reload()
    mountpoint = vol.attrs["Mountpoint"]
    return mountpoint


def copy_to_volume(path, volume_name):
    volumes = {volume_name: {'bind': '/mnt/src', 'mode': 'rw'}}
    dcli = docker.from_env()

    # Clear volume
    mountpoint = volume_mountpoint(volume_name)
    for filename in os.listdir(mountpoint):
        filepath = os.path.join(mountpoint, filename)
        try:
            shutil.rmtree(filepath)
        except OSError:
            os.remove(filepath)

    # Fill volume
    dummy = dcli.containers.create('ubuntu:bionic', 'echo hello world', volumes=volumes, name='dummy')
    mem = io.BytesIO()

    tf = tarfile.open(mode='w', fileobj=mem)
    tf.add(path, arcname='', recursive=True)
    tf.close()

    mem.seek(0)

    dummy.put_archive('/mnt/src', mem)

    dummy.remove(v=False)


def _show_descriptions():
    HEADER = '\033[95m'
    #    OKBLUE = '\033[94m'
    #    OKCYAN = '\033[96m'
    #    OKGREEN = '\033[92m'
    #    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    #    BOLD = '\033[1m'
    #    UNDERLINE = '\033[4m'

    caller_frame = inspect.stack()[2]  # NOTE: This assumes we are calling this function from run_tests()
    mod = inspect.getmodule(caller_frame[0])
    suite = unittest.defaultTestLoader.loadTestsFromModule(mod)

    testdescriptions = []
    for s in suite:
        for t in s:
            name = t._testMethodName
            header = f'{HEADER}{t.__class__.__name__}.{name}{ENDC}'
            descr = t._testMethodDoc if t._testMethodDoc is not None else f'{FAIL}<No description available>{ENDC}\n'
            testdescriptions.append('\n'.join([header, descr]))

    print('\n'.join(testdescriptions))


def run_tests(assignment, src_volume, pre=None, post=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--filter", help='Filter for groups starting with filter prefix', type=str)
    parser.add_argument("-p", "--path", help='Alternative path for assignment submissions', default='assignments',
                        type=str)
    parser.add_argument("-s", "--single", help='Run tests for a single submission at a given path', type=str)
    parser.add_argument("-i", "--ignore-missing", help='Ignore missing files in compile test', action='store_true')
    parser.add_argument("-d", "--show-description", help='Do not run any tests, just show their test description',
                        action='store_true')
    parser.add_argument("-v", "--version", help='Show testbench version and exit.',
                        action='store_true')

    args = parser.parse_args()

    if args.version:
        print(__version__)
        return

    if args.show_description:
        _show_descriptions()
        return

    if not os.geteuid() == 0:
        print("Only root can run this script. Try with sudo!")
        sys.exit(1)

    if args.single is not None:
        abspath = os.path.abspath(args.single)
        submissions = [(os.path.basename(abspath.rstrip(os.path.sep)), abspath)]
    else:
        root_dir = os.path.abspath(args.path)
        submissions = list(iter_submissions(assignment, root_dir=root_dir))

    endresult = {}

    log = logging.getLogger(__name__)
    logging.basicConfig(level=logging.INFO)

    def filter_fun(a):
        group, path = a
        if not args.filter:
            return True
        else:
            return group.startswith(args.filter)

    submissions = list(filter(filter_fun, submissions))

    try:
        if callable(pre):
            pre()

        for i, (group, path) in enumerate(submissions):
            result_dir = os.path.join('testresults', group)
            os.makedirs(result_dir, exist_ok=True)

            with open(os.path.join(result_dir, 'test.log'), 'w') as f:
                runner = unittest.TextTestRunner(stream=f, resultclass=TextTestResultWithSuccesses)

                log.info('Testing submission {} ({}/{})'.format(group,
                                                                i + 1,
                                                                len(submissions)))
                # quick work-around
                TKNTestCase.submission = group
                TKNTestCase.path = path
                CompileTest.ignore_missing = args.ignore_missing

                comp_suite = unittest.defaultTestLoader.loadTestsFromTestCase(CompileTest)

                caller_frame = inspect.stack()[1]
                mod = inspect.getmodule(caller_frame[0])
                suite = unittest.defaultTestLoader.loadTestsFromModule(mod)

                expected_files = set()
                for name, member in inspect.getmembers(mod,
                                                       lambda m: inspect.isclass(m) and
                                                                 issubclass(m, TKNTestCase) and
                                                                 TKNTestCase.__name__ != m.__name__):
                    expected_files.update(set(member.expected_files))

                CompileTest.expected_files = expected_files

                endresult[group] = {}
                endresult[group]['Compiled'] = '-'
                for s in suite:
                    for test in s._tests:
                        endresult[group][test.id()] = '-'

                res = runner.run(comp_suite)
                if not res.wasSuccessful():
                    # Don't bother to run tests if compile fails
                    log.info(f'Failed to compile {group} 💩 See logs in {result_dir} for details.')
                    endresult[group]['Compiled'] = 'x'
                    continue

                endresult[group]['Compiled'] = 'o'

                # Copy source code into volume before we run the code
                copy_to_volume(path, src_volume)

                res: TextTestResultWithSuccesses = runner.run(suite)

                fails = res.failures
                fails.extend(res.errors)

                for case, tb in fails:
                    endresult[group][case.id()] = 'x'
                    log.info('Failed {}:'.format(case.id()))
                    log.info(tb)
                for case, reason in res.skipped:
                    log.info(f'Skipped {case.id()}: {reason}')
                for case in res.successes:
                    endresult[group][case.id()] = 'o'

                if res.wasSuccessful() and len(res.skipped) == 0:
                    log.info(f'{group} passed all tests 😊')
                elif res.wasSuccessful():
                    log.info(f'{group} passed all tests, but we skipped some! 😊')

        res_path = 'testresults/results.csv' if not args.filter else f'testresults/results_{args.filter}.csv'
        with open(res_path, 'w') as f:
            header = True
            for group in sorted(endresult.keys()):
                if header:
                    header = False
                    cols = ['group']
                    cols.extend(sorted(endresult[group].keys()))
                    line = ','.join(cols)
                    f.write(line + '\n')

                cols = [group]
                cols.extend(endresult[group][t] for t in sorted(endresult[group].keys()))
                line = ','.join(cols)
                f.write(line + '\n')

        subprocess.run(['chown', '-R', f'{os.getenv("SUDO_UID")}:{os.getenv("SUDO_GID")}', 'testresults'])

    finally:
        log.info("All test runs finished. Please wait while we clean up!")
        if callable(post):
            post()


def which(searchpath, cmd):
    make_path = os.path.join(searchpath, cmd)
    cmake_path = os.path.join(searchpath, 'build', cmd)
    if os.path.isfile(cmake_path):
        path = os.path.join('/mnt', 'src', 'build', cmd)
    elif os.path.isfile(make_path):
        path = os.path.join('/mnt', 'src', cmd)
    else:
        raise OSError('File "{}" not found!'.format(cmd))

    return path


def convert_to_printf(byteseq=b''):
    seq = []
    for b in byteseq:
        seq.append('\\x{b:02x}'.format(b=b))
    return ''.join(seq)


class ExecAsyncHandler:
    def __init__(self,cli,  dsock, exec_id):
        self.cli = cli
        self.dsock = dsock
        self.exec_id = exec_id

    def collect(self, timeout=5.0):
        res = None
        out = bytearray(0)
        err = bytearray(0)

        terminated = False
        start = time.time()
        deadline = start + timeout

        while time.time() < deadline:

            rlist, _, _ = select.select([self.dsock], [], [], 0.5)
            if self.dsock not in rlist:
                break

            stream, size = docker.utils.socket.next_frame_header(self.dsock)
            data = docker.utils.socket.read_exactly(self.dsock, size)
            if not data:
                terminated = True
                break

            if stream == 1:
                out.extend(data)
            elif stream == 2:
                err.extend(data)

        self.dsock.close()
        if terminated:
            exec_info = self.cli.exec_inspect(self.exec_id)
            res = exec_info['ExitCode']

        return res, out, err


def exec_async(container: Container, cmd, stdin=None, attach_stdin=True, **kwargs):
    cli = docker.APIClient(base_url='unix://var/run/docker.sock')

    cmd_str = " ".join(map(lambda s: f'"{s}"' if " " in s else s, cmd))
    if stdin is None:
        exec_id = cli.exec_create(container.id, cmd_str, stdin=attach_stdin, **kwargs)
    else:
        hexstr = convert_to_printf(stdin)
        extended_cmd = ['bash', '-c', f"printf '{hexstr}' | {cmd_str}"]
        exec_id = cli.exec_create(container.id, extended_cmd, stdin=True, **kwargs)

    dsock = cli.exec_start(exec_id, socket=True)
    return ExecAsyncHandler(cli, dsock, exec_id)


def exec_timeout(container: Container, cmd, timeout, stdin=None, attach_stdin=True, **kwargs):
    cli = docker.APIClient(base_url='unix://var/run/docker.sock')

    cmd_str = " ".join(map(lambda s: f'"{s}"' if " " in s else s, cmd))
    if stdin is None:
        exec_id = cli.exec_create(container.id, cmd_str, stdin=attach_stdin, **kwargs)
    else:
        hexstr = convert_to_printf(stdin)
        extended_cmd = ['bash', '-c', f"printf '{hexstr}' | {cmd_str}"]
        exec_id = cli.exec_create(container.id, extended_cmd, stdin=True, **kwargs)


    dsock = cli.exec_start(exec_id, socket=True)

    res = None
    out = bytearray(0)
    err = bytearray(0)

    terminated = False
    start = time.time()
    deadline = start + timeout

    while time.time() < deadline:

        rlist, _, _ = select.select([dsock], [], [], 0.5)
        if dsock not in rlist:
            continue

        stream, size = docker.utils.socket.next_frame_header(dsock)
        data = docker.utils.socket.read_exactly(dsock, size)
        if not data:
            terminated = True
            break

        if stream == 1:
            out.extend(data)
        elif stream == 2:
            err.extend(data)

    dsock.close()
    if terminated:
        exec_info = cli.exec_inspect(exec_id)
        res = exec_info['ExitCode']

    return res, out, err


def iter_submissions(assignment, root_dir=None):
    if root_dir is None:
        root_dir = os.path.join(os.getcwd(), 'assignments')
    base = '{}/{}/submissions'.format(root_dir, assignment)
    for p in os.listdir(base):

        if not os.path.isdir(os.path.join(base, p)):
            continue

        submission_path = os.path.join(base, p)

        # Workaround for people who submit just a folder in the archives TLD
        # We have to check for 'visible' files only because Apple puts random
        # hidden folders EVERYWHERE...

        visible_folders = list(filter(lambda n: not n.startswith('.'),
                                      os.listdir(submission_path)))
        if len(visible_folders) == 1 and os.path.isdir(os.path.join(submission_path, visible_folders[0])):
            yield p, os.path.join(submission_path, os.listdir(submission_path)[0])
        else:
            yield p, submission_path


def prepare_buildenv(dockercli):
    log = logging.getLogger(__name__)
    try:
        dockercli.images.get(f"rnvs-buildenv:{RNVS_BUILDENV_VERSION}")
    except ImageNotFound:
        log.info("Build container not found. Building it now...")
        try:
            import importlib.resources as resources
            with resources.open_binary('rnvs_tb', 'Dockerfile_buildenv') as dockerfile:
                dockercli.images.build(fileobj=dockerfile, tag=f"rnvs-buildenv:{RNVS_BUILDENV_VERSION}")
        except ImportError:
            import pkg_resources
            dockerfile = pkg_resources.resource_stream('rnvs_tb', 'Dockerfile_buildenv')
            try:
                dockercli.images.build(fileobj=dockerfile, tag=f"rnvs-buildenv:{RNVS_BUILDENV_VERSION}")
            finally:
                dockerfile.close()



def compile_make(submission, path, compile_log):
    dockercli = docker.from_env()

    prepare_buildenv(dockercli)
    with open(compile_log, 'wb') as f:
        subprocess.run(['chown', '-R', '1000:1000', path])
        volumes = {path: {'bind': '/mnt/src', 'mode': 'rw'}}
        container = dockercli.containers.run(f'rnvs-buildenv:{RNVS_BUILDENV_VERSION}',
                                             'make',
                                             detach=True,
                                             working_dir='/mnt/src',
                                             volumes=volumes,
                                             network_mode='none',
                                             remove=False)

        try:
            returncode = container.wait(timeout=10)['StatusCode']
            f.write(container.logs())
        except requests.exceptions.ReadTimeout:
            f.write(b'Build container timed out!\n')
            container.stop()
        finally:
            container.remove()
            subprocess.run(['chown', '-R', f'{os.getenv("SUDO_UID")}:{os.getenv("SUDO_GID")}', path])

        if returncode != 0:
            return -1

        return MAKEFILE_SUCCESSFUL


def compile_cmake(submission, path, log_path):
    dockercli = docker.from_env()
    log = logging.getLogger(__name__)

    prepare_buildenv(dockercli)
    with open(log_path, 'wb') as f:
        subprocess.run(['chown', '-R', '1000:1000', path])
        volumes = {path: {'bind': '/mnt/src', 'mode': 'rw'}}

        build_script = ["sed --in-place=_orig '/cmake_minimum_required/d' CMakeLists.txt;",
                        "mkdir -p build &&",
                        "cd build &&",
                        "rm -rf * &&",
                        "cmake .. &&",
                        "make"]

        container = dockercli.containers.run(f'rnvs-buildenv:{RNVS_BUILDENV_VERSION}',
                                             ['bash', '-c',
                                              " ".join(build_script)],
                                             detach=True,
                                             working_dir='/mnt/src/',
                                             volumes=volumes,
                                             network_mode='none',
                                             remove=False)

        try:
            returncode = container.wait(timeout=10)['StatusCode']
            f.write(container.logs())
        except requests.exceptions.ReadTimeout:
            f.write(b'Build container timed out!\n')
            container.stop()
        finally:
            container.remove()
            subprocess.run(['chown', '-R', f'{os.getenv("SUDO_UID")}:{os.getenv("SUDO_GID")}', path])
            os.rename(os.path.join(path, 'CMakeLists.txt_orig'), os.path.join(path, 'CMakeLists.txt'))

        if returncode != 0:
            log.error('CMake failed! Returncode is non-zero!')
            return -1

        return CMAKE_SUCCESSFUL


def compile(submission, path, log_path):
    files = os.listdir(path)
    log = logging.getLogger(__name__)
    if 'Makefile' in files and os.path.isfile(os.path.join(path, 'Makefile')):
        log.warning('You are using a Makefile instead of CMake to build. I hope this is intentional!')
        return compile_make(submission, path, log_path)
    if 'CMakeLists.txt' in files and os.path.isfile(os.path.join(path, 'CMakeLists.txt')):
        return compile_cmake(submission, path, log_path)
    else:
        raise ValueError(f'No Makefile or CMakeLists.txt found in {path}. Aborting!')


class TextTestResultWithSuccesses(unittest.TextTestResult):
    def __init__(self, stream: typing.TextIO, descriptions: bool, vebosity: int):
        super(TextTestResultWithSuccesses, self).__init__(stream, descriptions, vebosity)
        self.successes = []

    def addSuccess(self, test):
        super(TextTestResultWithSuccesses, self).addSuccess(test)
        self.successes.append(test)


class TKNTestCase(unittest.TestCase):
    submission = None
    path = None
    expected_files = None

    @classmethod
    def setUpClass(cls):
        for binary in cls.expected_files:
            try:
                which(cls.path, binary)
            except OSError:
                raise unittest.SkipTest(f'Could not find "{binary}" executable!')


class CompileTest(TKNTestCase):
    ignore_missing = False

    @classmethod
    def setUpClass(cls):
        pass  # Important!: THis must be empty to override file check!

    def setUp(self):
        pass

    def test_compile(self):
        log = logging.getLogger(__name__)
        compile_log = os.path.join('testresults', self.submission, 'compile.log')
        result = compile(self.submission, self.path, compile_log)
        self.assertIn(result, [MAKEFILE_SUCCESSFUL, CMAKE_SUCCESSFUL], msg='Failed to compile binaries!')

        for f in self.expected_files:
            fullpath = os.path.join(self.path, 'build', f) if result == CMAKE_SUCCESSFUL else os.path.join(self.path, f)
            found_file = os.path.isfile(fullpath)
            if not self.ignore_missing:
                self.assertTrue(found_file,
                                msg=f'Did not find expected file "{fullpath}" after compiling!')
            elif not found_file:
                log.warning(f'Ignoring missing file that was expected: {fullpath}')

    def tearDown(self):
        pass

