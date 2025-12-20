import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, filedialog
import json


class ItemDesigner:
    def __init__(self, root):
        self.root = root
        root.title("Castle Adventure II - Item Designer")

        # Basic fields
        info_frame = ttk.LabelFrame(root, text="Basic Info", padding=10)
        info_frame.grid(row=0, column=0, padx=10, pady=5, sticky="ew")

        ttk.Label(info_frame, text="Key (id):").grid(row=0, column=0, sticky="w")
        self.key_entry = ttk.Entry(info_frame, width=40)
        self.key_entry.grid(row=0, column=1, pady=2)

        ttk.Label(info_frame, text="Name:").grid(row=1, column=0, sticky="w")
        self.name_entry = ttk.Entry(info_frame, width=50)
        self.name_entry.grid(row=1, column=1, pady=2)

        ttk.Label(info_frame, text="Description:").grid(row=2, column=0, sticky="nw")
        self.desc_text = scrolledtext.ScrolledText(info_frame, width=50, height=6)
        self.desc_text.grid(row=2, column=1, pady=2)

        # Numeric stats
        stats_frame = ttk.LabelFrame(root, text="Stats", padding=10)
        stats_frame.grid(row=1, column=0, padx=10, pady=5, sticky="ew")

        ttk.Label(stats_frame, text="Attack Damage:").grid(row=0, column=0, sticky="w")
        self.attack_entry = ttk.Entry(stats_frame, width=10)
        self.attack_entry.grid(row=0, column=1, sticky="w", padx=5)
        self.attack_entry.insert(0, "0")

        ttk.Label(stats_frame, text="Throw Damage:").grid(row=0, column=2, sticky="w")
        self.throw_entry = ttk.Entry(stats_frame, width=10)
        self.throw_entry.grid(row=0, column=3, sticky="w", padx=5)
        self.throw_entry.insert(0, "0")

        # Flags
        flags_frame = ttk.LabelFrame(root, text="Flags", padding=10)
        flags_frame.grid(row=2, column=0, padx=10, pady=5, sticky="ew")

        self.can_take = tk.BooleanVar(value=True)
        self.is_weapon = tk.BooleanVar(value=False)
        self.is_key = tk.BooleanVar(value=False)

        ttk.Checkbutton(flags_frame, text="Can Take", variable=self.can_take).grid(row=0, column=0, padx=5)
        ttk.Checkbutton(flags_frame, text="Is Weapon", variable=self.is_weapon).grid(row=0, column=1, padx=5)
        ttk.Checkbutton(flags_frame, text="Is Key", variable=self.is_key).grid(row=0, column=2, padx=5)

        # Buttons
        btn_frame = ttk.Frame(root)
        btn_frame.grid(row=3, column=0, padx=10, pady=8)

        ttk.Button(btn_frame, text="Generate C++ Code", command=self.generate_cpp).pack(side="left", padx=6)
        ttk.Button(btn_frame, text="Export JSON...", command=self.export_json).pack(side="left", padx=6)
        ttk.Button(btn_frame, text="Clear", command=self.clear_form).pack(side="left", padx=6)

        # Output
        output_frame = ttk.LabelFrame(root, text="Output / Preview", padding=10)
        output_frame.grid(row=4, column=0, padx=10, pady=5, sticky="ew")

        self.output_text = scrolledtext.ScrolledText(output_frame, width=72, height=12)
        self.output_text.grid(row=0, column=0)

    def safe_int(self, text):
        try:
            return int(text)
        except Exception:
            return 0

    def generate_cpp(self):
        key = self.key_entry.get().strip()
        name = self.name_entry.get().strip().replace('"', '\\"')
        desc = self.desc_text.get("1.0", tk.END).strip().replace('"', '\\"').replace('\n', ' ')
        attack = self.safe_int(self.attack_entry.get())
        throw = self.safe_int(self.throw_entry.get())
        can_take = 'true' if self.can_take.get() else 'false'
        is_weapon = 'true' if self.is_weapon.get() else 'false'
        is_key = 'true' if self.is_key.get() else 'false'

        if not key:
            messagebox.showwarning("Missing key", "Please enter an item key (id) to use in the database map.")
            return

        code = f'itemDatabase["{key}"] = {{"{name}", "{desc}", {attack}, {throw}, {can_take}, {is_weapon}, {is_key}}};\n'

        # Also show a prettier human-readable preview JSON style
        preview = {
            "key": key,
            "name": name,
            "description": desc,
            "attackDmg": attack,
            "throwDmg": throw,
            "canTake": self.can_take.get(),
            "isWeapon": self.is_weapon.get(),
            "isKey": self.is_key.get()
        }

        self.output_text.delete("1.0", tk.END)
        self.output_text.insert(tk.END, "// C++ initializer:\n")
        self.output_text.insert(tk.END, code + "\n")
        self.output_text.insert(tk.END, "// JSON preview:\n")
        self.output_text.insert(tk.END, json.dumps(preview, indent=2))

    def export_json(self):
        key = self.key_entry.get().strip()
        if not key:
            messagebox.showwarning("Missing key", "Please enter an item key (id) before exporting JSON.")
            return

        data = {
            "key": key,
            "name": self.name_entry.get().strip(),
            "description": self.desc_text.get("1.0", tk.END).strip(),
            "attackDmg": self.safe_int(self.attack_entry.get()),
            "throwDmg": self.safe_int(self.throw_entry.get()),
            "canTake": self.can_take.get(),
            "isWeapon": self.is_weapon.get(),
            "isKey": self.is_key.get()
        }

        path = filedialog.asksaveasfilename(defaultextension='.json', filetypes=[('JSON', '*.json')], title='Export Item as JSON')
        if path:
            try:
                with open(path, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=2)
                messagebox.showinfo("Exported", f"Item exported to {path}")
            except Exception as e:
                messagebox.showerror("Error", f"Failed to save file: {e}")

    def clear_form(self):
        self.key_entry.delete(0, tk.END)
        self.name_entry.delete(0, tk.END)
        self.desc_text.delete("1.0", tk.END)
        self.attack_entry.delete(0, tk.END)
        self.attack_entry.insert(0, "0")
        self.throw_entry.delete(0, tk.END)
        self.throw_entry.insert(0, "0")
        self.can_take.set(True)
        self.is_weapon.set(False)
        self.is_key.set(False)
        self.output_text.delete("1.0", tk.END)


if __name__ == "__main__":
    root = tk.Tk()
    app = ItemDesigner(root)
    root.mainloop()
