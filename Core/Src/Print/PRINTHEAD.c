#include "Print/PRINTHEAD.h"
#include "LOG/LOG.h"
#include <string.h>
#include <Print/printspider.h>
#include <stdlib.h>

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
#define PRINTHEAD__PatternsNumber 600

// static uint32_t PRINTHEAD__emptyPattern[PATTERN_LEN] = {0};
/*
    Each line durations is equal timer duration.
    Timer duration is based on the fastes signal - DCLK.
    DCLK perion = 400 ns -> Timer period = 200 ns
*/ 
// static uint32_t pattern[PATTERN_LEN] = {0};
static uint32_t PRINTHEAD__patterns[PRINTHEAD__PatternsNumber] = {0};

extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_up;

static bool PRINTHEAD__isStarted;

static printspider_waveform_desc_t selected_waveform;

static void select_waveform() {
    // TODO
    #ifdef PRINT_COLOR
        selected_waveform = printspider_get_waveform(PRINTSPIDER_WAVEFORM_COLOR_B);
    #else
        selected_waveform = printspider_get_waveform(PRINTSPIDER_WAVEFORM_BLACK_B);
    #endif
    }

void PRINTHEAD__PrintPattern()
{
    if (htim1.State != HAL_TIM_STATE_READY) {
        return; // таймер ещё занят
    }

    if (HAL_DMA_Start_IT(&hdma_tim1_up,
                     (uint32_t)PRINTHEAD__patterns,
                     (uint32_t)&GPIOB->BSRR,
                     PRINTHEAD__PatternsNumber) != HAL_OK)
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
        __HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE);
        HAL_TIM_Base_Stop(&htim1);
        if (!PRINTHEAD__isStarted)
        {
            HAL_DMA_Abort(&hdma_tim1_up);
        }
    }
}

static void PRINTHEAD__HalfTimerDMACallback(DMA_HandleTypeDef *hdma)
{
    if (hdma == &hdma_tim1_up) {
        if (!PRINTHEAD__isStarted)
        {
            /* 
                TODO: нужна нормальная нормальная обработка остановки печати:
                    - безопасность для картриджа (сброс PWRA/PWRB)
                    - обнуление группы сопел
            */
            memset(PRINTHEAD__patterns, 0, sizeof(PRINTHEAD__patterns));
        }
    }
}

static void PRINTHEAD__InitHeadPin(PRINTHEAD_tstHeadPin* headPin, GPIO_TypeDef* port, uint16_t pin)
{
    headPin->port = port;
    headPin->pin = pin;
    HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
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
    select_waveform();

    HAL_DMA_RegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_CPLT_CB_ID, PRINTHEAD__TimerDMACallback);
    HAL_DMA_RegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_HALFCPLT_CB_ID, PRINTHEAD__HalfTimerDMACallback);
    PRINTHEAD__isStarted = false;
    memset(PRINTHEAD__patterns, 0, sizeof(PRINTHEAD__patterns));
}

void PRINTHEAD_Deinit()
{
    PRINTHEAD__isStarted = false;
    HAL_DMA_UnRegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_CPLT_CB_ID);
    HAL_DMA_RegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_HALFCPLT_CB_ID, PRINTHEAD__HalfTimerDMACallback);
}

static void PRINTHEAD__preparePins(uint16_t *buffer, int len)
{
    static struct{
        bool F3;
        bool F5;
        bool DCLK;
        bool CSYNC;
        bool D1;
        bool D2;
        bool D3;
        bool S1;
        bool S2;
        bool S3;
        bool S4;
        bool S5;
    } P;

    for (int i = 0; i < len && i < PRINTHEAD__PatternsNumber; i++) {
        uint16_t buffer_i = buffer[i];

        P.F3 = buffer_i >> 10 & 1;
        P.F5 = buffer_i >> 11 & 1;
        P.DCLK = buffer_i >> 8 & 1;
        P.CSYNC = buffer_i >> 3 & 1;
        P.D1 = buffer_i >> 0 & 1;
        P.D2 = buffer_i >> 1 & 1;
        P.D3 = buffer_i >> 2 & 1;
        P.S1 = buffer_i >> 6 & 1;
        P.S2 = buffer_i >> 4 & 1;
        P.S3 = buffer_i >> 9 & 1;
        P.S4 = buffer_i >> 5 & 1;
        P.S5 = buffer_i >> 7 & 1;
        

        uint32_t patternLine = MakePatternLine(
            P.F3,
            P.F5,
            P.DCLK,
            P.CSYNC,
            P.D1, P.D2, P.D3,
            P.S1, P.S2, P.S3, P.S4, P.S5
        );

        PRINTHEAD__patterns[i] = patternLine;
    }
}

#define WAVEFORM_LEN 600
static void PRINTHEAD__prepareDataOut(uint8_t *nozdata)
{
    static uint16_t waveform_buffer[WAVEFORM_LEN];
    memset(waveform_buffer, 0, sizeof(uint16_t) * WAVEFORM_LEN);
    int generated_len = printspider_generate_waveform(waveform_buffer, selected_waveform.data, nozdata, selected_waveform.len);
    
    PRINTHEAD__preparePins(waveform_buffer, generated_len);
}

void PRINTHEAD_SetLineColor(PRINTHEAD_tstInstance* head, PRINTHEAD_tunColor color) // TODO: is it need to take nozzle column/row
{
    uint8_t nozdata[PRINTSPIDER_NOZDATA_SZ];
    memset(nozdata, 0, sizeof(nozdata));
    int offset = 0;
    for (int row = 0; row < 2; row++) {
        for (int nozzleIdx = 0; nozzleIdx < PRINTSPIDER_BLACK_NOZZLES_IN_ROW; nozzleIdx++) {
            // We take anything but white in any color channel of the image to
            // mean we want black there.
            switch (head->headType)
            {
            case PRINTHEAD_typeBlack:
            {
                if (color.CMYK.K != 0xff) {
                    // Random-dither 50%, as firing all nozzles is a bit hard on the
                    // power supply.
                    // if (rand() & 1) {
                        printspider_set_nozzle_black(nozdata, nozzleIdx + offset);
                    // }
                }
                break;
            }
            case PRINTHEAD_typeColor:
            {
                /* code */
                break;
            }
            
            default:
                LOG_Warning("Not Supported head type: %d", head->headType);
                break;
            }
        }
        offset += 168;
    }
    PRINTHEAD__prepareDataOut(nozdata);
}

void PRINTHEAD_Process(PRINTHEAD_tstInstance* head)
{
    
}

void PRINTHEAD_Start()
{
    PRINTHEAD__isStarted = true;
    PRINTHEAD__PrintPattern();
}

void PRINTHEAD_Stop()
{
    PRINTHEAD__isStarted = false;
}

bool PRINTHEAD_IsStarted()
{
    return PRINTHEAD__isStarted;
}
