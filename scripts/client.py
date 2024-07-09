import socket

def send_data_to_server(data, host='10.0.0.66', port=333):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((host, port))
        client_socket.sendall(data.encode('utf-8'))

if __name__ == "__main__":
    user_message = input("Enter the message you want to send: ")
    data = "ayy lmao" if user_message == "" else user_message
    send_data_to_server(data)
