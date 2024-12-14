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
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "touch.h"
#include "lcd.h"
#include "delay.h"

#include "objects.h"
#include "draw.h"
#include "control.h"

#include "24cxx.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SCORE_SAVE_ADDR_BASE 60
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t need_draw = 0, need_logic = 1;
float speed_x = 0, speed_y = 0;
float total_x = 0, end_position = 3000;
uint8_t sig_end = 0, end_msg_display = 0;
uint8_t scence_id = 0, score = 0, max_score = 0;

list_t obj_list_head;
// button -> ground -> character -> obstacle
list_t *ground_list_head = NULL, *obstacle_list_head = NULL, *character_list_head = NULL;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void show_greeting();
void init_button(list_t *p);
void init_ground(list_t *p);
void init_character(list_t *p);
void elements_init();
void show_warn(const char* s);
uint8_t MS_read_score();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  srand(HAL_GetTick());
  HAL_Delay_Init(72);
  LCD_Init();
  tp_dev.init(); // AT24Cxx Inited  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  LCD_Display_Dir(1);
  POINT_COLOR = RED;

  show_greeting();

  elements_init();

  HAL_Delay(1500);
  srand(HAL_GetTick());

  need_draw = need_logic = 0;
  max_score = MS_read_score();
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Start_IT(&htim4);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (need_draw && end_msg_display == 0 && need_logic <= 4){
      need_draw -= 1;
      draw_all(obj_list_head.next);
      show_game_status();
      if (sig_end){
        //TODO press key1 to restart
        end_msg_display = 1;
        LCD_Fill_Window(0, 80, 320, 170, WHITE);
        LCD_ShowString(50, 90, 200, 24, 24, "Game Finished!");
        show_game_status();
        if (max_score < score){
          max_score = score;
          MS_write_score(max_score);
        }
      }
      if (need_draw > 4){
        show_warn("                          ");
        show_warn("Drawing exceed time limits");
      }
    }
    if (need_logic && end_msg_display == 0){
      need_logic -= 1;
      game_logic(ground_list_head->next);
      update_elements(ground_list_head->next);
      if (need_logic > 4){
        // show_warn("Logic exceed time limits");
      }
    }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void show_greeting(){
  LCD_ShowString(30, 50, 200, 16, 16, "Created by JCX");
  LCD_ShowString(30, 70, 300, 16, 16, "Character/Scene Design:");
  LCD_ShowString(30, 90, 300, 16, 16, "  Videocult: Rain World");
  LCD_ShowString(30, 110, 200, 16, 16, "Artist: JCX");
  LCD_ShowString(30, 130, 200, 16, 16, "Programmer: JCX");
  LCD_ShowString(30, 150, 200, 16, 16, "24/12/01");
  if (tp_dev.touchtype != 0XFF)
  {
    LCD_ShowString(30, 170, 200, 16, 16, "Press KEY0 to Adjust"); // 电阻屏才显示
  }
}
/*
 240 - 
*/

static const uint16_t xfront = 272;
static const uint16_t ybottom = 240;
static const uint16_t lll = 5;
static const char button_s[5][20] =
{
  "UP",
  "JUMP",
  "CROUCH",
  "DASH",
  "DOWN"
};
static const uint16_t button_v[5] =
{
  MOVE_UP,
  JUMP_M,
  CROUCH_M,
  DASH_M,
  MOVE_DOWN
};

