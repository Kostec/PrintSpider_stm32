#ifndef OLED_H
#define OLED_H

/// @brief Initialize the OLED module
void OLED_Init();
/// @brief OLED loop task
/// @param pvParameters 
void OLED_Task(void *pvParameters);

#endif //OLED_H