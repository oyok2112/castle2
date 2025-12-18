# SAURON
# Self-Actualizing Utility regarding Overworld Nonsense

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox

class MapEditor:
    def __init__(self, root):
        self.root = root
        root.title("SAURON - Castle Adventure II - Map Editor")
        
        # Map dimensions
        self.MAP_WIDTH = 10
        self.MAP_HEIGHT = 10
        self.CELL_SIZE = 60
        
        # Room data storage
        self.rooms = {}  # Key: (y, x), Value: room dict
        self.selected_cell = None
        
        # Main layout
        main_frame = ttk.Frame(root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Left side - Grid
        left_frame = ttk.Frame(main_frame)
        left_frame.pack(side=tk.LEFT, padx=(0, 10))
        
        ttk.Label(left_frame, text="Map Grid (Click to select, Right-click to edit)").pack()
        
        # Canvas for grid
        self.canvas = tk.Canvas(
            left_frame, 
            width=self.MAP_WIDTH * self.CELL_SIZE, 
            height=self.MAP_HEIGHT * self.CELL_SIZE,
            bg='#2b2b2b'
        )
        self.canvas.pack()
        
        # Draw initial grid
        self.draw_grid()
        
        # Bind clicks
        self.canvas.bind('<Button-1>', self.on_left_click)
        self.canvas.bind('<Button-3>', self.on_right_click)
        
        # Right side - Room editor
        right_frame = ttk.Frame(main_frame)
        right_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        ttk.Label(right_frame, text="Room Editor", font=('Arial', 12, 'bold')).pack()
        
        self.selected_label = ttk.Label(right_frame, text="No cell selected")
        self.selected_label.pack(pady=5)
        
        # Room details
        details_frame = ttk.LabelFrame(right_frame, text="Room Details", padding=10)
        details_frame.pack(fill=tk.BOTH, expand=True, pady=5)
        
        # Name
        ttk.Label(details_frame, text="Name:").grid(row=0, column=0, sticky="w", pady=2)
        self.name_entry = ttk.Entry(details_frame, width=30)
        self.name_entry.grid(row=0, column=1, pady=2)
        
        # Description
        ttk.Label(details_frame, text="Description:").grid(row=1, column=0, sticky="nw", pady=2)
        self.desc_text = scrolledtext.ScrolledText(details_frame, width=30, height=4)
        self.desc_text.grid(row=1, column=1, pady=2)
        
        # Exits
        exits_frame = ttk.Frame(details_frame)
        exits_frame.grid(row=2, column=0, columnspan=2, pady=5)
        
        self.exit_north = tk.BooleanVar()
        self.exit_south = tk.BooleanVar()
        self.exit_east = tk.BooleanVar()
        self.exit_west = tk.BooleanVar()
        
        # Exit buttons in cross pattern
        ttk.Checkbutton(exits_frame, text="↑", variable=self.exit_north, width=3).grid(row=0, column=1)
        ttk.Checkbutton(exits_frame, text="←", variable=self.exit_west, width=3).grid(row=1, column=0)
        ttk.Label(exits_frame, text="Exits").grid(row=1, column=1)
        ttk.Checkbutton(exits_frame, text="→", variable=self.exit_east, width=3).grid(row=1, column=2)
        ttk.Checkbutton(exits_frame, text="↓", variable=self.exit_south, width=3).grid(row=2, column=1)
        
        # Items
        ttk.Label(details_frame, text="Items (comma-sep):").grid(row=3, column=0, sticky="w", pady=2)
        self.items_entry = ttk.Entry(details_frame, width=30)
        self.items_entry.grid(row=3, column=1, pady=2)
        
        # Flags
        flags_frame = ttk.Frame(details_frame)
        flags_frame.grid(row=4, column=0, columnspan=2, pady=5)
        
        self.exists = tk.BooleanVar(value=True)
        self.hidden = tk.BooleanVar()
        
        ttk.Checkbutton(flags_frame, text="Exists", variable=self.exists).pack(side=tk.LEFT, padx=5)
        ttk.Checkbutton(flags_frame, text="Hidden", variable=self.hidden).pack(side=tk.LEFT, padx=5)
        
        # Buttons
        btn_frame = ttk.Frame(details_frame)
        btn_frame.grid(row=5, column=0, columnspan=2, pady=10)
        
        ttk.Button(btn_frame, text="Save Room", command=self.save_room).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Delete Room", command=self.delete_room).pack(side=tk.LEFT, padx=2)
        ttk.Button(btn_frame, text="Clear Form", command=self.clear_form).pack(side=tk.LEFT, padx=2)
        
        # Bottom - Export
        export_frame = ttk.Frame(right_frame)
        export_frame.pack(fill=tk.BOTH, expand=True, pady=5)
        
        ttk.Button(export_frame, text="Generate C++ Code", command=self.generate_code).pack()
        
        self.output_text = scrolledtext.ScrolledText(export_frame, width=40, height=10)
        self.output_text.pack(fill=tk.BOTH, expand=True, pady=5)
        
    def draw_grid(self):
        self.canvas.delete("all")
        
        for y in range(self.MAP_HEIGHT):
            for x in range(self.MAP_WIDTH):
                x1 = x * self.CELL_SIZE
                y1 = y * self.CELL_SIZE
                x2 = x1 + self.CELL_SIZE
                y2 = y1 + self.CELL_SIZE
                
                # Determine cell color
                if (y, x) in self.rooms and self.rooms[(y, x)].get('exists', False):
                    color = '#4a4a4a' if not self.rooms[(y, x)].get('hidden', False) else '#6a4a6a'
                else:
                    color = '#1a1a1a'
                
                # Highlight selected
                if self.selected_cell == (y, x):
                    color = '#ffaa00'
                
                # Draw cell
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline='#555')
                
                # Draw room name if exists
                if (y, x) in self.rooms and self.rooms[(y, x)].get('exists', False):
                    name = self.rooms[(y, x)].get('name', '')
                    if name:
                        self.canvas.create_text(
                            x1 + self.CELL_SIZE/2, 
                            y1 + self.CELL_SIZE/2,
                            text=name[:8],  # Truncate for display
                            fill='white',
                            font=('Arial', 8)
                        )
                
                # Draw exits as lines
                if (y, x) in self.rooms:
                    room = self.rooms[(y, x)]
                    cx = x1 + self.CELL_SIZE/2
                    cy = y1 + self.CELL_SIZE/2
                    
                    if room.get('exitNorth', False):
                        self.canvas.create_line(cx, y1, cx, y1 + 10, fill='lime', width=2)
                    if room.get('exitSouth', False):
                        self.canvas.create_line(cx, y2, cx, y2 - 10, fill='lime', width=2)
                    if room.get('exitEast', False):
                        self.canvas.create_line(x2, cy, x2 - 10, cy, fill='lime', width=2)
                    if room.get('exitWest', False):
                        self.canvas.create_line(x1, cy, x1 + 10, cy, fill='lime', width=2)
                
                # Draw coordinates
                self.canvas.create_text(
                    x1 + 5, y1 + 5,
                    text=f"{y},{x}",
                    fill='#666',
                    font=('Arial', 7),
                    anchor='nw'
                )
    
    def get_cell_from_click(self, event):
        x = event.x // self.CELL_SIZE
        y = event.y // self.CELL_SIZE
        if 0 <= x < self.MAP_WIDTH and 0 <= y < self.MAP_HEIGHT:
            return (y, x)
        return None
    
    def on_left_click(self, event):
        cell = self.get_cell_from_click(event)
        if cell:
            self.selected_cell = cell
            self.load_room(cell)
            self.draw_grid()
    
    def on_right_click(self, event):
        cell = self.get_cell_from_click(event)
        if cell:
            self.selected_cell = cell
            self.load_room(cell)
            self.draw_grid()
            self.name_entry.focus()
    
    def load_room(self, cell):
        y, x = cell
        self.selected_label.config(text=f"Selected: [{y}][{x}]")
        
        if cell in self.rooms:
            room = self.rooms[cell]
            self.name_entry.delete(0, tk.END)
            self.name_entry.insert(0, room.get('name', ''))
            
            self.desc_text.delete("1.0", tk.END)
            self.desc_text.insert("1.0", room.get('description', ''))
            
            self.items_entry.delete(0, tk.END)
            self.items_entry.insert(0, ', '.join(room.get('items', [])))
            
            self.exit_north.set(room.get('exitNorth', False))
            self.exit_south.set(room.get('exitSouth', False))
            self.exit_east.set(room.get('exitEast', False))
            self.exit_west.set(room.get('exitWest', False))
            
            self.exists.set(room.get('exists', False))
            self.hidden.set(room.get('hidden', False))
        else:
            self.clear_form()
    
    def save_room(self):
        if not self.selected_cell:
            messagebox.showwarning("No Selection", "Please select a cell first")
            return
        
        y, x = self.selected_cell
        
        room_data = {
            'name': self.name_entry.get(),
            'description': self.desc_text.get("1.0", tk.END).strip(),
            'items': [item.strip() for item in self.items_entry.get().split(',') if item.strip()],
            'exitNorth': self.exit_north.get(),
            'exitSouth': self.exit_south.get(),
            'exitEast': self.exit_east.get(),
            'exitWest': self.exit_west.get(),
            'exists': self.exists.get(),
            'hidden': self.hidden.get()
        }
        
        self.rooms[(y, x)] = room_data
        self.draw_grid()
        messagebox.showinfo("Saved", f"Room [{y}][{x}] saved!")
    
    def delete_room(self):
        if not self.selected_cell:
            return
        
        if self.selected_cell in self.rooms:
            del self.rooms[self.selected_cell]
            self.clear_form()
            self.draw_grid()
            messagebox.showinfo("Deleted", "Room deleted")
    
    def clear_form(self):
        self.name_entry.delete(0, tk.END)
        self.desc_text.delete("1.0", tk.END)
        self.items_entry.delete(0, tk.END)
        self.exit_north.set(False)
        self.exit_south.set(False)
        self.exit_east.set(False)
        self.exit_west.set(False)
        self.exists.set(True)
        self.hidden.set(False)
    
    def generate_code(self):
        self.output_text.delete("1.0", tk.END)
        
        for (y, x), room in sorted(self.rooms.items()):
            if not room.get('exists', False):
                continue
            
            code = f"\nrooms[{y}][{x}].exists = true;\n"
            code += f'rooms[{y}][{x}].name = "{room["name"]}";\n'
            
            desc = room['description'].replace('"', '\\"').replace('\n', ' ')
            code += f'rooms[{y}][{x}].description = "{desc}";\n'
            
            if room.get('exitNorth'):
                code += f'rooms[{y}][{x}].exitNorth = true;\n'
            if room.get('exitSouth'):
                code += f'rooms[{y}][{x}].exitSouth = true;\n'
            if room.get('exitEast'):
                code += f'rooms[{y}][{x}].exitEast = true;\n'
            if room.get('exitWest'):
                code += f'rooms[{y}][{x}].exitWest = true;\n'
            
            if room.get('hidden'):
                code += f'rooms[{y}][{x}].hidden = true;\n'
            
            for item in room.get('items', []):
                code += f'rooms[{y}][{x}].items.push_back("{item}");\n'
            
            self.output_text.insert(tk.END, code)

if __name__ == "__main__":
    root = tk.Tk()
    app = MapEditor(root)
    root.mainloop()