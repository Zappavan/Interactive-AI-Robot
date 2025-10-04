from ollama import chat
import socket
from faster_whisper import WhisperModel
import sounddevice as sd
import numpy as np
import queue
from pynput import keyboard as kb
import time

# Initialize model once
model = WhisperModel("base", device="cpu")  ## tiny/base/small/medium/large


# --- ESP32 connection ---
ESP32_IP = "192.168.1.191"  # replace with your ESP32 local IP
ESP32_PORT = 12345

# ----- key functionality for push to talk----
SAMPLE_RATE = 16000

from pynput import keyboard as kb

key_pressed = False

def on_press(key):
    global key_pressed
    try:
        if key.char == 't':  # key to hold
            key_pressed = True
    except AttributeError:
        pass

def on_release(key):
    global key_pressed
    try:
        if key.char == 't':
            key_pressed = False
    except AttributeError:
        pass

listener = kb.Listener(on_press=on_press, on_release=on_release)
listener.start()


#---- PUSH TO TALK ----
def record_push_to_talk(sample_rate=16000):
    global key_pressed
    print("Hold 'T' to talk...")
    recording = []

    with sd.InputStream(samplerate=sample_rate, channels=1) as stream:
        while True:
            if key_pressed:
                audio_chunk, _ = stream.read(1024)
                recording.append(audio_chunk)
            else:
                if recording:  # if we already started recording and key released
                    break

    audio = np.concatenate(recording, axis=0).flatten()
    print("Recording stopped.")
    return audio



# --- transcription-----
def transcribe_audio(audio, sample_rate=16000):
    segments, info = model.transcribe(audio, beam_size=5)
    text = ""
    for segment in segments:
        text += segment.text + " "
    return text.strip()


def send_to_esp32(text):
    """Send text to the ESP32 via TCP."""
    if not text:
        return
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((ESP32_IP, ESP32_PORT))
            s.sendall((text + "\n").encode('utf-8'))
    except Exception as e:
        print("ESP32 send error:", e)

# --- Instructions to the LLM ---
instructions = """You are a robot assistant. You MUST respond EXACTLY like this:

CMD: <mood>
DISPLAY: <text>

Rules:
- Valid moods: <neutral>, <happy>, <excited>, <sad>, <angry>.
- DISPLAY text must be short (1–2 sentences) and clear.
- Do NOT include any extra text, explanations, or greetings.
- Do NOT write anything outside CMD and DISPLAY.
"""

# --- Conversation buffer ---
conversation = [
    {"role": "system", "content": instructions}
]

# --- Parsing helper ---
def parse_llm_reply(reply):
    mood = None
    display_text = None

    for line in reply.splitlines():
        line = line.strip()
        if line.upper().startswith("CMD:"):
            mood_raw = line[len("CMD:"):].strip()
            mood = mood_raw.replace("<", "").replace(">", "").lower()
            if mood not in ["neutral", "happy", "excited", "sad", "angry"]:
                mood = None
        elif line.upper().startswith("DISPLAY:"):
            display_raw = line[len("DISPLAY:"):].strip()
            display_text = display_raw.replace("<", "").replace(">", "")

    return mood, display_text

# --- Send to LLM and ESP32 ---
def send_to_llm(user_input):
    global conversation
    conversation.append({"role": "user", "content": user_input})
    
    response = chat(model="gemma3:1b", messages=conversation)
    reply = response['message']['content']
    conversation.append({"role": "assistant", "content": reply})

    # Debug: print raw output
    print("\n=== RAW LLM OUTPUT ===")
    print(reply)
    print("=====================\n")

    mood, display = parse_llm_reply(reply)

    # Defaults if parsing fails
    if mood is None:
        mood = "neutral"
    if display is None:
        display = ""

    # Send to ESP32
    if display:
        send_to_esp32(f"{mood}|{display}")

    return mood, display

# --- Main loop ---
print("Push to talk mode: hold T to talk, Ctrl+C to quit, or say 'exit' or 'quit'")
while True:
    try:
        audio = record_push_to_talk(SAMPLE_RATE) #push to talk
        user_text = transcribe_audio(audio)
        print("You (transcribed):", user_text)

        if user_text.lower() in ["exit", "quit"]:
            break
        mood, display = send_to_llm(user_text)
        print("Mood:", mood, "| Response:", display)
    except KeyboardInterrupt:
        print("\nExiting.")
        break
    except Exception as e:
        print("Error:", e)
