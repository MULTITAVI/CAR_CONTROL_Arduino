import serial

BAUDRATE = int(input('Введите значение BAUDRATE (для работы с нашим скетчем, введите 115200)\n'))
PORT = input('Введите значение PORT\n')

ser = serial.Serial(baudrate=BAUDRATE, port=PORT)

if not ser.isOpen():
    ser.open()

print('Управление: 4 - влево, 8 - вперед, 5 - назад, 6 - вправо, 7 - вращение против часовой, 9 - по часовой, '
      '0 - завершить')
print('Вводите команды:')

rule = ''

while rule != '0':
    rule = input()
    ser.write(rule)

ser.close()
