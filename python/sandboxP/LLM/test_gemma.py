from ollama import chat

# Define your prompt
prompt = "User said: 'Hey how are you?'. Context: you are talkative. Respond with concise COMMAND or DISPLAY text."

# Send the prompt to the model
response = chat(model="gemma3:1b", messages=[{"role": "user", "content": prompt}])

# Print the response
print("LLM response:")
print(response['message']['content'])
