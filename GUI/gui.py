"""
gui.py - Python (Tkinter) FRONTEND for the C++ Traffic Fine System.

Run:   python3 gui.py

All real work (fine calculation, linked list, hash table, stack, queue,
sorting, file handling) is done by the C++ program. This window only
collects input, shows results, and draws the dashboard. backend.py is the
bridge that talks to the C++ program.
"""
import datetime
import tkinter as tk
from tkinter import ttk, messagebox

from backend import TrafficBackend, BackendError, VEHICLE_TYPES, VIOLATION_TYPES

NAVY = "#1B2A41"
SLATE = "#3E5C76"
AMBER = "#F4A300"
RED = "#E63946"
GREEN = "#2E7D32"
BG = "#F5F6F8"
CARD = "#FFFFFF"
INK = "#1F2937"
MUTED = "#6B7280"

DEMO_VEHICLES = [
    ("Two-Wheeler", "UP16AB1234", "Abhay", "9999999999"),
    ("Four-Wheeler", "DL01XY9999", "Akhil", "8888888888"),
    ("Commercial", "UK07CD4321", "Divyanshi", "7777777777"),
    ("Two-Wheeler", "UK08EF5555", "Rakshit", "6666666666"),
]
DEMO_VIOLATIONS = [
    ("UP16AB1234", "Speeding", "2026-10-01"),
    ("UP16AB1234", "No Helmet", "2026-10-03"),
    ("UP16AB1234", "Speeding", "2026-10-07"),
    ("DL01XY9999", "Signal Jump", "2026-10-05"),
    ("UK07CD4321", "Wrong Parking", "2026-10-02"),
    ("UK07CD4321", "Wrong Parking", "2026-10-06"),
]


