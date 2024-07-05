import socket

def start_server(host='0.0.0.0', port=8080):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((host, port))
        server_socket.listen(5)
        print(f"Server listening on {host}:{port}")

        while True:
            client_socket, client_address = server_socket.accept()
            with client_socket:
                print(f"Connection from {client_address}")
                while True:
                    data = client_socket.recv(1024)
                    if not data:
                        break
                    message = data.decode('utf-8')
                    print(f"Received data: {message}")
                    response = "pong"
                    print(f"Sending: {response.encode('utf-8')}")
                    client_socket.sendall(response.encode('utf-8'))

if __name__ == "__main__":
    start_server()
