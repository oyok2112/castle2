import tkinter as tk
from tkinter import ttk, scrolledtext

class RoomEditor:
    def __init__(self, root):
        self.root = root
        root.title("Castle Adventure II - Room Editor")
        
        # Position
        pos_frame = ttk.LabelFrame(root, text="Position", padding=10)
        pos_frame.grid(row=0, column=0, padx=10, pady=5, sticky="ew")
        
        ttk.Label(pos_frame, text="Y (row):").grid(row=0, column=0)
        self.y_entry = ttk.Entry(pos_frame, width=10)
        self.y_entry.grid(row=0, column=1, padx=5)
        
        ttk.Label(pos_frame, text="X (col):").grid(row=0, column=2)
        self.x_entry = ttk.Entry(pos_frame, width=10)
        self.x_entry.grid(row=0, column=3, padx=5)
        
        # Basic Info
        info_frame = ttk.LabelFrame(root, text="Basic Info", padding=10)
        info_frame.grid(row=1, column=0, padx=10, pady=5, sticky="ew")
        
        ttk.Label(info_frame, text="Name:").grid(row=0, column=0, sticky="w")
        self.name_entry = ttk.Entry(info_frame, width=50)
        self.name_entry.grid(row=0, column=1, pady=2)
        
        ttk.Label(info_frame, text="Description:").grid(row=1, column=0, sticky="nw")
        self.desc_text = scrolledtext.ScrolledText(info_frame, width=50, height=4)
        self.desc_text.grid(row=1, column=1, pady=2)
        
        ttk.Label(info_frame, text="Alt Text:").grid(row=2, column=0, sticky="nw")
        self.alt_text = scrolledtext.ScrolledText(info_frame, width=50, height=4)
        self.alt_text.grid(row=2, column=1, pady=2)
        
        ttk.Label(info_frame, text="Visit Event:").grid(row=3, column=0, sticky="w")
        self.visit_event = ttk.Entry(info_frame, width=50)
        self.visit_event.grid(row=3, column=1, pady=2)
        self.visit_event.insert(0, "none")
        
        # Exits
        exit_frame = ttk.LabelFrame(root, text="Exits", padding=10)
        exit_frame.grid(row=2, column=0, padx=10, pady=5, sticky="ew")
        
        self.exit_west = tk.BooleanVar()
        self.exit_south = tk.BooleanVar()
        self.exit_north = tk.BooleanVar()
        self.exit_east = tk.BooleanVar()
        
        
        ttk.Checkbutton(exit_frame, text="North", variable=self.exit_north).grid(row=0, column=0, padx=5)
        ttk.Checkbutton(exit_frame, text="South", variable=self.exit_south).grid(row=0, column=1, padx=5)
        ttk.Checkbutton(exit_frame, text="East", variable=self.exit_east).grid(row=0, column=2, padx=5)
        ttk.Checkbutton(exit_frame, text="West", variable=self.exit_west).grid(row=0, column=3, padx=5)
        
        # Flags
        flags_frame = ttk.LabelFrame(root, text="Flags", padding=10)
        flags_frame.grid(row=3, column=0, padx=10, pady=5, sticky="ew")
        
        self.exists = tk.BooleanVar(value=True)
        self.hidden = tk.BooleanVar()
        
        ttk.Checkbutton(flags_frame, text="Exists", variable=self.exists).grid(row=0, column=0, padx=5)
        ttk.Checkbutton(flags_frame, text="Hidden", variable=self.hidden).grid(row=0, column=1, padx=5)
        
        # Items
        items_frame = ttk.LabelFrame(root, text="Items (comma-separated)", padding=10)
        items_frame.grid(row=4, column=0, padx=10, pady=5, sticky="ew")
        
        self.items_entry = ttk.Entry(items_frame, width=50)
        self.items_entry.grid(row=0, column=0, pady=2)
        
        # Generate Button
        btn_frame = ttk.Frame(root)
        btn_frame.grid(row=5, column=0, padx=10, pady=10)
        
        ttk.Button(btn_frame, text="Generate C++ Code", command=self.generate_code).pack(side="left", padx=5)
        ttk.Button(btn_frame, text="Clear", command=self.clear_form).pack(side="left", padx=5)
        
        # Output
        output_frame = ttk.LabelFrame(root, text="Generated C++ Code", padding=10)
        output_frame.grid(row=6, column=0, padx=10, pady=5, sticky="ew")
        
        self.output_text = scrolledtext.ScrolledText(output_frame, width=70, height=15)
        self.output_text.grid(row=0, column=0)
        
    def generate_code(self):
        y = self.y_entry.get()
        x = self.x_entry.get()
        name = self.name_entry.get()
        desc = self.desc_text.get("1.0", tk.END).strip().replace('"', '\\"').replace('\n', ' ')
        alt = self.alt_text.get("1.0", tk.END).strip().replace('"', '\\"').replace('\n', ' ')
        visit_event = self.visit_event.get()
        items = [item.strip() for item in self.items_entry.get().split(',') if item.strip()]
        
        code = f"rooms[{y}][{x}].exists = {'true' if self.exists.get() else 'false'};\n"
        code += f'rooms[{y}][{x}].name = "{name}";\n'
        code += f'rooms[{y}][{x}].description = "{desc}";\n'
        
        if alt:
            code += f'rooms[{y}][{x}].altText = "{alt}";\n'
        
        if visit_event != "none":
            code += f'rooms[{y}][{x}].visitEvent = "{visit_event}";\n'
        
        if self.exit_north.get():
            code += f'rooms[{y}][{x}].exitNorth = true;\n'
        if self.exit_south.get():
            code += f'rooms[{y}][{x}].exitSouth = true;\n'
        if self.exit_east.get():
            code += f'rooms[{y}][{x}].exitEast = true;\n'
        if self.exit_west.get():
            code += f'rooms[{y}][{x}].exitWest = true;\n'
        
        if self.hidden.get():
            code += f'rooms[{y}][{x}].hidden = true;\n'
        
        for item in items:
            code += f'rooms[{y}][{x}].items.push_back("{item}");\n'
        
        code += "\n"
        
        self.output_text.insert(tk.END, code)
        self.output_text.see(tk.END)
        
    def clear_form(self):
        self.y_entry.delete(0, tk.END)
        self.x_entry.delete(0, tk.END)
        self.name_entry.delete(0, tk.END)
        self.desc_text.delete("1.0", tk.END)
        self.alt_text.delete("1.0", tk.END)
        self.visit_event.delete(0, tk.END)
        self.visit_event.insert(0, "none")
        self.items_entry.delete(0, tk.END)
        self.exit_north.set(False)
        self.exit_south.set(False)
        self.exit_east.set(False)
        self.exit_west.set(False)
        self.exists.set(True)
        self.hidden.set(False)

if __name__ == "__main__":
    root = tk.Tk()
    app = RoomEditor(root)
    root.mainloop()