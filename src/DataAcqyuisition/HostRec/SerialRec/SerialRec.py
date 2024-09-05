import serial
import os
import time

label = 0   #动作标签
index = 0   #动作序列
DataPath = "../../../../Dataset/"

def process_data(ser, data):
    global index
    start = 0
    while True:
        filename = f"{label:02}_{index:02}.txt"
        filepath = os.path.join(DataPath, filename)
        count = 0
        with open(filepath, "w") as file:
            if start == 0:
                start = 1
                file.write(data + '\n')
                count += 1
                time.sleep(0.01)
            while True:
                if ser.in_waiting > 0:
                    # 读取串口数据

                    data = ser.readline().decode('utf-8').strip()
                    file.write(data + '\n')
                    time.sleep(0.01)  # 避免CPU占用过高
                    count += 1
                    if count == 600:
                        break
        index += 1
        print(index)

def read_serial_data(port, baudrate):
    # 打开串口
    global index
    ser = serial.Serial(port, baudrate, timeout=1)
    print(f"Connected to {port} at {baudrate} baudrate.")
    try:
        while True:
            if ser.in_waiting > 0:
                data = ser.readline().decode('utf-8').strip()
                if data.startswith("ACCELERO"):
                    process_data(ser, data)  # 调用处理数据的函数
                    break
    finally:
        ser.close()

if __name__ == "__main__":
    port = "COM3"  # 替换为你实际的串口号，例如 "COM3" 或 "/dev/ttyUSB0"
    baudrate = 115200  # 替换为你实际使用的波特率
    read_serial_data(port, baudrate)
