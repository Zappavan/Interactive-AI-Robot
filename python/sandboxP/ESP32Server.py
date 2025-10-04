import socket

HOST = ''       # '' means listen on all available interfaces
PORT = 12345    # make sure this matches ESP32 code

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    print(f"Waiting for ESP32 on port {PORT}...")
    conn, addr = s.accept()
    with conn:
        print("Connected by", addr)
        while True:
            data = conn.recv(1024)
            if not data:
                break
            print("ESP32 says:", data.decode().strip())
            # Send a command back
            conn.sendall(b"LLM_REPLY: Hello from Mac!\n")
