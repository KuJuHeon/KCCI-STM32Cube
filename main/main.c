/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE 송

  printf("%s\r\n",__FILE__);
  printf("main() Start!!\r\n");

  FND_Init();
  DCMOTOR_Init();
  LCD_Init();

  disp_ON_OFF(ON,OFF,OFF);
  //HAL_TIM_Base_Start_IT(&htim10);
  //TIM10->CCR1 = standard;
  //HAL_TIM_PWM_Stop(&htim10,TIM_CHANNEL_1);
  //HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
  //DCMOTOR_Init();
  display_fnd(0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (dc_start == 0 && stopFlag == 1) {
		  clrscr();
		  lcd(0, 0, "stop");
		  //lcd(0,1,"stop");
		  stopFlag = 0;
	  }
	  if (rx2Flag)
	  {
		  printf("recv2 : %s\r\n", rx2Data);
		  clrscr();
		  size = strlen(rx2Data);
		  if (size < 17) {
			  //(x,y) y가 행
			  lcd(0, 0, rx2Data); //1번째 행에 표시
		  }
		  //문자열 길이가 size<41 넘으면
		  else if (size < 33) {
			  //두 줄에 나눠서 표시
			  lcd(0, 0, rx2Data); 
			  lcd(0, 1, rx2Data + 16);
		  }
		  rx2Flag = 0;
	  }
	  //타이머 플래그
	  if (m_cntFlag) {
		  //fnd에 현재 초 출력
		  display_fnd(m_cnt);
		  //타이머 플래그 초기화
		  m_cntFlag = 0;
		  //런닝머신 동작 상태라면 3초마다 lcd 갱신해주자
		  if (dc_start == 1 && m_cnt % 3 == 0) {
			  clrscr();
			  sprintf(km_kcal, "km: %.2lf", km);
			  lcd(0, 0, km_kcal);
			  sprintf(km_kcal, "kcal: %.1lf", kcal);
			  lcd(0, 1, km_kcal);
		  }
	  }
	  if (key != 0) {
		  printf("key: %d\r\n", key);
		  //1번 키 눌렀을 때
		  if (key == 1) {
			  //런닝머신 멈춰있었다면 시작
			  if (dc_start == 0) {
				  //타이머 초기화하고 오버플로우 시 인터럽트 모드
				  MX_TIM7_Init();
				  HAL_TIM_Base_Start_IT(&htim7);
				  printf("Type : start\r\n");
				  //lcd
				  clrscr();
				  lcd(0, 0, "start");
				  dc_start = 1;
				  TIM10->CCR1 = standard;
				  HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
				  DCMOTOR_ON();
				  //해당 숫자를 FND로 표시
				  display_fnd(m_cnt);
			  }
			  //런닝머신 움직이고 있었다면 멈추고 결과 전송
			  else if (dc_start == 1) {
				  HAL_TIM_Base_Stop_IT(&htim7);
				  printf("Type : stop\r\n");

				  HAL_TIM_PWM_Stop(&htim10, TIM_CHANNEL_1);
				  DCMOTOR_OFF();
				  //끝났으면 결과 전송
				  bluetooth_Event();
				  //변수들 초기화
				  dc_start = 0;
				  stopFlag = 1;
				  m_cnt = 0;
				  m_cntFlag = 0;
				  duty = 30;
				  km = 0;
				  kcal = 0;
				  display_fnd(0);
				  //마지막에 4개의 FND 전부 0으로 켜주기
				  GPIOC->ODR = (GPIOC->ODR & 0xf000 | Font[0]);

			  }
		  }
		  //2,3번 버튼(속도 제어용) 눌렀다면
		  else if (key == 2 || key == 3)
		  {
			  //2번 버튼이라면 속도 감소
			  if (key == 2 && duty >= 10)
			  {
				  duty -= 10;
				  //printf("Type : low %d\r\n");
			  }
			  //3번 버튼이라면 속도 증가
			  else if (key == 3 && duty <= 90)
			  {
				  duty += 10;
				  //printf("Type : high %d\r\n");
			  }
			  pwm = (int)(17700 * (duty / 100.0));
			  printf("speed : %d\r\n", duty);
			  //이 pwm값을 실제 적용하자
			  __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, pwm);
		  }
		  key = 0;
	  }
	  /* USER CODE END WHILE */

	  /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void) //타이머
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 999;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
//static void MX_TIM10_Init(void)
//{
//
//  /* USER CODE BEGIN TIM10_Init 0 */
////////
//  /* USER CODE END TIM10_Init 0 */
//
//  TIM_OC_InitTypeDef sConfigOC = {0};
//
//  /* USER CODE BEGIN TIM10_Init 1 */
////////
//  /* USER CODE END TIM10_Init 1 */
//  htim10.Instance = TIM10;
//  htim10.Init.Prescaler = 167;
//  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim10.Init.Period = 17699;
//  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
//  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  if (HAL_TIM_PWM_Init(&htim10) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sConfigOC.OCMode = TIM_OCMODE_PWM1;
//  sConfigOC.Pulse = 0;
//  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
//  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
//  if (HAL_TIM_PWM_ConfigChannel(&htim10, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN TIM10_Init 2 */
////////
//  /* USER CODE END TIM10_Init 2 */
//  HAL_TIM_MspPostInit(&htim10);
//
//}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_5|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 PC6 PC7
                           PC8 PC9 PC10 PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 PA8
                           PA9 PA10 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB5 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_5|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM7)
	{
		static int digit = 0;
		static int t_cnt = 0;
		t_cnt++;
		if(t_cnt >= 1000)	//1Sec
		{
			//printf("hihihi\r\n");
			t_cnt = 0;
			m_cnt++;
			m_cntFlag = 1;

			//km, kcal 계산
			km += ((double)(duty/10)) / 3600;
			kcal += 0.0157*((0.1*(duty/10)*3.5)/3.5)*60;
		}
		display_digit(digit,arrayNum[digit]);
		digit = (digit + 1)%4;
	}
}
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2)
    {
    	static int i=0;
    	rx2Data[i] = cdata;
    	if(rx2Data[i] == '\r')
    	{
    		rx2Data[i] = '\0';
    		rx2Flag = 1;
    		i = 0;
    	}
    	else
    	{
    		i++;
    	}
		//1바이?�� ?��?��?���???????? ?��?��
    	HAL_UART_Receive_IT(&huart2, &cdata,1);
    }
}
void FND_Init(){
	MX_GPIO_Init();
	//MX_TIM7_Init();
	//?��?��?��?�� start?
	//HAL_TIM_Base_Start_IT(&htim7);
}
void display_digit(int pos,int num) //해당 pos에 숫자 num을 표시하겠다
{
	if(fndFlag)
	{
		//IAR?��?��?�� ?��?���??????? PORTC?�� ?? ?��?�� ?�� �????????��?��?��?�� ?��기느 그런 ?��?�� ?��?��?�� 그려?��
		GPIOC->ODR = (GPIOC->ODR |0x0f00); //FND all off
		GPIOC->ODR = (GPIOC->ODR & ~(GPIO_PIN_8 << pos));
		GPIOC->ODR = (GPIOC->ODR & 0xff00 | Font[num]);
		//GPIO_Write(GPIOC,GPIO_ReadInputData(GPIOC) | 0x0f00); //fnd all off
		//GPIO_Write(GPIOC,GPIO_ReadInputData(GPIOC) & ~(GPIO_Pin_8 << pos));
		//GPIO_Write(GPIOC,(GPIO_ReadInputData(GPIOC) & 0xff00 )| Font[num]);
        //Delay(1);
	}
}
void display_fnd( int N )  // 세그먼트를 N의 값으로 표시하겠다
{
  int Buff ;

   if(N < 0)
  {
    N=-N;
    arrayNum[0] = 18;
  }
  else
    arrayNum[0] = N /1000;  //세그먼트 천의 자리 추출

  Buff = N % 1000 ;
  arrayNum[1] = Buff / 100 ; // 세그먼트 백의 자리 추출
  Buff = Buff % 100;
  arrayNum[2] = Buff /10 ;      // 세그먼트 십의 자리 추출
  arrayNum[3] =  Buff % 10 ;    // 세그먼트 일의 자리 추출
}

void display_onoff(int flag)
{

  fndFlag = flag;

  if(!fndFlag)
  {
	  GPIOC->ODR = (GPIOC->ODR |0x0f00); //FND all off
    //GPIO_Write(GPIOC,GPIO_ReadInputData(GPIOC) | 0x0f00); //fnd all off
  }
}
void bluetooth_Event(){
	char sendBuf[CMD_SIZE]={0};
    sprintf(sendBuf,"[%s]%s@%.2lf@%.1lf@%d\n","KJH_GYM","MACHINE",km,kcal,m_cnt);
    HAL_UART_Transmit(&huart4, (uint8_t *)sendBuf, strlen(sendBuf), 0xFFFF);
    HAL_Delay(500);
    sprintf(sendBuf,"[%s]%s@%.2lf@%.1lf@%d\n","KJH_AND","MACHINE",km,kcal,m_cnt);
    HAL_UART_Transmit(&huart4, (uint8_t *)sendBuf, strlen(sendBuf), 0xFFFF);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
