import openai
import sys
import socket
import threading
import signal
import psutil
import argparse
from datetime import datetime
import json

# Function to get the host IP address
def get_host_ip(interface_name):
    addrs = psutil.net_if_addrs()
    if interface_name in addrs:
        for addr in addrs[interface_name]:
            if addr.family == socket.AF_INET:
                return addr.address
    return '0.0.0.0'

import socket
from datetime import datetime
import openai

def handle_client(client_socket, client_address):
    with client_socket:
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        print(f"[{timestamp}] Connection from {client_address}")
        
        buffer = ""
        
        while True:
            data = client_socket.recv(1024).decode('utf-8')
            if not data:
                break
            
            buffer += data
            
            # Check if the buffer contains the delimiter \n\r
            if "\n\r" in buffer:
                message, buffer = buffer.split("\n\r", 1)
                
                # Create OpenAI client and get response
                client = openai.OpenAI(
                    base_url="http://localhost:8080/v1",  # Change to your API server URL
                    api_key="sk-no-key-required"          # Update with your actual API key if necessary
                )
                
                completion = client.chat.completions.create(
                    model='gpt-3.5-turbo',
                    messages=[
                        {"role": "system", "content": "Be very concise responding in a single snappy one-liner.\n\r"},
                        {'role': 'user', 'content': message}
                    ],
                    temperature=1.5,
                    max_tokens=32,  # Limit the response to 64 tokens
                    stream=False
                )
                
                # Directly access the response content
                response_message = completion.choices[0].message.content
                print(response_message)
                
                timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                print(f"[{timestamp}] Received data: {message}")
                print(f"[{timestamp}] Sending data: {response_message}")
                client_socket.sendall(response_message.encode('utf-8'))



# Function to start the server
def start_server(interface_name='enp37s0', port=8080):
    host = get_host_ip(interface_name)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(5)
    print(f"Server listening on {host}:{port}")

    def signal_handler(sig, frame):
        print('Shutting down server...')
        server_socket.close()
        sys.exit(0)

    signal.signal(signal.SIGINT, signal_handler)

    while True:
        try:
            client_socket, client_address = server_socket.accept()
            client_handler = threading.Thread(target=handle_client, args=(client_socket, client_address))
            client_handler.start()
        except Exception as e:
            print(f"Error: {e}")
            break

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Start the server.')
    parser.add_argument('--port', type=int, default=8080, help='Port number to start the server on')
    args = parser.parse_args()

    start_server(port=args.port)
