# xiaoqiang_greeting

## 功能说明

小强迎宾功能，当人从小强面前走过时让小强说欢迎

## 使用

```bash
roslaunch xiaoqiang_greeting xiaoqiang_greeting.launch
```

在小强上测试

```bash
# 关闭startup服务
sudo service startup stop
# 启动 greeting
roslaunch xiaoqiang_greeting xiaoqiang_greeting_test.launch
```

在小强摄像头前面走过就会有语音提示了

### 订阅话题

|话题|类型|说明|
|--|--|--|
|image|sensor_msgs/Image|摄像头图像输入话题|
|odom|nav_msgs/Odometry|机器人速度状态，运动状态下不进行检测|

### 输出话题

|话题|类型|说明|
|--|--|--|
|text|std_msgs/String|输出的欢迎语句|
|result|std_msgs/Int64|图像变化计算结果|
|processed_image|sensor_msgs/Image|处理后的图像输出|

### 参数说明

请参照 [xiaoqiang_greeting.launch](./launch/xiaoqiang_greeting.launch) 文件注释