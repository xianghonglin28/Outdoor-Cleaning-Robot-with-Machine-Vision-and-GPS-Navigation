import sensor, image, time, math
from pyb import UART, LED

uart = UART(3, 115200)   # 定义串口3变量
uart.init(115200, bits=8, parity=None, stop=1)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)  # 如果分辨率太高，内存可能会溢出…
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)        # 必须关闭此功能，以防止图像冲洗…
sensor.set_auto_whitebal(False)    # 必须关闭此功能，以防止图像冲洗…
clock = time.clock()

K_z = 56.5  # 比例尺
tag_families = image.TAG36H11  # 这里使用的是H11家族的tag码

# 将负数转换为补码形式
def to_twos_complement(value, bits=8):
    if value < 0:
        return (1 << bits) + value
    return value

while True:
    clock.tick()
    img = sensor.snapshot()

    distance = 0
    error_x = 0
    error_y = 0
    tag_found = False

    for tag in img.find_apriltags(families=tag_families):
        img.draw_rectangle(tag.rect(), color=(255, 0, 0))
        img.draw_cross(tag.cx(), tag.cy(), color=(0, 255, 0))

        # 计算真实距离和误差
        distance = int(-tag.z_translation() * K_z)  # 真实的距离，单位为mm
        error_x = 80 - tag.cx()  # x方向的误差
        error_y = 60 - tag.cy()  # y方向的误差

        # 限制误差范围
        error_x = max(min(error_x, 80), -80)
        error_y = max(min(error_y, 40), -40)

        tag_found = True
        break  # 只处理第一个检测到的标签

    # 如果没有检测到标签，使用默认值
    if not tag_found:
        distance = 0
        error_x = 0
        error_y = 0

    # 转换为补码形式
    tx_distance = min(distance // 10, 255)  # 限制在0-255范围内
    tx_error_x = to_twos_complement(int(error_x))
    tx_error_y = to_twos_complement(int(error_y))

    # 创建数据帧并发送
    FH = bytearray([0x2C, 0x12, tx_distance, tx_error_x, tx_error_y, 0x5B])
    # 在发送前添加
    print("Sending:", [hex(x) for x in FH])
    uart.write(FH)

    # 打印调试信息
    print("Distance:", tx_distance, "cm | X:", error_x, "| Y:", error_y,
          "| Raw:", tx_distance, tx_error_x, tx_error_y)

    time.sleep_ms(50)  # 添加延时避免发送过快
