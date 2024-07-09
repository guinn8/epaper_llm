import socket
import time

def send_data_to_server(data, host='10.0.0.66', port=333):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((host, port))
        client_socket.sendall(data.encode('utf-8'))

if __name__ == "__main__":
    data = "Hello ESP32!"
    send_data_to_server(data)