def money(n):
    return "Rs.{:,}".format(n)


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Intelligent Traffic Fine & Violation Management System")
        self.geometry("1200x850")
        self.minsize(1080, 800)
        self.configure(bg=BG)

        self.backend = TrafficBackend()
        self._build_styles()
        self._build_header()
        self._build_status()      # packed before the body so it always stays visible
        self._build_body()
        self.protocol("WM_DELETE_WINDOW", self.on_close)
        self._start_backend()

    # ------------------------------------------------------------------
    # layout
    # ------------------------------------------------------------------
    def _build_styles(self):
        s = ttk.Style(self)
        s.theme_use("clam")
        s.configure("TFrame", background=BG)
        s.configure("Card.TFrame", background=CARD)
        s.configure("Card.TLabel", background=CARD, foreground=INK, font=("Helvetica", 10))
        s.configure("Title.TLabel", background=CARD, foreground=NAVY, font=("Helvetica", 12, "bold"))
        s.configure("TEntry", padding=4)
        s.configure("TCombobox", padding=4)
        for name, bg, fg, hover in [
            ("Navy.TButton", NAVY, "white", SLATE),
            ("Amber.TButton", AMBER, NAVY, "#FFB926"),
            ("Red.TButton", RED, "white", "#F25C69"),
            ("Green.TButton", GREEN, "white", "#3A9A40"),
        ]:
            s.configure(name, background=bg, foreground=fg, borderwidth=0,
                        font=("Helvetica", 10, "bold"), padding=(10, 7))
            s.map(name, background=[("active", hover)])

    def _build_header(self):
        bar = tk.Frame(self, bg=NAVY, height=74)
        bar.pack(fill="x")
        bar.pack_propagate(False)
        dots = tk.Canvas(bar, width=70, height=20, bg=NAVY, highlightthickness=0)
        dots.place(x=24, y=14)
        for i, c in enumerate([RED, AMBER, GREEN]):
            dots.create_oval(2 + i * 22, 3, 16 + i * 22, 17, fill=c, outline=c)
        tk.Label(bar, text="Intelligent Traffic Fine & Violation Management System",
                 bg=NAVY, fg="white", font=("Helvetica", 17, "bold")).place(x=24, y=34)
        tk.Label(bar, text="C++ backend (OOP + hand-built DSA)  \u00b7  Python Tkinter frontend",
                 bg=NAVY, fg="#CADCFC", font=("Helvetica", 10)).place(x=112, y=12)
        ttk.Button(bar, text="Load demo data", style="Amber.TButton",
                   command=self.load_demo).place(relx=1.0, x=-24, y=30, anchor="ne")

    def _card(self, parent, title):
        outer = tk.Frame(parent, bg=CARD, highlightbackground="#E5E7EB", highlightthickness=1)
        ttk.Label(outer, text=title, style="Title.TLabel").pack(anchor="w", padx=14, pady=(8, 2))
        inner = ttk.Frame(outer, style="Card.TFrame")
        inner.pack(fill="x", padx=14, pady=(0, 10))
        return outer, inner

    def _field(self, frame, row, label, widget):
        ttk.Label(frame, text=label, style="Card.TLabel").grid(row=row, column=0, sticky="w", pady=3)
        widget.grid(row=row, column=1, sticky="ew", pady=3, padx=(10, 0))
        frame.columnconfigure(1, weight=1)

    def _build_body(self):
        body = ttk.Frame(self)
        body.pack(fill="both", expand=True, padx=18, pady=14)
        body.columnconfigure(0, weight=0, minsize=400)
        body.columnconfigure(1, weight=1)
        body.rowconfigure(0, weight=1)

        # ---------------- left: forms ----------------
        left = ttk.Frame(body)
        left.grid(row=0, column=0, sticky="nsew", padx=(0, 14))

        # register
        outer, f = self._card(left, "1. Register vehicle")
        outer.pack(fill="x", pady=(0, 10))
        self.reg_type = ttk.Combobox(f, values=list(VEHICLE_TYPES), state="readonly")
        self.reg_type.current(0)
        self.reg_no = ttk.Entry(f)
        self.reg_owner = ttk.Entry(f)
        self.reg_phone = ttk.Entry(f)
        self._field(f, 0, "Type", self.reg_type)
        self._field(f, 1, "Reg. number", self.reg_no)
        self._field(f, 2, "Owner name", self.reg_owner)
        self._field(f, 3, "Phone", self.reg_phone)
        ttk.Button(f, text="Register vehicle", style="Navy.TButton",
                   command=self.do_register).grid(row=4, column=0, columnspan=2, sticky="ew", pady=(8, 0))

        # violation
        outer, f = self._card(left, "2. Log violation")
        outer.pack(fill="x", pady=(0, 10))
        self.vio_reg = ttk.Entry(f)
        self.vio_type = ttk.Combobox(f, values=list(VIOLATION_TYPES), state="readonly")
        self.vio_type.current(0)
        self.vio_date = ttk.Entry(f)
        self.vio_date.insert(0, datetime.date.today().isoformat())
        self._field(f, 0, "Reg. number", self.vio_reg)
        self._field(f, 1, "Violation", self.vio_type)
        self._field(f, 2, "Date", self.vio_date)
        ttk.Button(f, text="Log violation", style="Red.TButton",
                   command=self.do_log).grid(row=3, column=0, columnspan=2, sticky="ew", pady=(8, 0))

        # search
        outer, f = self._card(left, "3. Search vehicle")
        outer.pack(fill="x", pady=(0, 10))
        self.search_reg = ttk.Entry(f)
        self._field(f, 0, "Reg. number", self.search_reg)
        ttk.Button(f, text="Show details & history", style="Navy.TButton",
                   command=self.do_search).grid(row=1, column=0, columnspan=2, sticky="ew", pady=(8, 0))

        # payment
        outer, f = self._card(left, "4. Pay fines  (queue in C++)")
        outer.pack(fill="x")
        ttk.Button(f, text="Pay oldest pending fine", style="Green.TButton",
                   command=self.do_pay).grid(row=0, column=0, sticky="ew")
        f.columnconfigure(0, weight=1)

        # ---------------- right: dashboard ----------------
        right = ttk.Frame(body)
        right.grid(row=0, column=1, sticky="nsew")
        right.columnconfigure(0, weight=1)
        right.rowconfigure(1, weight=1)

        stats = ttk.Frame(right)
        stats.grid(row=0, column=0, sticky="ew")
        self.stat_vars = {}
        spec = [("vehicles", "Vehicles", NAVY), ("violations", "Violations", NAVY),
                ("issued", "Fines issued", NAVY), ("collected", "Collected", GREEN),
                ("pending", "Pending", RED)]
        for i, (key, caption, color) in enumerate(spec):
            stats.columnconfigure(i, weight=1)
            box = tk.Frame(stats, bg=CARD, highlightbackground="#E5E7EB", highlightthickness=1)
            box.grid(row=0, column=i, sticky="ew", padx=(0 if i == 0 else 8, 0))
            var = tk.StringVar(value="0")
            self.stat_vars[key] = var
            tk.Label(box, textvariable=var, bg=CARD, fg=color,
                     font=("Helvetica", 17, "bold")).pack(pady=(10, 0))
            tk.Label(box, text=caption, bg=CARD, fg=MUTED,
                     font=("Helvetica", 9)).pack(pady=(0, 10))

        logbox = tk.Frame(right, bg=CARD, highlightbackground="#E5E7EB", highlightthickness=1)
        logbox.grid(row=1, column=0, sticky="nsew", pady=(12, 0))
        ttk.Label(logbox, text="Activity log  (responses from the C++ program)",
                  style="Title.TLabel").pack(anchor="w", padx=14, pady=(10, 6))
        wrap = ttk.Frame(logbox, style="Card.TFrame")
        wrap.pack(fill="both", expand=True, padx=14, pady=(0, 12))
        self.log_text = tk.Text(wrap, height=8, wrap="word", font=("Courier", 10), bg="#FBFBFC",
                                fg=INK, relief="flat", state="disabled", padx=8, pady=6)
        scroll = ttk.Scrollbar(wrap, command=self.log_text.yview)
        self.log_text.configure(yscrollcommand=scroll.set)
        scroll.pack(side="right", fill="y")
        self.log_text.pack(side="left", fill="both", expand=True)
        self.log_text.tag_configure("ok", foreground=GREEN)
        self.log_text.tag_configure("err", foreground=RED)
        self.log_text.tag_configure("info", foreground=INK)
        self.log_text.tag_configure("time", foreground=MUTED)

    def _build_status(self):
        self.status = tk.StringVar(value="Starting C++ backend...")
        tk.Label(self, textvariable=self.status, bg=NAVY, fg="#CADCFC", anchor="w",
                 font=("Helvetica", 9), padx=18, pady=5).pack(fill="x", side="bottom")

    # ------------------------------------------------------------------
    # helpers
    # ------------------------------------------------------------------
    def log(self, message, kind="info"):
        stamp = datetime.datetime.now().strftime("%H:%M:%S")
        self.log_text.configure(state="normal")
        self.log_text.insert("end", stamp + "  ", "time")
        self.log_text.insert("end", message.replace("\n", "\n          ") + "\n", kind)
        self.log_text.see("end")
        self.log_text.configure(state="disabled")

    def refresh(self):
        rep = self.backend.report()
        self.stat_vars["vehicles"].set(str(rep["vehicles"]))
        self.stat_vars["violations"].set(str(rep["violations"]))
        self.stat_vars["issued"].set(money(rep["issued"]))
        self.stat_vars["collected"].set(money(rep["collected"]))
        self.stat_vars["pending"].set(money(rep["pending"]))

    def _start_backend(self):
        try:
            self.backend.start()
        except BackendError as e:
            messagebox.showerror("Backend error", str(e))
            self.status.set("Backend failed to start")
            return
        self.log(self.backend.startup_message or "Backend started.", "info")
        self.status.set("C++ backend running  \u00b7  records are saved when you close this window")
        self.refresh()

    def _run(self, func, *args, kind_on_ok="ok"):
        try:
            msg = func(*args)
        except BackendError as e:
            self.log(str(e), "err")
            self.status.set("Backend error")
            return None
        failed = TrafficBackend.is_error(msg)
        self.log(msg, "err" if failed else kind_on_ok)
        self.status.set("Failed - see log" if failed else "Done")
        self.refresh()
        return not failed

    # ------------------------------------------------------------------
    # button handlers
    # ------------------------------------------------------------------
    def do_register(self):
        reg, owner, phone = self.reg_no.get().strip(), self.reg_owner.get().strip(), self.reg_phone.get().strip()
        if not (reg and owner and phone):
            self.log("Error: fill in registration number, owner name and phone.", "err")
            return
        ok = self._run(self.backend.register_vehicle, self.reg_type.get(), reg, owner, phone)
        if ok:
            for e in (self.reg_no, self.reg_owner, self.reg_phone):
                e.delete(0, "end")

    def do_log(self):
        reg, date = self.vio_reg.get().strip(), self.vio_date.get().strip()
        if not reg:
            self.log("Error: enter the vehicle registration number.", "err")
            return
        try:
            datetime.datetime.strptime(date, "%Y-%m-%d")
        except ValueError:
            self.log("Error: date must be in YYYY-MM-DD format.", "err")
            return
        self._run(self.backend.log_violation, reg, self.vio_type.get(), date)

    def do_search(self):
        reg = self.search_reg.get().strip()
        if not reg:
            self.log("Error: enter a registration number to search.", "err")
            return
        self._run(self.backend.search, reg, kind_on_ok="info")

    def do_pay(self):
        self._run(self.backend.pay_oldest)

    def load_demo(self):
        for vtype, reg, owner, phone in DEMO_VEHICLES:
            self._run(self.backend.register_vehicle, vtype, reg, owner, phone)
        for reg, vio, date in DEMO_VIOLATIONS:
            self._run(self.backend.log_violation, reg, vio, date)
        self.status.set("Demo data loaded through the C++ backend")

    def on_close(self):
        self.backend.stop()
        self.destroy()


if __name__ == "__main__":
    App().mainloop()
