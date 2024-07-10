import socket

lorem = "Lorem ipsum dolor sit amet, consectetur adipiscing elit."

def send_data_to_server(data, host='10.0.0.66', port=333):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.settimeout(5)  # Set a timeout of 5 seconds
        client_socket.connect((host, port))
        client_socket.sendall(data.encode('utf-8'))
        
        try:
            response = client_socket.recv(1024)  # Adjust buffer size if necessary
            print("Received from server:", response.decode('utf-8'))
        except socket.timeout:
            print("No response from server within 5 seconds")

if __name__ == "__main__":
    user_message = input("Enter the message you want to send: ")
    data = lorem if user_message == "" else user_message
    send_data_to_server(data)
