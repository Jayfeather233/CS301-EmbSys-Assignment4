/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim3;

extern uint8_t scence_id;

extern const uint16_t grass1[5184];
extern const uint16_t grass1_trans[324];
extern const uint16_t grass2[5184];
extern const uint16_t grass2_trans[324];
extern const uint16_t grass3[5184];
extern const uint16_t grass3_trans[324];
extern const uint16_t grass4[5184];
extern const uint16_t grass4_trans[324];
extern const uint16_t grass5[5184];
extern const uint16_t grass5_trans[324];
extern const uint16_t end_obj[5184];
extern const uint16_t end_obj_trans[324];
extern const uint16_t solid_obj[5184];
extern const uint16_t solid_obj_trans[324];

extern const uint16_t ground1[6400];
extern const uint16_t ground1_trans[400];
extern const uint16_t ground2[6400];
extern const uint16_t ground2_trans[400];

extern const uint16_t obstacle[5184];
extern const uint16_t obstacle_trans[324];
extern const uint16_t obstacle_crouch[5184];
extern const uint16_t obstacle_crouch_trans[324];
extern const uint16_t obstacle_dash[5184];
extern const uint16_t obstacle_dash_trans[324];
extern const uint16_t obstacle_jump[5184];
extern const uint16_t obstacle_jump_trans[324];

extern const uint16_t slugcat_run[22][1024];
extern const uint16_t slugcat_run_trans[22][64];

extern const uint16_t score_obj[4][1024];
extern const uint16_t score_obj_trans[4][64];

extern const uint16_t stop[6][256];
extern const uint16_t stop_trans[6][16];

extern uint8_t need_draw, need_logic;
extern float speed_x, speed_y;
extern float total_x, end_position;
extern uint8_t sig_end, end_msg_display;
extern uint8_t scence_id, score;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void show_warn(const char* s);
void show_game_status();
void game_status_update();
void MS_write_score(uint8_t score);
extern uint8_t max_score;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_WKUP_Pin GPIO_PIN_0
#define KEY_WKUP_GPIO_Port GPIOA
#define KEY_WKUP_EXTI_IRQn EXTI0_IRQn
#define KEY0_Pin GPIO_PIN_5
#define KEY0_GPIO_Port GPIOC
#define KEY0_EXTI_IRQn EXTI9_5_IRQn
#define LED0_Pin GPIO_PIN_8
#define LED0_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_15
#define KEY1_GPIO_Port GPIOA
#define KEY1_EXTI_IRQn EXTI15_10_IRQn
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
