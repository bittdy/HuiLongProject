# HuiLongProject
Code for the UGV of HuiLong

hookdll是一个windows下用钩子监控键盘的dll，hook是对其的调用，同时使用socket发送采集到的按键。但是代码效果在发送一段时间按键后server不再接收，怀疑是windows下钩子函数的回调机制的影响，留个坑==

testClient是采集键盘输入，并使用socket发送的客户端
testServer是接收信息，并按照rs232转8路开关量设备(FT-RSA-0882)的协议发送rs232串口数据的服务器端
