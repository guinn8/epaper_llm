# E-ink and LLM project

![Description](20240710_122145.gif)

Gif of simple and cheap hardware that can prompt a local llama.cpp server running on my 3090 and serving Meta-Llama-3-70B-Instruct-IQ2_XS.gguf.

## Some setup
```shell
sudo apt install stlink-tools
make && bash scripts/debug.sh
screen /dev/ttyACM0 115200
```