void init_button(list_t *p){
  // uint16_t bheight = 24, by=ybottom;
  // for(int16_t i = lll-1; i >= 0; --i){
  //   by -= bheight;
  //   myobj_t *pp = new_myobj(xfront, by, BUTTON);
  //   pp->data->img_x = lcddev.width - xfront;
  //   pp->data->img_y = bheight;
  //   uint16_t *ppx = (uint16_t*)malloc(sizeof(uint16_t)*2);
  //   ppx[0] = rand();
  //   ppx[1] = button_v[i];
  //   pp->data->data = ppx;
  //   pp->data->mask = button_s[i];
  //   pp->data->scale = 1;
  //   insert_lt(p, pp);
  //   // by -= 6;
  // }
  uint16_t bheight = 24;
  uint16_t by = lcddev.height-bheight, bx = lcddev.width, dx = lcddev.width - xfront;
  for(int16_t i = lll-1; i >= 0; --i){
    bx -= dx;
    myobj_t *pp = new_myobj(bx, by, BUTTON);
    pp->data->img_x = lcddev.width - xfront;
    pp->data->img_y = bheight;
    uint16_t *ppx = (uint16_t*)malloc(sizeof(uint16_t)*2);
    ppx[0] = rand();
    ppx[1] = button_v[i];
    pp->data->data = ppx;
    pp->data->mask = button_s[i];
    pp->data->scale = 1;
    insert_lt(p, pp);
    // by -= 6;
  }
}
void init_ground(list_t *p){
  insert_lt(p, new_myobj(0, 62, GROUND));
  insert_lt(p, new_myobj(0, 142, GROUND));
  insert_lt(p, new_myobj(0, 222, GROUND));
  insert_lt(p, new_myobj(320, 62, GROUND));
  insert_lt(p, new_myobj(320, 142, GROUND));
  insert_lt(p, new_myobj(320, 222, GROUND));
}
void init_character(list_t *p){
  insert_lt(p, new_myobj(10, 120, FIGURE));
}
void show_warn(const char* s){
  LCD_ShowString(10, 10, 300, 12, 12, s);
}
void elements_init(){
  if(ground_list_head != NULL){
    free_lt(ground_list_head->next);
  } else {
    init_button(&obj_list_head);
  }


  ground_list_head = &obj_list_head;
  while(ground_list_head->next != NULL){
    ground_list_head = ground_list_head->next;
  }

  init_ground(ground_list_head);

  character_list_head = ground_list_head;
  while(character_list_head->next != NULL){
    character_list_head = character_list_head->next;
  }

  init_character(character_list_head);

  obstacle_list_head = character_list_head;
  while(obstacle_list_head->next != NULL){
    obstacle_list_head = obstacle_list_head->next;
  }
}

static char gs_str1[20] = "Time: ";
static char gs_str2[20] = "Score: ";
static char gs_str3[20] = "MAX Score: ";
static uint16_t tim_tick_s = 0, tim_tick_m = 0;

void game_status_update(){
  tim_tick_s += 1;
  if (tim_tick_s == 60) {
    tim_tick_m += 1;
    tim_tick_s = 0;
  }
}

void show_game_status(){
  sprintf(gs_str1+6, "%02d:%02d", tim_tick_m, tim_tick_s);
  sprintf(gs_str2+7, "%02d", score);
  sprintf(gs_str3+11, "%02d", max_score);
  if (tim_tick_m == 2){
    sig_end = 1;
    end_msg_display = 1;
    LCD_Fill_Window(0, 80, 320, 170, WHITE);
    LCD_ShowString(50, 90, 200, 24, 24, "Exceed Time Limits!");
    LCD_ShowString(100, 120, 200, 12, 12, gs_str1);
    LCD_ShowString(100, 136, 200, 12, 12, gs_str2);
    LCD_ShowString(100, 152, 200, 12, 12, gs_str3);
  }
  if (end_msg_display || sig_end){
    LCD_ShowString(100, 120, 200, 12, 12, gs_str1);
    LCD_ShowString(100, 136, 200, 12, 12, gs_str2);
    LCD_ShowString(100, 152, 200, 12, 12, gs_str3);
  } else {
    LCD_ShowString(10, 10, 200, 12, 12, gs_str1);
    LCD_ShowString(10, 26, 200, 12, 12, gs_str2);
    LCD_ShowString(10, 42, 200, 12, 12, gs_str3);
  }
}

uint8_t MS_read_score(){
  uint8_t tmp = AT24CXX_ReadOneByte(SCORE_SAVE_ADDR_BASE);
  if (tmp == 0x0A) {
    return AT24CXX_ReadOneByte(SCORE_SAVE_ADDR_BASE+1);
  } else {
    return 0;
  }
}
void MS_write_score(uint8_t score){
  AT24CXX_WriteOneByte(SCORE_SAVE_ADDR_BASE, 0x0A);
  AT24CXX_WriteOneByte(SCORE_SAVE_ADDR_BASE+1, score);
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
