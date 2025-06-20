#include "stm32f4xx.h"

#define DEBOUNCE 25000
#define PRESS_N_HOLD 200000

int time=0, chegou=0, receive_input = 1;
char tecla;

void setup();
//timer interuptions
void TIM1_UP_TIM10_IRQHandler(void);
void TIM1_TRG_COM_TIM11_IRQHandler(void);
void TIM8_UP_TIM13_IRQHandler(void);
//serial comunication
void manda_serial(int a);
int _write(int file, char *ptr, int len);
void USART2_IRQHandler(void);
//display functions
uint32_t convert_to_display(int num);
int get_digit(int num, uint32_t display);

//////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    int button[] = {0, 0};

    setup();

    while(1){
        if (chegou) {
            chegou = 0;
            TIM13->DIER &= ~0x01;   //disable timer 13 interuption - stop requesting time

            //get tens
            printf("\r\nTimer: %c", tecla);
            time += 10*(tecla-'0');
            while(chegou==0);   //wait for the user to type
            
            //get units
            printf("%cs\r\n", tecla);
            time += (tecla-'0');
            chegou=0;

            receive_input = 0;  //disable user input
            printf("Pressione o gatilho para comecar\n\rTimer: %i", time);

            //wait start button
            while(1){
                if(GPIOA->IDR & 2){
                    button[0]++;
                    //if button 0 has been pressed for a while
                    if(button[0]>DEBOUNCE){
                        TIM10->DIER |= 0x01;    //enable timer 10 interuption - start countdown
                        
                        button[0] = 0;
                        break;
                    }
                }
            }

            while(1){
                if(GPIOA->IDR & 1){
                    button[1]++;
                    //if button 1 has been pressed for a while
                    if(button[1]>DEBOUNCE){
                        printf("\r\nA contagem de tempo foi interrompida pelo usuario\n\n\r");

                        button[1] = 0;
                        break;
                    }
                }

                else if(time==0){
                    printf("\r\nContagem de tempo finalizada!\r\n\r\n");
                    break;
                }
            }

            TIM10->DIER &= ~0x01;   //disable timer 10 interuption - stop countdown
            TIM13->DIER |= 0x01;    //enable timer 13 interuption - restart requesting time
            receive_input = 1;      //enable user input
        }
        time = 0;
    }
}
//////////////////////////////////////////////////////////////////////////////////

void setup(){
    RCC->AHB1ENR = 0x87;
    GPIOA->MODER = 0x28000400;

    //PA0 e PA1 = inputs
    GPIOA->MODER &= ~0xF;
    GPIOA->PUPDR |= 0xA;
    //PB0 e PB1 = outputs
    GPIOB->MODER = 0x5;
    GPIOB->ODR = 0b11;
    //PC0 a PC6 = outputs
    GPIOC->MODER = 0x1555;

    //set timer 10 to 1Hz
    RCC->APB2ENR |= (1<<17);    //clock timer 10 enable
    TIM10->PSC = 999;
    TIM10->ARR = 15999;
    TIM10->CR1 |= 0x01;  //enable timer 10 couting

    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 1);    //Priority 1
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    //set timer 11 to 100Hz
    RCC->APB2ENR |= (1<<18);    //clock timer 11 enable
    TIM11->PSC = 999;
    TIM11->ARR = 159;
    TIM11->CR1 |= 0x01;     //enable timer 11 couting
    TIM11->DIER |= 0x01;    //enable timer 11 interuption

    NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 3);   //Priority 3
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

    //set timer 13 to 1/3Hz
    RCC->APB1ENR |= 0x80;	//clock timer 13 enable
    TIM13->PSC = 1999;
    TIM13->ARR = 23999;
    TIM13->CR1 |= 0x01; 	//enable timer 13 couting
    TIM13->DIER |= 0x01;    //enable timer 13 interuption

    NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 2);   //Priority 2
    NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);

    //configurando a porta serial assincrona
    RCC->APB1ENR|=0x20000;      //habilita clock usart 2

    GPIOA->MODER|=0xA0;         //Configurando Pino A2 e A3 como função alternativa
    GPIOA->AFR[0]|=0x00007700;  //ativando a função alternativa usart2 nos pinos PA2 e PA3

    USART2->CR1|=0x202C;        //0x200C; ativando interrupção recepção
    USART2->CR2=0;
    USART2->CR3=0;
    USART2->BRR|=(52<<4);       //16MHz/(16*19200)   definir baudrate = 19200

    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
}

//verifica se o buffer ta vazio e manda o valor pelo serial
void manda_serial(int a){
    while(!((USART2->SR)&0x80)); //aguarda  o buffer liberar
    USART2->DR = a;
}

//usa a funcao printf pra enviar uma string por serial
int _write(int file, char *ptr, int len) {
    int i=0;
    for(i=0 ; i<len ; i++){
        manda_serial((*ptr++));
    }
    return len;
}

//interrupcao do USART2 (recebe valores)
void USART2_IRQHandler(void) {
    if (USART2->SR&0x20){
        if (USART2->DR != 27 && receive_input) {
            chegou = 1;
            tecla = USART2->DR;
        }
    }
    USART2->SR&=~(0x20);    //limpa flag da interrupção
}

void TIM1_UP_TIM10_IRQHandler(void){
    time--;
    printf("\r              ");
    printf("\rTimer: %i", time);

    TIM10->SR &= ~0x01; //clear flag
    TIM10->CNT = 0;     //reset counter
}

void TIM1_TRG_COM_TIM11_IRQHandler(void){
    //skip to the next display
    if((GPIOB->IDR&0x3) == 0b01)    GPIOB->ODR = 0b10;
    else                            GPIOB->ODR = 0b01;

    //print number    
    GPIOC->ODR = convert_to_display(get_digit(time, GPIOB->ODR & 0x3));

    TIM11->SR &= ~0x01; //clear flag
    TIM11->CNT = 0;     //reset counter
}

void TIM8_UP_TIM13_IRQHandler(void){
	printf("Estou aguardando o tempo, Edrick\r\n");

	TIM13->SR &= ~0x01; //clear flag
    TIM13->CNT = 0;     //reset counter
}

int get_digit(int num, uint32_t display){
    switch (display)
    {
    case (0b10):
        return (num%10)/1;

    case (0b01):
        return (num - num%10)/10;

    default:
        return -1;
    }
}

uint32_t convert_to_display(int num){
    switch (num)
    {
    case 0:
        return 0b1111110;

    case 1:
        return 0b0110000;

    case 2:
        return 0b1101101;

    case 3:
        return 0b1111001;

    case 4:
        return 0b0110011;

    case 5:
        return 0b1011011;

    case 6:
        return 0b1011111;

    case 7:
        return 0b1110000;

    case 8:
        return 0b1111111;

    case 9:
        return 0b1111011;

    default:
        return 0b0000001;
    }
}
