from ollama import chat

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
    # Keep last turn detailed, summarize rest
    if len(conv) <= MAX_TURNS + 1:
        return conv  # nothing to summarize yet
    
    # Everything except system + last N turns
    older = conv[1:-MAX_TURNS]
    recent = conv[-MAX_TURNS:]
    
    # Create summary text
    summary_text = "Summarize the following for future context:\n"
    for msg in older:
        if msg['role'] == 'user':
            summary_text += f"User: {msg['content']}\n"
        elif msg['role'] == 'assistant':
            summary_text += f"Assistant: {msg['content']}\n"
    
    summary_msg = {"role": "system", "content": f"[SUMMARY]: {summary_text}"}
    
    # New conversation: instructions + summary + recent
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
    
    return display_text

# --- Test ---
print("Type your messages below. Type 'exit' to quit.")
while True:
    user_text = input("You: ")
    if user_text.lower() in ["exit", "quit"]:
        break
    display = send_to_llm(user_text)
    print("OLED Display:", display)
