#include <Arduino.h>
 
#define LED_U0RXD 44
#define LED_U0TXD 43
 
// 创建两个任务，
// TaskBlink1任务：一个LED 1s亮一次，
// TaskBlink2任务：另一个LED 2s亮一次。
void TaskBlink1( void *pvParameters );
void TaskBlink2( void *pvParameters );
 
void setup() {
  Serial0.begin(115200);
  uint32_t blink_delay1 = 1000; // Delay between changing state on LED pin
  uint32_t blink_delay2 = 2000; // Delay between changing state on LED pin
 
  xTaskCreate(
    TaskBlink1       // 这个任务运行的函数
    ,  "Task Blink1" //  给人看的名字
    ,  2048        // 任务栈的大小，用于存储任务运行时的上下文信息。简单来说，就是最多存这么多信息
    ,  (void*) &blink_delay1 // 任务参数。要么没有填NULL；要么必须为无类型指针
    ,  2  // 优先级
    ,  NULL // 任务的句柄，用于管理和控制任务，NULL相当于0，意味着此处不需要任务句柄
    );
 
 
  xTaskCreate(
    TaskBlink2       // 这个任务运行的函数
    ,  "Task Blink2" //  给人看的名字
    ,  2048        // 任务栈的大小，用于存储任务运行时的上下文信息。简单来说，就是最多存这么多信息
    ,  (void*) &blink_delay2 // 任务参数。要么没有填NULL；要么必须为无类型指针
    ,  2  // 优先级
    ,  NULL // 任务的句柄，用于管理和控制任务，NULL相当于0，意味着此处不需要任务句柄
    );
 
  Serial0.printf("Basic Multi Threading Arduino Example\n");
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}
 
void loop(){
// 这里什么都不用写
}
 
/*---------------------- Tasks ---------------------*/
 
void TaskBlink1(void *pvParameters){  // This is a task.
  uint32_t blink_delay = *((uint32_t*)pvParameters);
 
  // 初始化LED_U0RXD为output模式
  pinMode(LED_U0RXD, OUTPUT);
// for死循环
  for (;;){ // 多线程，每个任务必须是死循环
    digitalWrite(LED_U0RXD, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(blink_delay);
    digitalWrite(LED_U0RXD, LOW);    // turn the LED off by making the voltage LOW
    delay(blink_delay);
  }
}
 
void TaskBlink2(void *pvParameters){  // This is a task.
  uint32_t blink_delay = *((uint32_t*)pvParameters);
 
  // 初始化LED_U0RXD为output模式
  pinMode(LED_U0TXD, OUTPUT);
 
  for (;;){ // A Task shall never return or exit.
    digitalWrite(LED_U0TXD, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(blink_delay);
    digitalWrite(LED_U0TXD, LOW);    // turn the LED off by making the voltage LOW
    delay(blink_delay);
  }
}