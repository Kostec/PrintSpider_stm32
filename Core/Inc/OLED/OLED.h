/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#ifndef OLED_H
#define OLED_H

/*
 * The module process UI for OLED display
*/

/// @brief Initialize the OLED module
void OLED_Init();
/// @brief OLED loop task
/// @param pvParameters 
void OLED_Task(void *pvParameters);

#endif //OLED_H