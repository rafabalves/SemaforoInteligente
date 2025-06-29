import serial
import time

# Abre a comunicação serial com o Arduino (verifique a porta!)
arduino = serial.Serial('COM5', 9600)  # No Linux/Mac: '/dev/ttyUSB0' ou '/dev/ttyACM0'
time.sleep(2)  # Aguarda o Arduino reiniciar após abrir a porta serial

# Exemplo: variável que será enviada
variavel = 123

# Envia o valor (como string) com caractere de nova linha
arduino.write(f"{variavel}\n".encode())

# Opcional: ler resposta do Arduino
resposta = arduino.readline().decode().strip()
print(f"Arduino respondeu: {resposta}")

arduino.close()