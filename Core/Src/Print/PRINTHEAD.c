#include "Print/PRINTHEAD.h"
#include "LOG/LOG.h"

#define PIN_S4     (1U << 1)
#define PIN_S5     (1U << 2)
#define PIN_PWRB   (1U << 3)
#define PIN_PWRA   (1U << 4)
#define PIN_D3     (1U << 5)
#define PIN_D2     (1U << 6)
#define PIN_D1     (1U << 7)
#define PIN_DCLK   (1U << 8)
#define PIN_CSYNC  (1U << 9)
#define PIN_S1     (1U << 13)
#define PIN_S2     (1U << 14)
#define PIN_S3     (1U << 15)

#define PATTERN_LEN 16
static uint32_t pattern[PATTERN_LEN] = {
    (PIN_DCLK) | (PIN_D1) | (PIN_D2 << 16) | (PIN_CSYNC),
    (PIN_DCLK << 16) | (PIN_D1) | (PIN_D2 << 16) | (PIN_CSYNC),
    (PIN_DCLK) | (PIN_D1 << 16) | (PIN_D2) | (PIN_CSYNC),
    (PIN_DCLK << 16) | (PIN_D1 << 16) | (PIN_D2) | (PIN_CSYNC),

    (PIN_DCLK) | (PIN_D1) | (PIN_D2) | (PIN_CSYNC),
    (PIN_DCLK << 16) | (PIN_D1) | (PIN_D2) | (PIN_CSYNC),
    (PIN_DCLK) | (PIN_D1 << 16) | (PIN_D2 << 16) | (PIN_CSYNC),
    (PIN_DCLK << 16) | (PIN_D1 << 16) | (PIN_D2 << 16) | (PIN_CSYNC),

    (PIN_DCLK) | (PIN_D1) | (PIN_D2 << 16) | (PIN_CSYNC << 16),
    (PIN_DCLK << 16) | (PIN_D1) | (PIN_D2 << 16) | (PIN_CSYNC << 16),
    (PIN_DCLK) | (PIN_D1 << 16) | (PIN_D2) | (PIN_CSYNC << 16),
    (PIN_DCLK << 16) | (PIN_D1 << 16) | (PIN_D2) | (PIN_CSYNC << 16),

    (PIN_DCLK) | (PIN_D1) | (PIN_D2) | (PIN_CSYNC << 16),
    (PIN_DCLK << 16) | (PIN_D1) | (PIN_D2) | (PIN_CSYNC << 16),
    (PIN_DCLK) | (PIN_D1 << 16) | (PIN_D2 << 16) | (PIN_CSYNC << 16),
    (PIN_DCLK << 16) | (PIN_D1 << 16) | (PIN_D2 << 16) | (PIN_CSYNC << 16),
};

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_up;

void Start_Print_DMA(void)
{
    if (htim1.State != HAL_TIM_STATE_READY) {
        return; // таймер ещё занят
    }

    if (HAL_DMA_Start_IT(&hdma_tim1_up,
                     (uint32_t)pattern,
                     (uint32_t)&GPIOB->BSRR,
                     PATTERN_LEN) != HAL_OK)
    {
        LOG_Error("%s", "DMA Start error");
    }

    __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);
    if (HAL_TIM_Base_Start(&htim1) != HAL_OK)
    {
        LOG_Error("%s", "Timer Start error");
        return;
    }
}

static void PRINTHEAD__InitHeadPin(PRINTHEAD_tstHeadPin* headPin, GPIO_TypeDef* port, uint16_t pin)
{
    headPin->port = port;
    headPin->pin = pin;
    // TODO: Init State to 0?
}

void PRINTHEAD_Init(PRINTHEAD_tstInstance* head, PRINTHEAD_tenHeadType headType)
{
    head->headType = headType;
    
    PRINTHEAD__InitHeadPin(&head->PWRA,     PWRA_GPIO_Port, PWRA_Pin);
    PRINTHEAD__InitHeadPin(&head->PWRB,     PWRB_GPIO_Port, PWRA_Pin);
    PRINTHEAD__InitHeadPin(&head->D1,       D1_GPIO_Port, D1_Pin);
    PRINTHEAD__InitHeadPin(&head->D2,       D2_GPIO_Port, D2_Pin);
    PRINTHEAD__InitHeadPin(&head->D3,       D3_GPIO_Port, D3_Pin);
    PRINTHEAD__InitHeadPin(&head->DCLK,     DCLK_GPIO_Port, DCLK_Pin);
    PRINTHEAD__InitHeadPin(&head->CSYNC,    CSYNC_GPIO_Port, CSYNC_Pin);
    PRINTHEAD__InitHeadPin(&head->S1,       S1_GPIO_Port, S1_Pin);
    PRINTHEAD__InitHeadPin(&head->S2,       S2_GPIO_Port, S2_Pin);
    PRINTHEAD__InitHeadPin(&head->S3,       S3_GPIO_Port, S3_Pin);
    PRINTHEAD__InitHeadPin(&head->S4,       S4_GPIO_Port, S4_Pin);
    PRINTHEAD__InitHeadPin(&head->S5,       S5_GPIO_Port, S5_Pin);
    
    PRINTHEAD__InitHeadPin(&head->ID,       NULL, GPIO_PIN_0);
    PRINTHEAD__InitHeadPin(&head->TS,       NULL, GPIO_PIN_0);
}

void PRINTHEAD_Deinit()
{
    // TODO
}

void PRINTHEAD_SetColor(PRINTHEAD_tstInstance* head, uint8_t color) // TODO: is it need to take nozzle column/row
{
    
}

void PRINTHEAD_Process(PRINTHEAD_tstInstance* head)
{
    Start_Print_DMA();
}
