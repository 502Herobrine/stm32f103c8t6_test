# STM32F103C8T6 蜂鸣器+舵机控制项目

## 📁 项目文件结构

```
buzzer/
├── platformio.ini              # PlatformIO 项目配置
├── include/
│   └── buzzer.h               # 蜂鸣器驱动头文件
├── lib/
│   ├── buzzer.c               # 蜂鸣器驱动实现
│   └── servo/
│       ├── servo_control.h    # 舵机驱动头文件
│       └── servo_control.c    # 舵机驱动实现
├── src/
│   ├── main.c                 # 主程序入口
│   └── README.md              # 项目文档
└── test/
    └── README
```

## 🔧 硬件平台

- **微控制器**：STM32F103C8T6
- **系统时钟**：72MHz (HSE 8MHz × 9)
- **编程工具**：ST-Link V2
- **固件框架**：STM32Cube HAL

## 📌 GPIO 配置表

### 蜂鸣器配置

| 功能 | GPIO 引脚 | 模式 | 说明 |
|------|---------|------|------|
| **蜂鸣器控制** | **PB13** | **推挽输出** | 低电平触发，持续发声 |

**初始化代码：**
```c
GPIO_InitStruct.Pin = GPIO_PIN_13;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  /* 推挽输出 */
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
```

**控制逻辑：**
- `HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)` → 蜂鸣器开启（低电平）
- `HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)` → 蜂鸣器关闭（高电平）

---

### 舵机 PWM 配置

| 功能 | GPIO 引脚 | 定时器 | 通道 | 说明 |
|------|---------|--------|------|------|
| **舵机信号** | **PA0** | **TIM2** | **CH1** | PWM 50Hz |

**PWM 参数：**
- **频率**：50Hz（周期 20ms）
- **分频系数**：72-1（1MHz 时钟）
- **计数周期**：20000（20ms）
- **脉宽范围**：500-2500 μs

**脉宽与转向对应关系：**
| 脉宽 | 状态 |
|------|------|
| **1000 μs** | 全速反转 |
| **1500 μs** | 停止（中位） |
| **2000 μs** | 全速正转 |

**舵机初始化代码流程：**
```c
1. 使能 GPIO 时钟：__HAL_RCC_GPIOA_CLK_ENABLE()
2. 使能 TIM2 时钟：__HAL_RCC_TIM2_CLK_ENABLE()
3. 配置 PA0 为 PWM 输出（复用推挽输出）
4. 初始化 TIM2：
   - 分频系数：71（使得计数器时钟 = 1MHz）
   - 计数模式：向上计数
   - 自动重装值：19999（产生 50Hz）
5. 配置 TIM2_CH1 为 PWM 模式 1
6. 启动 PWM 输出
```

**控制逻辑：**
```c
// 设置脉宽（500-2500 μs）
__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse_value);

// 例如：正转（中速）
Servo_Forward(50);  // speed 1-100

// 反转
Servo_Reverse(50);

// 停止
Servo_Stop();
```

---

## 📋 引脚分配总结

```
STM32F103C8T6 引脚分配：

PA0  ──→  TIM2_CH1  ──→  舵机信号（PWM 50Hz）
PB13 ──→  GPIO_OUT ──→  蜂鸣器控制（推挽输出）
GND  ──→  公共地
```

---

## 🚀 代码执行流程

```
main()
  ├─ HAL_Init()                    /* HAL 库初始化 */
  ├─ SystemClock_Config()          /* 系统时钟配置 72MHz */
  ├─ Buzzer_Init()                 /* 初始化蜂鸣器（PB13） */
  ├─ Servo_Init()                  /* 初始化舵机（PA0 - TIM2_CH1） */
  ├─ Servo_Forward(50)             /* 舵机中速正转 */
  └─ while(1)
      ├─ Buzzer_On()               /* 蜂鸣器保持开启 */
      ├─ HAL_Delay(100)            /* 延时 100ms */
      └─ (舵机 PWM 由定时器自动生成，持续正转)
```

---

## 🔌 硬件接线

### 蜂鸣器接线
```
有源蜂鸣器
  ├─ 信号脚 ──→ PB13（GPIO 推挽输出）
  ├─ 电源脚 ──→ VCC (正极)
  └─ 地脚   ──→ GND（与 STM32 共地）

工作原理：PB13 输出低电平（GPIO_PIN_RESET）时触发蜂鸣
```

### 舵机接线
```
SG90 标准舵机
  ├─ 棕色线（GND）  ──→ GND
  ├─ 红色线（VCC）  ──→ 5V 电源（需要独立电源，最好配 100μF 滤波电容）
  └─ 橙色线（信号）  ──→ PA0（TIM2_CH1 - PWM 输出）

脉宽解释：
  • 1.0ms  → 舵机转到 -90°（或全速反转）
  • 1.5ms  → 舵机保持中位（或停止）
  • 2.0ms  → 舵机转到 +90°（或全速正转）
```

---

## 📝 API 使用示例

### 蜂鸣器 API
```c
Buzzer_Init();              // 初始化
Buzzer_On();                // 开启（低电平）
Buzzer_Off();               // 关闭（高电平）
Buzzer_Toggle();            // 翻转状态
uint8_t state = Buzzer_GetState();  // 获取状态
```

### 舵机 API
```c
Servo_Init();               // 初始化
Servo_Forward(50);          // 正转，速度 50/100
Servo_Reverse(75);          // 反转，速度 75/100
Servo_SetPulse(1500);       // 设置脉宽 1500μs（中位）
Servo_Stop();               // 停止（脉宽 1500μs）
uint16_t pulse = Servo_GetPulse();  // 获取当前脉宽
```

---

## ⚙️ 系统时钟配置

```c
时钟源：HSE (外部晶振) = 8MHz
PLL 倍频：9×
最终 SYSCLK：8MHz × 9 = 72MHz

分频配置：
  ├─ AHB 分频：DIV1  (72MHz)
  ├─ APB1 分频：DIV2  (36MHz) - TIM2 在此域
  └─ APB2 分频：DIV1  (72MHz)
```

---

## 📌 编译和烧录

### 编译
```bash
platformio run
```

### 烧录
```bash
platformio run --target upload
```

### 监视串口
```bash
platformio device monitor
```

---

## ✅ 功能验证

上电后应观察到：
1. ✓ **蜂鸣器**：PA0 立即发声（持续，因为 PB13 保持低电平）
2. ✓ **舵机**：PA0 输出 50Hz PWM，舵机中速正转（脉宽约 2000μs）

---

## 🐛 常见问题

### Q1: 舵机不转动
- 检查 PA0 是否与舵机信号线正确连接
- 确认 5V 电源是否正常（舵机需要独立的 5V 电源）
- 检查地线是否与 STM32 共地

### Q2: 蜂鸣器不响
- 检查 PB13 是否为低电平（GPIO_PIN_RESET）
- 确认蜂鸣器的极性接正确（通常仅支持单一极性）
- 检查电源是否正常连接

### Q3: 上传时无法识别调试器
```bash
# Linux 权限问题解决
sudo chmod 666 /dev/bus/usb/001/010
```

---

## 📚 参考资源

- [STM32F103C8T6 数据手册](http://www.st.com/)
- [STM32 HAL 库参考](https://www.st.com/resource/en/user_manual/um1850-stm32cubef1-hal-and-ll-api-stage-2-driver-reference-manual.zip)
- [PlatformIO 官方文档](https://docs.platformio.org/)

---

**最后更新**: 2026-04-23
