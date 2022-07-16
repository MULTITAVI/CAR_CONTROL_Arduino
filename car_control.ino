#include <DynamixelWorkbench.h>

#if defined(__OPENCM904__)
#define DEVICE_NAME "3" //Dynamixel on Serial3(USART3) <-OpenCM 485EXP
#elif defined(__OPENCR__)
#define DEVICE_NAME ""
#endif

#define BAUDRATE 1000000

int motor_count = 4;

DynamixelWorkbench dx1;

void setup() {
  Serial.begin(115200);

  const char *log;
  uint16_t model_number = 0;

  for (int i = 1; i <= motor_count; i++) {
    dx1.init(DEVICE_NAME, BAUDRATE, &log);
    dx1.ping(i, &model_number, &log);
    dx1.wheelMode(i, 0, &log);
  }
}

void Spin_wheel(int motor_ID, int velocity)//max_velocity = 255, velocity>0 - вперед {
  if (motor_ID == 2 || motor_ID == 4)
  velocity = - velocity;

  dx1.goalVelocity(motor_ID, (int32_t)velocity);
}

void Move_to_vector(float (&vector)[2], float w, int velocity)//Функция, задающая скорости колёс так, чтобы машинка двигалась в нужном направлении, w - угловая скорость -1 <= w <= 1,по ч.с. >0 {
  float x = vector[0];
  float y = vector[1];

  int velocity_x = velocity * x / sqrt(pow(x, 2) + pow(y, 2)); //скорость машины вдоль осей
  int velocity_y = velocity * y / sqrt(pow(x, 2) + pow(y, 2));

  float wheel_velocity[motor_count + 1]; //скорость отдельного колеса

  wheel_velocity[1] = velocity_x - velocity_y;
  wheel_velocity[2] = velocity_x + velocity_y;
  wheel_velocity[3] = velocity_x + velocity_y;
  wheel_velocity[4] = velocity_x - velocity_y;

  wheel_velocity[1] = wheel_velocity[1] * abs(1 - w) + w * velocity;
  wheel_velocity[3] = wheel_velocity[3] * abs(1 - w) + w * velocity;
  wheel_velocity[2] = wheel_velocity[2] * abs(1 - w) - w * velocity;
  wheel_velocity[4] = wheel_velocity[4] * abs(1 - w) - w * velocity;

  for (int i = 1; i <= motor_count; i++)
  Spin_wheel(i, wheel_velocity[i]);
}

void Keyboard_control(int velocity) {
  float x = 0, y = 0;
  float w = 0;

  char val = Serial.read();
  if (val != '\n') flash();

  if (val == '8') x += 1;
  else if (val == '5') x -= 1;
  else if (val == '4') y += 1;
  else if (val == '6') y -= 1;
  else if (val == '9') w += 1; //вращение по ч.с.
  else if (val == '7') w -= 1; //вращение против ч.с.

  float vector[2];
  vector[0] = x;
  vector[1] = y;

  Move_to_vector(vector, w, velocity);
}

void flash() {
  while(Serial.read() != '\n');
}

void loop() {
  if (Serial) {
    if(Serial.available() > 0) {
      int velocity = 255;
      Keyboard_control(velocity);
    }
  }
}
