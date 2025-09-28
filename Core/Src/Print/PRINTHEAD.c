#include "Print/PRINTHEAD.h"
#include "LOG/LOG.h"
#include <string.h>

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

#define PIN_SET(pin) ((uint32_t)(pin))
#define PIN_RESET(pin) ((uint32_t)((pin) << 16))
#define MakePatternPin(pin, value) (value ? PIN_SET(pin) : PIN_RESET(pin))
#define MakePatternLine(PWRA, PWRB, DCLK, CSYNC, D1, D2, D3, S1, S2, S3, S4, S5) \
    MakePatternPin(PIN_PWRA, PWRA)  | MakePatternPin(PIN_PWRB, PWRB)    | \
    MakePatternPin(PIN_DCLK, DCLK)  | MakePatternPin(PIN_CSYNC, CSYNC)  | \
    MakePatternPin(PIN_D1, D1)      | MakePatternPin(PIN_D2, D2)        | MakePatternPin(PIN_D3, D3) | \
    MakePatternPin(PIN_S1, S1)      | MakePatternPin(PIN_S2, S2)        | MakePatternPin(PIN_S3, S3) | MakePatternPin(PIN_S4, S4) | MakePatternPin(PIN_S5, S5)

#define PATTERN_LEN 16
#define PRINTHEAD__PatternsNumber 4

static uint32_t PRINTHEAD__testPattern[PATTERN_LEN] = {
    //              PWRA,   PWRB,   DCLK,   CSYNC,  D1,      D2,     D3,     S1,     S2,     S3,     S4,     S5
    MakePatternLine(0,      0,      1,      1,      1,       1,      1,      1,      0,      0,      0,      0),
    MakePatternLine(0,      0,      0,      1,      1,       1,      1,      1,      0,      0,      0,      0),
    MakePatternLine(0,      0,      1,      0,      1,       1,      1,      0,      1,      0,      0,      0),
    MakePatternLine(0,      0,      0,      0,      1,       1,      1,      0,      1,      0,      0,      0),
    MakePatternLine(0,      0,      1,      0,      1,       1,      1,      0,      0,      1,      0,      0),
    MakePatternLine(0,      0,      0,      0,      1,       1,      1,      0,      0,      1,      0,      0),
    MakePatternLine(0,      0,      1,      0,      1,       1,      1,      0,      0,      0,      1,      0),
    MakePatternLine(0,      0,      0,      0,      1,       1,      1,      0,      0,      0,      1,      0),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      1),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      1),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      0),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      0),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      0),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      0),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      0),
    MakePatternLine(0,      0,      0,      0,      0,       0,      0,      0,      0,      0,      0,      0)
};

static uint32_t PRINTHEAD__emptyPattern[PATTERN_LEN] = {0};
/*
    Each line durations is equal timer duration.
    Timer duration is based on the fastes signal - DCLK.
    DCLK perion = 400 ns -> Timer period = 200 ns
*/ 
static uint32_t pattern[PATTERN_LEN] = {0};
static uint32_t* PRINTHEAD__patterns[PRINTHEAD__PatternsNumber] = {
    PRINTHEAD__testPattern,
    PRINTHEAD__testPattern,
    PRINTHEAD__testPattern,
    PRINTHEAD__emptyPattern
};
static uint8_t PRINTHEAD__currentPatternNumber = 0;

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_up;

static bool PRINTHEAD__isStarted;

void Start_Print_DMA(void)
{
    if (htim1.State != HAL_TIM_STATE_READY) {
        return; // таймер ещё занят
    }

    if (HAL_DMA_Start_IT(&hdma_tim1_up,
                     (uint32_t)PRINTHEAD__patterns[PRINTHEAD__currentPatternNumber],
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

static void PRINTHEAD__TimerDMACallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == &hdma_tim1_up) {
        HAL_TIM_Base_Stop(&htim1);
        __HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE);
        // Сбросить состояние DMA
        HAL_DMA_Abort(hdma);

        // Теперь можно подготовить новый буфер и снова вызвать Start_Print_DMA()
        if (PRINTHEAD__isStarted) Start_Print_DMA();
    }
}

static void PRINTHEAD__HalfTimerDMACallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == &hdma_tim1_up) {
        PRINTHEAD__currentPatternNumber++;

        if (PRINTHEAD__currentPatternNumber >= PRINTHEAD__PatternsNumber)
        {
            PRINTHEAD__currentPatternNumber = 0;
        }
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

    // TODO 
    
    memcpy(pattern, PRINTHEAD__patterns[0], PATTERN_LEN * sizeof(uint32_t));

    HAL_DMA_RegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_CPLT_CB_ID, PRINTHEAD__TimerDMACallback);
    HAL_DMA_RegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_HALFCPLT_CB_ID, PRINTHEAD__HalfTimerDMACallback);
    PRINTHEAD__isStarted = false;
}

void PRINTHEAD_Deinit()
{
    PRINTHEAD__isStarted = false;
}

void PRINTHEAD_SetColor(PRINTHEAD_tstInstance* head, uint8_t color) // TODO: is it need to take nozzle column/row
{
    
}

void PRINTHEAD_Process(PRINTHEAD_tstInstance* head)
{
    
}

void PRINTHEAD_Start()
{
    PRINTHEAD__isStarted = true;
    Start_Print_DMA();
}

void PRINTHEAD_Stop()
{
    PRINTHEAD__isStarted = false;
}

bool PRINTHEAD_IsStarted()
{
    return PRINTHEAD__isStarted;
}
