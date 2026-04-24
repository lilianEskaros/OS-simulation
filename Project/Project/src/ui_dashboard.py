import customtkinter as ctk
import subprocess
import threading
import re
import os

# --- Configuration ---
EXECUTABLE_NAME = "./os_simulator.exe"

ctk.set_appearance_mode("Dark")  
ctk.set_default_color_theme("blue")  

class ModernOSDashboard(ctk.CTk):
    def __init__(self):
        super().__init__()
        
        self.title("Modern OS Simulator Dashboard")
        self.geometry("1280x880")
        
        self.cycles = [{"cycle_num": "0 (Init)", "events": [], "queues": [], "memory": []}]
        self.current_cycle_index = 0
        self.in_memory_block = False
        self.is_viewing_history = False 
        self.is_autoplay = False
        
        self.setup_ui()

    def setup_ui(self):
        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(4, weight=1) 

        # --- Top Header Frame ---
        header_frame = ctk.CTkFrame(self, fg_color="transparent")
        header_frame.grid(row=0, column=0, padx=20, pady=(10, 5), sticky="ew")
        header_frame.grid_columnconfigure(1, weight=1)

        self.title_lbl = ctk.CTkLabel(header_frame, text="OS Offline. Please Boot System.", font=ctk.CTkFont(size=24, weight="bold"))
        self.title_lbl.grid(row=0, column=1, pady=0)

        btn_frame = ctk.CTkFrame(header_frame, fg_color="transparent")
        btn_frame.grid(row=1, column=1, pady=0)
        
        self.prev_btn = ctk.CTkButton(btn_frame, text="◀ Previous Cycle", command=self.prev_cycle, width=130, state="disabled")
        self.prev_btn.pack(side="left", padx=10)
        
        self.play_btn = ctk.CTkButton(btn_frame, text="▶ Auto-Play", command=self.toggle_autoplay, width=130, fg_color="#f39c12", hover_color="#e67e22", state="disabled")
        self.play_btn.pack(side="left", padx=10)

        self.next_btn = ctk.CTkButton(btn_frame, text="Next Cycle ▶", command=self.next_cycle, width=130, state="disabled")
        self.next_btn.pack(side="left", padx=10)

        # --- Boot Menu Bar ---
        self.boot_frame = ctk.CTkFrame(self, fg_color="#8e44ad", corner_radius=8)
        self.boot_frame.grid(row=1, column=0, padx=20, pady=(0, 5), sticky="ew")
        self.boot_frame.grid_columnconfigure(2, weight=1)

        ctk.CTkLabel(self.boot_frame, text="⚙️ Select Scheduler:", font=ctk.CTkFont(size=14, weight="bold"), text_color="white").grid(row=0, column=0, padx=20, pady=4)
        
        self.scheduler_var = ctk.StringVar(value="3 (MLFQ)")
        self.scheduler_dropdown = ctk.CTkComboBox(self.boot_frame, variable=self.scheduler_var, values=["1 (Round Robin)", "2 (HRRN)", "3 (MLFQ)"], state="readonly", width=200)
        self.scheduler_dropdown.grid(row=0, column=1, padx=10, pady=4)

        self.boot_btn = ctk.CTkButton(self.boot_frame, text="⚡ BOOT OS", command=self.start_simulation, fg_color="#2ecc71", hover_color="#27ae60", font=ctk.CTkFont(weight="bold"))
        self.boot_btn.grid(row=0, column=3, padx=20, pady=4)

        # --- Live Input Bar ---
        self.input_frame = ctk.CTkFrame(self, fg_color="#1f538d", corner_radius=8)
        self.input_frame.grid(row=2, column=0, padx=20, pady=(0, 5), sticky="ew")
        self.input_frame.grid_columnconfigure(1, weight=1)

        self.status_lbl = ctk.CTkLabel(self.input_frame, text="SYSTEM OFFLINE", font=ctk.CTkFont(size=14, weight="bold"), text_color="white")
        self.status_lbl.grid(row=0, column=0, padx=20, pady=4)

        self.input_var = ctk.StringVar()
        self.input_entry = ctk.CTkEntry(self.input_frame, textvariable=self.input_var, font=ctk.CTkFont(size=14), placeholder_text="Waiting for OS prompt...", state="disabled")
        self.input_entry.grid(row=0, column=1, padx=(0, 20), pady=4, sticky="ew")
        self.input_entry.bind("<Return>", self.submit_input)
        
        self.input_btn = ctk.CTkButton(self.input_frame, text="Submit", command=self.submit_input, state="disabled", fg_color="#27ae60", hover_color="#2ecc71")
        self.input_btn.grid(row=0, column=2, padx=20, pady=4)

        # --- SYSTEM HUD (Running Process & Swap Indicator) ---
        self.hud_frame = ctk.CTkFrame(self, fg_color="#2c3e50", corner_radius=8)
        self.hud_frame.grid(row=3, column=0, padx=20, pady=(0, 5), sticky="ew")
        self.hud_frame.grid_columnconfigure(1, weight=1)
        
        self.running_lbl = ctk.CTkLabel(self.hud_frame, text="⚙️ Running Process: NONE", font=ctk.CTkFont(size=15, weight="bold"), text_color="white")
        self.running_lbl.grid(row=0, column=0, padx=20, pady=6, sticky="w")
        
        self.swap_lbl = ctk.CTkLabel(self.hud_frame, text="💽 Disk Activity: IDLE", font=ctk.CTkFont(size=15, weight="bold"), text_color="#f1c40f")
        self.swap_lbl.grid(row=0, column=2, padx=20, pady=6, sticky="e")

        # --- Main Content Panels ---
        content_frame = ctk.CTkFrame(self, fg_color="transparent")
        content_frame.grid(row=4, column=0, padx=20, pady=(5, 10), sticky="nsew")
        content_frame.grid_columnconfigure((0, 1), weight=2)
        content_frame.grid_columnconfigure(2, weight=3)
        content_frame.grid_rowconfigure(0, weight=1)

        events_frame = ctk.CTkFrame(content_frame)
        events_frame.grid(row=0, column=0, padx=(0, 10), sticky="nsew")
        events_frame.grid_rowconfigure(1, weight=1)
        events_frame.grid_columnconfigure(0, weight=1)
        ctk.CTkLabel(events_frame, text="⚡ Execution Events", font=ctk.CTkFont(size=16, weight="bold")).grid(row=0, column=0, pady=5)
        self.events_text = ctk.CTkTextbox(events_frame, font=ctk.CTkFont(family="Consolas", size=14), state="disabled", wrap="word")
        self.events_text.grid(row=1, column=0, padx=10, pady=(0, 10), sticky="nsew")

        queues_frame = ctk.CTkFrame(content_frame)
        queues_frame.grid(row=0, column=1, padx=10, sticky="nsew")
        queues_frame.grid_rowconfigure(1, weight=1)
        queues_frame.grid_columnconfigure(0, weight=1)
        ctk.CTkLabel(queues_frame, text="🔄 System Queues", font=ctk.CTkFont(size=16, weight="bold")).grid(row=0, column=0, pady=5)
        self.queues_text = ctk.CTkTextbox(queues_frame, font=ctk.CTkFont(family="Consolas", size=15, weight="bold"), state="disabled", text_color="#00d2d3", wrap="word")
        self.queues_text.grid(row=1, column=0, padx=10, pady=(0, 10), sticky="nsew")

        memory_frame = ctk.CTkFrame(content_frame)
        memory_frame.grid(row=0, column=2, padx=(10, 0), sticky="nsew")
        memory_frame.grid_rowconfigure(1, weight=1)
        memory_frame.grid_columnconfigure(0, weight=1)
        ctk.CTkLabel(memory_frame, text="💾 RAM (Memory Array)", font=ctk.CTkFont(size=16, weight="bold")).grid(row=0, column=0, pady=5)
        self.memory_text = ctk.CTkTextbox(memory_frame, font=ctk.CTkFont(family="Consolas", size=14), state="disabled", wrap="none")
        self.memory_text.grid(row=1, column=0, padx=10, pady=(0, 10), sticky="nsew")

    # --- Auto-Play Logic ---
    def toggle_autoplay(self):
        self.is_autoplay = not self.is_autoplay
        if self.is_autoplay:
            self.play_btn.configure(text="⏸ Pause", fg_color="#e74c3c", hover_color="#c0392b")
            self.auto_step()
        else:
            self.play_btn.configure(text="▶ Auto-Play", fg_color="#f39c12", hover_color="#e67e22")

    def auto_step(self):
        if not self.is_autoplay: return
        
        if self.current_cycle_index == len(self.cycles) - 1:
            if self.input_entry.cget("state") == "normal":
                self.toggle_autoplay()
                return
            elif hasattr(self, 'process') and self.process.poll() is not None:
                self.toggle_autoplay() 
                return
            else:
                self.after(500, self.auto_step) 
        else:
            self.next_cycle()
            self.after(1000, self.auto_step) 

    def start_simulation(self):
        if not os.path.exists(EXECUTABLE_NAME) and not os.path.exists(EXECUTABLE_NAME.replace('.exe', '')):
            self.title_lbl.configure(text="❌ ERROR: os_simulator executable not found!")
            return

        self.boot_btn.configure(state="disabled", text="BOOTING...")
        self.scheduler_dropdown.configure(state="disabled")
        self.boot_frame.configure(fg_color="#34495e")
        
        self.prev_btn.configure(state="normal")
        self.next_btn.configure(state="normal")
        self.play_btn.configure(state="normal")
        self.status_lbl.configure(text="🟢 RUNNING...")

        choice = self.scheduler_var.get()[0] 

        self.process = subprocess.Popen(
            [EXECUTABLE_NAME, choice],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1 
        )

        self.thread = threading.Thread(target=self.read_output_live, daemon=True)
        self.thread.start()

    def read_output_live(self):
        buffer = ""
        while True:
            char = self.process.stdout.read(1)
            if not char and self.process.poll() is not None: break 
            buffer += char
            if char == '\n':
                self.after(0, self.parse_line, buffer.strip())
                buffer = ""
            elif buffer.endswith("Please enter a value: "):
                self.after(0, self.parse_line, buffer.strip())
                buffer = ""
                self.after(0, self.request_input) 

        if buffer: self.after(0, self.parse_line, buffer.strip())
        self.after(0, self.finalize_simulation)

    def parse_line(self, line):
        if not line: return
        
        cycle_match = re.search(r"--- Clock Cycle: (\d+) ---", line)
        if cycle_match:
            c_num = cycle_match.group(1)
            if len(self.cycles) == 1 and "Init" in self.cycles[0]["cycle_num"]:
                self.cycles[0]["cycle_num"] = c_num
            else:
                self.cycles.append({"cycle_num": c_num, "events": [], "queues": [], "memory": []})
            
            self.in_memory_block = False
            self.refresh_ui()
            return

        current = self.cycles[-1]

        if line == "--- current Memory State ---": self.in_memory_block = True; return
        elif line == "----------------------------" and self.in_memory_block: self.in_memory_block = False; return

        if self.in_memory_block: current["memory"].append(line)
        elif "Queue" in line or "Waiting List" in line: current["queues"].append(line)
        elif not line.startswith("[End of Cycle"): current["events"].append(line)

        self.refresh_ui()

    def request_input(self):
        self.input_frame.configure(fg_color="#c0392b") 
        target_cycle = self.cycles[-1]['cycle_num']
        self.status_lbl.configure(text=f"🛑 INPUT REQUIRED (For Cycle {target_cycle}):")
        
        self.input_entry.configure(state="normal", placeholder_text="Type your input here...")
        self.input_btn.configure(state="normal")
        self.input_entry.focus()
        
        self.cycles[-1]["events"].append("\n▶ [OS PROMPT]: Please enter a value...")
        self.refresh_ui()

    def submit_input(self, event=None):
        if self.input_entry.cget("state") == "disabled": return
        val = self.input_var.get()
        self.input_var.set("")
        
        self.input_entry.configure(state="disabled", placeholder_text="Waiting for OS prompt...")
        self.input_btn.configure(state="disabled")
        self.input_frame.configure(fg_color="#1f538d") 
        self.status_lbl.configure(text="🟢 RUNNING...")
        
        self.cycles[-1]["events"].append(f"✅ [USER INPUT]: \"{val}\"\n")
        self.refresh_ui()
        self.process.stdin.write(val + "\n")
        self.process.stdin.flush()

    def finalize_simulation(self):
        self.status_lbl.configure(text="🏁 SIMULATION COMPLETE")
        self.input_frame.configure(fg_color="#27ae60") 
        self.refresh_ui()

    def refresh_ui(self):
        if self.current_cycle_index == len(self.cycles) - 1:
            self.display_cycle(self.current_cycle_index)
        else:
            self.update_title_only()

    def update_title_only(self):
        if not self.cycles: return
        cycle_data = self.cycles[self.current_cycle_index]
        total_str = str(len(self.cycles) - 1)
        
        is_live = (self.current_cycle_index == len(self.cycles) - 1) and (self.process.poll() is None)
        is_complete = self.process.poll() is not None
        
        if is_live:
            mode_tag = " 🔴 (LIVE MODE)"
        elif is_complete and self.current_cycle_index == len(self.cycles) - 1:
            mode_tag = " ✅ (COMPLETED)"
        else:
            mode_tag = " ⏪ (HISTORY MODE)"
            
        self.title_lbl.configure(text=f"Clock Cycle: {cycle_data['cycle_num']} / {total_str}{mode_tag}")

    def update_hud(self, events_text):
        running_match = re.search(r"Executing P(\d+):", events_text)
        if running_match:
            self.running_lbl.configure(text=f"⚙️ Running Process: P{running_match.group(1)}", text_color="#2ecc71")
        elif "finished execution" in events_text and not running_match:
            self.running_lbl.configure(text="⚙️ Running Process: NONE", text_color="white")

        if "swapped out to disk" in events_text or "swapped in from disk" in events_text:
            self.swap_lbl.configure(text="💽 Disk Activity: SWAPPING!", text_color="#e74c3c")
        else:
            self.swap_lbl.configure(text="💽 Disk Activity: IDLE", text_color="#f1c40f")

    def display_cycle(self, index):
        if not self.cycles or index < 0 or index >= len(self.cycles): return
        cycle_data = self.cycles[index]
        
        self.update_title_only()
        
        events_str = "\n".join(cycle_data["events"])
        self.update_hud(events_str)
        
        self.update_textbox(self.events_text, events_str)
        self.update_textbox(self.queues_text, "\n\n".join(cycle_data["queues"]))
        self.update_textbox(self.memory_text, "\n".join(cycle_data["memory"]))

    def update_textbox(self, textbox, text):
        textbox.configure(state="normal")
        textbox.delete("1.0", "end")
        textbox.insert("end", text)
        textbox.see("end")
        textbox.configure(state="disabled")

    def next_cycle(self):
        if self.current_cycle_index < len(self.cycles) - 1:
            self.current_cycle_index += 1
            self.display_cycle(self.current_cycle_index)

    def prev_cycle(self):
        if self.current_cycle_index > 0:
            self.current_cycle_index -= 1
            self.display_cycle(self.current_cycle_index)

if __name__ == "__main__":
    app = ModernOSDashboard()
    app.mainloop()