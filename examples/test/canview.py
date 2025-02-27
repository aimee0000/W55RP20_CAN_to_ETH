import socket
import threading
import tkinter as tk
from tkinter import scrolledtext, ttk

def get_local_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip_address = s.getsockname()[0]
        s.close()
        return ip_address
    except Exception:
        return "127.0.0.1"  
    
class CANVIEW:
    def __init__(self, master):
        self.master = master
        self.master.title("CANVIEW")

        # Network Settings
        settings_frame = tk.LabelFrame(master, text="Settings")
        settings_frame.pack(pady=5, fill="x")

        # Mode, IP, and Port on the same row
        tk.Label(settings_frame, text="Mode:").grid(row=0, column=0, padx=5, pady=5, sticky="e")
        self.mode_var = tk.StringVar(value="Server")
        self.mode_select = ttk.Combobox(settings_frame, textvariable=self.mode_var, values=["Server", "Client"], state="readonly", width=10)
        self.mode_select.grid(row=0, column=1, padx=5, pady=5)
        self.mode_select.bind("<<ComboboxSelected>>", self.update_mode)

        tk.Label(settings_frame, text="IP Address:").grid(row=0, column=2, padx=5, pady=5, sticky="e")
        self.ip_entry = tk.Entry(settings_frame, width=15)
        #self.ip_entry.insert(0, "192.168.11.2")
        self.ip_entry.grid(row=0, column=3, padx=5, pady=5)

        tk.Label(settings_frame, text="Port:").grid(row=0, column=4, padx=5, pady=5, sticky="e")
        self.port_entry = tk.Entry(settings_frame, width=6)
        self.port_entry.insert(0, "4001")
        self.port_entry.grid(row=0, column=5, padx=5, pady=5)

        # Unified Connect/Disconnect Button moved to the right
        self.connect_button = tk.Button(settings_frame, text="Listen", command=self.toggle_connection)
        self.connect_button.grid(row=0, column=9, padx=5, pady=5, sticky="e")
        settings_frame.grid_columnconfigure(7, weight=1)

        # CAN Settings
        can_frame = tk.LabelFrame(master, text="CAN")
        can_frame.pack(pady=5, fill="x")

        # here
        tk.Label(can_frame, text="Format:").grid(row=0, column=0, padx=5, pady=5, sticky="e")
        self.format_var = tk.StringVar(value="STD Data")
        self.format_select = ttk.Combobox(can_frame, textvariable=self.format_var, values=["STD Data", "EXT Data"], state="readonly", width=10)
        self.format_select.grid(row=0, column=1, padx=5, pady=5)
        self.format_select.bind("<<ComboboxSelected>>", self.update_format)

        tk.Label(can_frame, text="ID(Hex):").grid(row=0, column=2, padx=5, pady=5, sticky="e")
        self.can_id_entry = tk.Entry(can_frame, width=10, justify='center')  # Center-aligned
        self.can_id_entry.insert(0, "123")
        self.can_id_entry.grid(row=0, column=3, padx=5, pady=5)
        self.can_id_entry.bind("<KeyRelease>", self.validate_std_can_id)

        tk.Label(can_frame, text="DLC:").grid(row=0, column=4, padx=5, pady=5, sticky="e")
        self.dlc_var = tk.StringVar(value="1")
        self.dlc_select = ttk.Combobox(can_frame, textvariable=self.dlc_var, values=[str(i) for i in range(1, 9)], state="readonly", width=3)
        self.dlc_select.grid(row=0, column=5, padx=5, pady=5)

        tk.Label(can_frame, text="Data:").grid(row=1, column=0, padx=5, pady=5, sticky="w")

        self.data_entries = []
        for i in range(8):
            entry = tk.Entry(can_frame, width=10, justify='center')  # Center-aligned
            entry.insert(0, "00")  # Default value
            entry.grid(row=1, column=1 + i, padx=5, pady=5)
            self.data_entries.append(entry)

        self.can_send_button = tk.Button(can_frame, text="Send", command=self.can_send, state="disabled")  # Disabled by default
        self.can_send_button.grid(row=1, column=9, padx=5, pady=5, sticky="e")

        # Display Area with Labels for ID, DLC, Data
        display_frame = tk.Frame(master)
        display_frame.pack(pady=5)

        tk.Label(display_frame, text="ID", font=("Arial", 10, "bold")).place(relx=0.0, rely=0.02, anchor="w")
        tk.Label(display_frame, text="DLC", font=("Arial", 10, "bold")).place(relx=0.2, rely=0.02, anchor="w")
        tk.Label(display_frame, text="Data", font=("Arial", 10, "bold")).place(relx=0.5, rely=0.02, anchor="w")

        self.display = scrolledtext.ScrolledText(display_frame, width=60, height=20)
        self.display.pack(pady=20, fill="both", expand=True)

        self.create_context_menu()

        # Unified Status Bar
        self.status_var = tk.StringVar()
        self.status_bar = tk.Label(master, textvariable=self.status_var, anchor="w")
        self.status_bar.pack(fill="x")

        self.server_running = False
        self.server_socket = None
        self.client_socket = None
        self.client_threads = []
        self.client_sockets = []
        self.connected = False

        self.update_mode()

    def update_mode(self, event=None):
        mode = self.mode_var.get()
        if mode == "Server":
            self.connect_button.config(text="Listen")
            self.ip_entry.config(state="normal")
            self.ip_entry.delete(0, tk.END)
            self.ip_entry.insert(0, get_local_ip()) 
            self.ip_entry.config(state="disabled", disabledbackground="#f0f0f0")
        else:
            self.connect_button.config(text="Connect")
            self.ip_entry.config(state="normal")  
            self.ip_entry.delete(0, tk.END)
            self.ip_entry.insert(0, "192.168.11.2")

    def update_status(self, message, is_error=False):
        if is_error:
            self.status_bar.config(fg="red")
        else:
            self.status_bar.config(fg="black")
        self.status_var.set(message)

    def toggle_connection(self):
        if self.server_running:
            self.stop_connection()
        else:
            self.open_connection()
        self.can_send_button.config(state="normal" if self.server_running else "disabled")

    def open_connection(self):
        mode = self.mode_var.get()
        ip = self.ip_entry.get()
        port = int(self.port_entry.get())

        if mode == "Server":
            self.start_server(ip, port)
        elif mode == "Client":
            self.start_client(ip, port)

        self.can_send_button.config(state="normal")  # Enable Send button after connection

    def stop_connection(self):
        mode = self.mode_var.get()
        if mode == "Server":
            self.stop_server()
        elif mode == "Client":
            self.disconnect_client()
        self.can_send_button.config(state="disabled")

    def start_server(self, ip, port):
        if not self.server_running:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.settimeout(1.0)
            try:
                self.server_socket.bind((ip, port))
                self.server_socket.listen(5)
                self.server_running = True
                self.connect_button.config(text="Close")
                self.update_status(f"Server started on {ip}:{port}")
                threading.Thread(target=self.accept_clients, daemon=True).start()
            except Exception as e:
                self.update_status(f"Failed to start server: {e}", is_error=True)

    def start_client(self, ip, port):
        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((ip, port))
            self.server_running = True
            self.connected = True
            self.connect_button.config(text="Disconnect")
            self.update_status(f"Connected to server at {ip}:{port}")
            threading.Thread(target=self.receive_data_from_server, daemon=True).start()
        except Exception as e:
            self.server_running = False
            self.update_status(f"Failed to connect to server: {e}", is_error=True)


    def stop_server(self):
        if self.server_running:
            self.server_running = False
            if self.server_socket:
                try:
                    self.server_socket.shutdown(socket.SHUT_RDWR)
                except:
                    pass
                self.server_socket.close()

            for client_socket in self.client_sockets:
                try:
                    client_socket.shutdown(socket.SHUT_RDWR)
                except:
                    pass
                client_socket.close()

            for thread in self.client_threads:
                thread.join(timeout=1.0)

            self.connect_button.config(text="Listen")
            self.update_status("Connection closed")
            self.can_send_button.config(state="disabled")


    def disconnect_client(self):
        if self.server_running and self.client_socket:
            try:
                self.client_socket.shutdown(socket.SHUT_RDWR)
            except:
                pass
            self.client_socket.close()
            self.server_running = False
            self.connect_button.config(text="Connect")
            if self.connected:
                self.update_status("Connection closed")
                self.connected = False
                self.can_send_button.config(state="disabled")

    def accept_clients(self):
        while self.server_running:
            try:
                client_socket, addr = self.server_socket.accept()
                client_socket.settimeout(1.0)
                self.client_sockets.append(client_socket)
                self.update_status(f"Client connected: {addr}")
                client_thread = threading.Thread(target=self.handle_client, args=(client_socket,), daemon=True)
                self.client_threads.append(client_thread)
                client_thread.start()
            except socket.timeout:
                continue
            except:
                break

    def handle_client(self, client_socket):
        with client_socket:
            while self.server_running:
                try:
                    data = client_socket.recv(1024)
                    if not data:
                        break
                    self.display_data(data.decode('utf-8'))
                except socket.timeout:
                    continue
                except:
                    break
        if client_socket in self.client_sockets:
            self.client_sockets.remove(client_socket)
        self.update_status("Client disconnected")

    def receive_data_from_server(self):
        buffer = ""
        while self.server_running:
            try:
                data = self.client_socket.recv(1024)

                if not data:
                    print("data failed")
                    break  

                buffer += data.decode('utf-8')

                while "\n" in buffer:
                    message, buffer = buffer.split("\n", 1)
                    self.display_data(message.strip())

            except (ConnectionResetError, OSError):
                break

        if self.connected:
            self.update_status("Connection closed")
            self.connected = False
            self.disconnect_client()
            self.can_send_button.config(state="disabled")

    # CAN field
    def update_format(self, event=None):
        format_type = self.format_var.get()

        if format_type == "STD Data":
            self.can_id_entry.delete(0, tk.END)
            self.can_id_entry.insert(0, "123")
            self.can_id_entry.bind("<KeyRelease>", self.validate_std_can_id)
        elif format_type == "EXT Data":
            self.can_id_entry.delete(0, tk.END)
            self.can_id_entry.insert(0, "1F112233")
            self.can_id_entry.bind("<KeyRelease>", self.validate_ext_can_id)

    def validate_std_can_id(self, event=None):
        try: 
            can_id = int(self.can_id_entry.get(), 16)
            if can_id > 0x7FF:
                self.can_id_entry.delete(0, tk.END)
                self.can_id_entry.insert(0, "7FF")
        except ValueError:
            self.can_id_entry.delete(0, tk.END)
            self.can_id_entry.insert(0, "0")

    def validate_ext_can_id(self, event=None):
        try:
            can_id = int(self.can_id_entry.get(), 16)
            if can_id > 0x1FFFFFFF:
                self.can_id_entry.delete(0, tk.END)
                self.can_id_entry.insert(0, "1FFFFFFF")
        except ValueError:
            self.can_id_entry.delete(0, tk.END)
            self.can_id_entry.insert(0, "0")

    def can_send(self):
        if not self.server_running:
            self.update_status("Not connected", is_error=True)
            return

        #can_id = self.can_id_entry.get()
        can_id = int(self.can_id_entry.get(), 16)
        
        if self.format_var.get() == "EXT Data":  
            can_id |= (1 << 31)  

        can_id_str = format(can_id, 'X')

        dlc = int(self.dlc_var.get())
        data_values = [self.data_entries[i].get() for i in range(dlc)]
        formatted_data = ' '.join(data_values) + "\x00"

        message = f"{can_id_str},{formatted_data}\n"

        try:
            if self.mode_var.get() == "Server":
                for client in self.client_sockets:
                    client.sendall(message.encode('utf-8'))
            elif self.mode_var.get() == "Client" and self.client_socket:
                self.client_socket.sendall(message.encode('utf-8'))
            self.update_status(f"Sent: {message.strip()}")
        except Exception as e:
            self.update_status(f"Failed to send: {e}", is_error=True)

    # Data field
    def display_data(self, message):
        if "," in message:
            try:
                id_part, rest = message.split(",", 1)
                dlc_part, data_part = rest.strip().split(" ", 1)
                dlc = int(dlc_part.strip("[]"))
                data_items = data_part.strip().split(" ")

                can_id = int(id_part, 16)

                if can_id & (1 << 31):  
                    can_id &= 0x7FFFFFFF 

                can_id_str = format(can_id, 'X')

            except ValueError:
                self.update_status("Data format error", is_error=True)
                return

            formatted_data = ' '.join(f"{item:<2}" for item in data_items)
            formatted_message = f"{can_id_str:<14}{dlc:<12}{formatted_data}\n"
            self.display.insert(tk.END, formatted_message)
            self.display.see(tk.END) 
            self.update_status("")  # Clear error message if data is valid
        else:
            self.update_status("Data format error", is_error=True) 

    def create_context_menu(self):
        self.context_menu = tk.Menu(self.master, tearoff=0)
        self.context_menu.add_command(label="Clear", command=self.clear_display)
        self.display.bind("<Button-3>", self.show_context_menu)

    def show_context_menu(self, event):
        self.context_menu.post(event.x_root, event.y_root)

    def clear_display(self):
        self.display.delete("1.0", tk.END)

if __name__ == "__main__":
    root = tk.Tk()
    app = CANVIEW(root)
    root.mainloop()
