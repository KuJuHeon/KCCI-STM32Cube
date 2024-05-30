#include "main.h"
#include "lcd.h"

#define TLCD_RS  GPIO_PIN_0                  // LCD RS 신호 
#define TLCD_RW  GPIO_PIN_1                  // LCD RW 신호 
#define TLCD_E   GPIO_PIN_2                  // LCD E 신호 
//#define TLCD_EN { GPIO_ResetBits(GPIOB, TLCD_E); GPIO_SetBits(GPIOB, TLCD_E); }
#define TLCD_EN { HAL_GPIO_WritePin(GPIOB, TLCD_E, GPIO_PIN_RESET); HAL_GPIO_WritePin(GPIOB, TLCD_E, GPIO_PIN_SET); }
#define DATA     GPIOA                       // LCD 데이터 신호 
#define DATAPIN 5 //처음 시작이 0번이 아니라 5번부터 시작이라서 그만큼 이동시키는 게 편함



/*
    GPIOC->ODR = (GPIOC->ODR |0x0f00); //FND all off
    GPIO_Write(GPIOC,GPIO_ReadInputData(GPIOC) | 0x0f00); //fnd all off
*/
// delay 함수
/*static void Delay(const uint32_t Count)
{
    __IO uint32_t index = 0;
    for (index = (16800 * Count); index != 0; index--);
}*/
// delay 함수
static void Delay(const uint32_t Count)
{
  __IO uint32_t index = 0;
  for(index = (16800 * Count); index != 0; index--);
}
static void Delay_us(const uint32_t Count)
{
    __IO uint32_t index = 0;
    for (index = (16 * Count); index != 0; index--);
}

// LCD 에 모든 명령을 전송한후 반드시 Enable Pulse를 주어야 한다.
void E_Pulse(void)
{
    //GPIO_SetBits(GPIOB, TLCD_E);
    HAL_GPIO_WritePin(GPIOB, TLCD_E, GPIO_PIN_SET);
    Delay(5);
    //GPIO_ResetBits(GPIOB, TLCD_E);
    HAL_GPIO_WritePin(GPIOB, TLCD_E, GPIO_PIN_RESET);
}

// LCD에 데이터를 전송하는 함수이다   
void TLCD_DATA(unsigned char data)
{
    DATA->ODR = data << DATAPIN; //FND all off
    //GPIO_Write(DATA, data << DATAPIN);
    TLCD_EN;    // 이 부분에서는  E_Pulse() 함수 대신 사용한다.
}

void Func_Set(void)
{
    //GPIO_ResetBits(GPIOB, TLCD_RW);        // RW = 0, RS = 0 일 때 IR쓰기 명령
    HAL_GPIO_WritePin(GPIOB, TLCD_RW, GPIO_PIN_RESET);
    //GPIO_ResetBits(GPIOB, TLCD_RS);
    HAL_GPIO_WritePin(GPIOB, TLCD_RS, GPIO_PIN_RESET);
    //GPIO_Write(DATA, 0x38);               // 초기화를 위한 Function Set
    //GPIO_Write(DATA, 0x38 << DATAPIN);
    DATA->ODR = 0x38 << DATAPIN;
    E_Pulse();                            // Enable Pulse
}

// LCD 초기화 함수
void LCD_Init(void)
{
    //GPIO_ResetBits(GPIOB, TLCD_E);
    HAL_GPIO_WritePin(GPIOB, TLCD_E, GPIO_PIN_RESET);
    Delay(15);                    // 약 15ms 시간 지연
    Func_Set();
    Delay(10);                    // 약 10ms 시간 지연
    Func_Set();
    Delay_us(150);                // 약 150us 시간 지연
    Func_Set();
    //이게 다 데이터 시트에 명령어 있음
    //GPIO_Write(DATA, 0x0f << DATAPIN);       // 디스플레이 ON, 커서 ON
    DATA->ODR = 0x0f << DATAPIN;
    E_Pulse();
    //문자열 출력하고 문자열 주소 등가 or 감소?자동으로 오른쪽을 주소를 올리겠다?
    //GPIO_Write(DATA, 0x06 << DATAPIN);       // 글자를 오른쪽으로 시프트 시키며 출력 
    DATA->ODR = 0x06 << DATAPIN;
    E_Pulse();
    //GPIO_Write(DATA, 0x01 << DATAPIN);       // 화면 클리어
    DATA->ODR = 0x01 << DATAPIN;
    E_Pulse();
}

