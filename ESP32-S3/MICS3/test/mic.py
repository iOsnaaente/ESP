import numpy as np  # Biblioteca para manipulação de arrays numéricos
import serial
import pyaudio

# Configurações do Serial
SERIAL_PORT = '/dev/ttyACM0'  # Substitua pela porta correta do seu ESP32
BAUD_RATE = 1000000    # Taxa de baud usada no ESP32

# Configurações de áudio
SAMPLE_RATE  = 44100   # Taxa de amostragem (deve ser igual à usada no ESP32)
CHANNELS     = 1       # Canal mono
SAMPLE_WIDTH = 2       # 2 bytes por amostra (16 bits)

# Fator de amplificação
AMPLIFICATION_FACTOR = 10.0  # Ajuste conforme necessário (e.g., 1.5, 2.0, etc.)

# Inicializa a reprodução de áudio
audio = pyaudio.PyAudio()
stream = audio.open(
    format = audio.get_format_from_width(SAMPLE_WIDTH),
    channels = CHANNELS,
    rate = SAMPLE_RATE,
    output = True
)

# Inicializa a porta serial
ser = serial.Serial(SERIAL_PORT, BAUD_RATE)

print("Capturando dados do microfone e reproduzindo... Pressione Ctrl+C para parar.")

try:
    while True:
        # Lê dados da porta serial
        data = ser.read(512 * 2)  # Lê 1024 bytes (ajuste conforme necessário)
        
        # Converte os dados binários para um array NumPy (int16)
        audio_data = np.frombuffer(data, dtype=np.int16)
        
        # Amplifica os dados (mantendo os limites de 16 bits)
        amplified_data = np.clip(audio_data * AMPLIFICATION_FACTOR, -32768, 32767).astype(np.int16)
        
        # Envia os dados amplificados para reprodução
        stream.write(amplified_data.tobytes(), 512)

except KeyboardInterrupt:
    print("\nEncerrando...")

finally:
    # Fecha os recursos
    stream.stop_stream()
    stream.close()
    audio.terminate()
    ser.close()
