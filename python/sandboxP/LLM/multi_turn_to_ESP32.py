from ollama import chat
import socket

# --- ESP32 connection ---
ESP32_IP = "192.168.1.191"  # replace with your ESP32 local IP
ESP32_PORT = 12345

def send_to_esp32(text):
    """Send the DISPLAY text to the ESP32 via TCP."""
    if not text:
        return
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((ESP32_IP, ESP32_PORT))
            s.sendall((text + "\n").encode('utf-8'))
    except Exception as e:
        print("ESP32 send error:", e)

# --- Instructions ---
instructions = """You are a robot assistant.
- Only respond with DISPLAY: <text>.
- Be concise.
- Wait until user finishes speaking before responding."""

# --- Conversation buffer ---
conversation = [
    {"role": "system", "content": instructions}
]

# --- Parameters ---
MAX_TURNS = 5  # summarize after this many exchanges
MIN_TOKENS_AFTER_SUMMARY = 200  # optional minimum context size

def summarize_conversation(conv):
    """Summarize older turns to reduce prompt size."""
    if len(conv) <= MAX_TURNS + 1:
        return conv  # nothing to summarize yet
    
    older = conv[1:-MAX_TURNS]
    recent = conv[-MAX_TURNS:]
    
    summary_text = "Summarize the following for future context:\n"
    for msg in older:
        if msg['role'] == 'user':
            summary_text += f"User: {msg['content']}\n"
        elif msg['role'] == 'assistant':
            summary_text += f"Assistant: {msg['content']}\n"
    
    summary_msg = {"role": "system", "content": f"[SUMMARY]: {summary_text}"}
    return [conv[0], summary_msg] + recent

def send_to_llm(user_input):
    global conversation
    # Add user message
    conversation.append({"role": "user", "content": user_input})
    
    # Summarize if needed
    conversation[:] = summarize_conversation(conversation)
    
    # Send to LLM
    response = chat(model="gemma3:1b", messages=conversation)
    reply = response['message']['content']
    
    # Add assistant message
    conversation.append({"role": "assistant", "content": reply})
    
    # Parse DISPLAY only
    display_text = None
    for line in reply.splitlines():
        line = line.strip()
        if line.upper().startswith("DISPLAY:"):
            display_text = line[len("DISPLAY:"):].strip()
            break
    
    # Send to ESP32
    send_to_esp32(display_text)
    
    return display_text

# --- Main loop ---
print("Type your messages below. Type 'exit' to quit.")
while True:
    user_text = input("You: ")
    if user_text.lower() in ["exit", "quit"]:
        break
    display = send_to_llm(user_text)
    print("OLED Display:", display)