/******************************************
   lcd_char() : 1 문자를 디스플레이 하는 함수
*******************************************/
void lcd_char(char s)
{
    //GPIO_SetBits(GPIOB, TLCD_RS);
    HAL_GPIO_WritePin(GPIOB, TLCD_RS, GPIO_PIN_SET);
    //GPIO_Write(DATA, s << DATAPIN);
    DATA->ODR = s << DATAPIN;
    E_Pulse();
}

void lcd_disp(char x, char y)
{
    //GPIO_ResetBits(GPIOB, TLCD_RS);
    HAL_GPIO_WritePin(GPIOB, TLCD_RS, GPIO_PIN_RESET);
    //GPIO_ResetBits(GPIOB, TLCD_RW);
    HAL_GPIO_WritePin(GPIOB, TLCD_RW, GPIO_PIN_RESET);
    //
    if (y == 0) //GPIO_Write(DATA, (x + 0x80) << DATAPIN); 
        DATA->ODR = (x + 0x80) << DATAPIN;
    else if (y == 1) //GPIO_Write(DATA, (x + 0xc0) << DATAPIN);
        DATA->ODR = (x + 0xc0) << DATAPIN;
    E_Pulse();
}

void move_disp(char p)
{
    //이게 다 데이터시트 보면 둘 다 0으로 만들어야 해당 명령어 가능
    //GPIO_ResetBits(GPIOB, TLCD_RS);
    HAL_GPIO_WritePin(GPIOB, TLCD_RS, GPIO_PIN_RESET);
    //GPIO_ResetBits(GPIOB, TLCD_RW);
    HAL_GPIO_WritePin(GPIOB, TLCD_RW, GPIO_PIN_RESET);

    if (p == LEFT) //GPIO_Write(DATA, (0x18) << DATAPIN);
        DATA->ODR = (0x18) << DATAPIN;
    else if (p == RIGHT) //GPIO_Write(DATA, (0x1c) << DATAPIN);
        DATA->ODR = (0x1c) << DATAPIN;

    E_Pulse();
}

void disp_ON_OFF(char d, char c, char b)
{
    char disp = 0x08;

    //GPIO_ResetBits(GPIOB, TLCD_RS);
    HAL_GPIO_WritePin(GPIOB, TLCD_RS, GPIO_PIN_RESET);
    //GPIO_ResetBits(GPIOB, TLCD_RW);
    HAL_GPIO_WritePin(GPIOB, TLCD_RW, GPIO_PIN_RESET);

    if (d == ON) d = 0x04;
    else         d = 0x00;

    if (c == ON) c = 0x02;
    else          c = 0x00;

    if (b == ON) b = 0x01;
    else          b = 0x00;

    //GPIO_Write(DATA, (disp | d | c | b) << DATAPIN);
    DATA->ODR = (disp | d | c | b) << DATAPIN;
    E_Pulse();

}

void clrscr(void)
{
    //GPIO_ResetBits(GPIOB, TLCD_RS);
    HAL_GPIO_WritePin(GPIOB, TLCD_RS, GPIO_PIN_RESET);
    //GPIO_ResetBits(GPIOB, TLCD_RW);
    HAL_GPIO_WritePin(GPIOB, TLCD_RW, GPIO_PIN_RESET);
    //GPIO_Write(DATA, 0x01 << DATAPIN);
    DATA->ODR = 0x01 << DATAPIN;
    E_Pulse();

    Delay(10);
}


/***************************************************************
   LCD 에 위치를 지정하여 문자열을 출력할 수 있도록 만든 함수이다
****************************************************************/
void lcd(char x, char y, char* str)
{
    lcd_disp(x, y);
    while (*str) lcd_char(*str++);
}
