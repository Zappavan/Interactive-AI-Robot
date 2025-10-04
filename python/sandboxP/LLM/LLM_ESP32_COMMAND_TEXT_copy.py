from ollama import chat
import socket

# --- ESP32 connection ---
ESP32_IP = "192.168.1.191"  # replace with your ESP32 local IP
ESP32_PORT = 12345

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
- DISPLAY text must be short (1–2 sentences) and clear, so write <text you want to say>.
- Do NOT include any extra text, explanations, or greetings.
- Do NOT write anything outside CMD and DISPLAY.
"""


# --- Conversation buffer ---
conversation = [
    {"role": "system", "content": "You are starting a new session. Forget all previous interactions."}
]

def send_to_llm(user_input):
    global conversation
    # Add user message
    conversation.append({"role": "user", "content": user_input})
    
    # Send to LLM
    response = chat(model="gemma3:1b", messages=conversation)
    reply = response['message']['content']
    
    # Add assistant message
    conversation.append({"role": "assistant", "content": reply})
    
    # TEMP: print the raw LLM output
    print("\n=== RAW LLM OUTPUT ===")
    print(reply)
    print("=====================\n")
    
    # Old parsing code temporarily disabled
    # mood = None
    # display_text = None
    # for line in reply.splitlines():
    #     line = line.strip()
    #     if line.upper().startswith("CMD:"):
    #         mood = line[len("CMD:"):].strip().lower()
    #     elif line.upper().startswith("DISPLAY:"):
    #         display_text = line[len("DISPLAY:"):].strip()
    # if display_text:
    #     send_to_esp32(f"{mood}|{display_text}")
    
    return reply  # just return the raw string for now


# --- Main loop ---
print("Type your messages below. Type 'exit' to quit.")
while True:
    user_text = input("You: ")
    if user_text.lower() in ["exit", "quit"]:
        break
    mood, display = send_to_llm(user_text)
    print("Mood:", mood, "| Response:", display)
