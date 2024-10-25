import serial
import time
import tkinter as tk
from tkinter import messagebox, scrolledtext
import threading

# Configuración del puerto serial
try:
    puertoSerial = serial.Serial('COM4', 9600, timeout=1)
    time.sleep(2)  # Esperar a que se establezca la conexión serial
except Exception as e:
    messagebox.showerror("Error", f"No se pudo abrir el puerto COM6: {str(e)}")
    exit()

# Función para leer datos desde el Arduino y mostrarlos en la interfaz
def leer_serial():
    while True:
        if puertoSerial.in_waiting > 0:
            linea = puertoSerial.readline().decode('utf-8').rstrip()
            if linea:
                text_area.config(state='normal')  # Habilitar el área de texto
                text_area.insert(tk.END, linea + '\n')
                text_area.config(state='disabled')  # Deshabilitar el área de texto
                text_area.see(tk.END)
        time.sleep(0.1)
# Crear la ventana de la interfaz gráfica
root = tk.Tk()
root.title("Simulador de Pulsaciones")

# Marco para la salida de texto
text_area = scrolledtext.ScrolledText(root, width=50, height=20, state='disabled')
text_area.pack(pady=10)

# Etiqueta de instrucciones
label = tk.Label(root, text="Selecciona un botón para simular la pulsación")
label.pack(pady=10)

# Función para enviar comandos al Arduino
def enviar_comando(comando):
    puertoSerial.write((comando + '\n').encode('utf-8'))

# Botones para simular A0, A1, A2, A3, y A4
btn_A0 = tk.Button(root, text="Pulsar A0", command=lambda: enviar_comando('A0'))
btn_A0.pack(pady=5)

btn_A1 = tk.Button(root, text="Pulsar A1", command=lambda: enviar_comando('A1'))
btn_A1.pack(pady=5)

btn_A2 = tk.Button(root, text="Pulsar A2", command=lambda: enviar_comando('A2'))
btn_A2.pack(pady=5)

btn_A3 = tk.Button(root, text="Pulsar A3", command=lambda: enviar_comando('A3'))
btn_A3.pack(pady=5)

btn_A4 = tk.Button(root, text="Pulsar A4", command=lambda: enviar_comando('A4'))
btn_A4.pack(pady=5)

btn_A5 = tk.Button(root, text="Pulsar A5", command=lambda: enviar_comando('A5'))
btn_A5.pack(pady=5)

btn_A6 = tk.Button(root, text="Pulsar A6", command=lambda: enviar_comando('A6'))
btn_A6.pack(pady=5)

btn_A7 = tk.Button(root, text="Pulsar A7", command=lambda: enviar_comando('A7'))
btn_A7.pack(pady=5)

btn_A8 = tk.Button(root, text="Pulsar A8", command=lambda: enviar_comando('A8'))
btn_A8.pack(pady=5)

btn_A9 = tk.Button(root, text="Pulsar A9", command=lambda: enviar_comando('A9'))
btn_A9.pack(pady=5)


# Botón para salir
def cerrar():
    if puertoSerial.is_open:
        puertoSerial.close()
    root.quit()

btn_exit = tk.Button(root, text="Salir", command=cerrar)
btn_exit.pack(pady=20)

# Hilo para leer desde el Arduino
hilo_lectura = threading.Thread(target=leer_serial, daemon=True)
hilo_lectura.start()

# Ejecutar el bucle principal de la interfaz gráfica
root.mainloop()
