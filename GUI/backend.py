import os
import queue
import re
import subprocess
import sys
import threading

PROMPT = b"Enter choice: "         
MENU_MARK = "===== TRAFFIC FINE"    

# Prompts that the C++ program prints while asking for details
PROMPT_TEXTS = [
    "Type (1=Two-Wheeler, 2=Four-Wheeler, 3=Commercial): ",
    "Registration number: ",
    "Owner name (one word): ",
    "Phone: ",
    "Violation (1=Speeding, 2=Signal Jump, 3=No Helmet, 4=Wrong Parking): ",
    "Date (YYYY-MM-DD): ",
]

VEHICLE_TYPES = {"Two-Wheeler": 1, "Four-Wheeler": 2, "Commercial": 3}
VIOLATION_TYPES = {"Speeding": 1, "Signal Jump": 2, "No Helmet": 3, "Wrong Parking": 4}


class BackendError(Exception):
    pass


def one_word(text):
    """The C++ input uses cin >> word, so spaces would break the protocol."""
    return re.sub(r"\s+", "_", text.strip())


class TrafficBackend:
    def __init__(self, root=None):
        here = os.path.dirname(os.path.abspath(__file__))
        self.root = root or os.path.dirname(here)
        self.src_dir = os.path.join(self.root, "src")
        self.proc = None
        self._chunks = queue.Queue()
        self.startup_message = ""

    # starting / stopping the C++ program
    def _exe_path(self):
        name = "traffic.exe" if sys.platform.startswith("win") else "traffic"
        return os.path.join(self.root, "build", name)

    def _ensure_built(self):
        """Compile src/main.cpp into build/traffic if it is missing or older than the sources."""
        exe = self._exe_path()
        main_cpp = os.path.join(self.src_dir, "main.cpp")
        if not os.path.exists(main_cpp):
            raise BackendError("src/main.cpp not found in " + self.root)
        sources = [os.path.join(self.src_dir, f) for f in os.listdir(self.src_dir)
                   if f.endswith((".h", ".cpp"))]
        newest = max(os.path.getmtime(f) for f in sources)
        if os.path.exists(exe) and os.path.getmtime(exe) >= newest:
            return
        os.makedirs(os.path.dirname(exe), exist_ok=True)
        try:
            result = subprocess.run(
                ["g++", "-std=c++11", os.path.join("src", "main.cpp"), "-o", os.path.relpath(exe, self.root)],
                cwd=self.root, capture_output=True, text=True)
        except FileNotFoundError:
            raise BackendError("g++ was not found. Install a C++ compiler (for example MinGW on Windows).")
        if result.returncode != 0:
            raise BackendError("C++ compile failed:\n" + result.stderr)

    def start(self):
        self._ensure_built()
        self.proc = subprocess.Popen(
            [self._exe_path()], cwd=self.root,
            stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT, bufsize=0)
        threading.Thread(target=self._reader, daemon=True).start()
        first = self._read_until_prompt()
        self.startup_message = self._clean(first)

    def stop(self):
        """Equivalent of menu option 0: saves records.txt and exits."""
        if self.proc is None:
            return ""
        msg = ""
        try:
            self.proc.stdin.write(b"0\n")
            self.proc.stdin.flush()
            self.proc.stdin.close()
            self.proc.wait(timeout=5)
            msg = self._drain_text()
        except Exception:
            self.proc.kill()
        self.proc = None
        return msg.strip()

    # low-level talking
    def _reader(self):
        proc = self.proc
        while True:
            data = os.read(proc.stdout.fileno(), 4096)
            if not data:
                break
            self._chunks.put(data)
        self._chunks.put(None)        

    def _read_until_prompt(self, timeout=5.0):
        buf = b""
        while not buf.endswith(PROMPT):
            try:
                chunk = self._chunks.get(timeout=timeout)
            except queue.Empty:
                raise BackendError("C++ program did not respond (timeout).")
            if chunk is None:
                raise BackendError("C++ program closed unexpectedly.")
            buf += chunk
        return buf.decode("utf-8", errors="replace")

    def _drain_text(self):
        out = b""
        while True:
            try:
                chunk = self._chunks.get(timeout=1.0)
            except queue.Empty:
                break
            if chunk is None:
                break
            out += chunk
        return out.decode("utf-8", errors="replace")

    @staticmethod
    def _clean(raw):
        """Keep only the result lines: drop the menu block and the prompts."""
        cut = raw.find(MENU_MARK)
        text = raw if cut == -1 else raw[:cut]
        for p in PROMPT_TEXTS:
            text = text.replace(p, "")
        return text.strip()

    def _command(self, lines):
        if self.proc is None:
            raise BackendError("Backend is not running.")
        payload = "\n".join(str(x) for x in lines) + "\n"
        self.proc.stdin.write(payload.encode("utf-8"))
        self.proc.stdin.flush()
        return self._clean(self._read_until_prompt())

    # one method per menu option
    def register_vehicle(self, vehicle_type, reg, owner, phone):
        return self._command([1, VEHICLE_TYPES[vehicle_type],
                              one_word(reg), one_word(owner), one_word(phone)])

    def log_violation(self, reg, violation_type, date):
        return self._command([2, one_word(reg), VIOLATION_TYPES[violation_type],
                              one_word(date)])

    def search(self, reg):
        return self._command([3, one_word(reg)])

    def pay_oldest(self):
        return self._command([4])

    def report_text(self):
        return self._command([5])

    def list_all(self):
        return self._command([6])

    # parsed versions (used by the dashboard)
    def report(self):
        text = self.report_text()
        keys = {
            "vehicles": r"Vehicles registered : (\d+)",
            "violations": r"Violations issued   : (\d+)",
            "issued": r"Total fines issued  : Rs\.(\d+)",
            "collected": r"Collected \(paid\)    : Rs\.(\d+)",
            "pending": r"Pending \(unpaid\)    : Rs\.(\d+)",
        }
        out = {}
        for k, pat in keys.items():
            m = re.search(pat, text)
            out[k] = int(m.group(1)) if m else 0
        return out

    @staticmethod
    def is_error(message):
        return message.startswith("Error:") or message.startswith("Invalid") \
            or message.startswith("No pending")
