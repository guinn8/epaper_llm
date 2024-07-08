sudo apt install stlink-tools
make && bash scripts/debug.sh
screen /dev/ttyACM0 115200