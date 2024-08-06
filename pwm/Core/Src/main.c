/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include "stm32f103xb.h"

void GPIO_init(void);
void delay_ms(int time);
void PWM_init(int freq, float duty);

int main(void){
	int x = 1;
	GPIO_init();
	while(1){
		PWM_init(100, x);
		++x;
		if(x > 100) x = 1;
		delay_ms(100);
	}

}

void delay_ms(int time){
	//Delay em milissegundos (1MHz -> 1us)
	uint32_t ticks = time*1000;
	SysTick->LOAD = ticks;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	SysTick->CTRL = 0;
}

void GPIO_init(void){
	//Configurar A7 como pwm
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Habilitar o clock do gpio A
	GPIOA->CRL &= ~GPIO_CRL_CNF7_0; //Limpa as configurações
	GPIOA->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7; //Habilita função alternativa e saída em 50MHz
}

void PWM_init(int freq, float duty){
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;											// Habilita o clock do Timer3
	TIM3->CCER |= TIM_CCER_CC2E;												// Habilita CCP2
	TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;		// Modo PWM 1, preload enable
	TIM3->CR1 = TIM_CR1_ARPE;													// Auto reload, preload habilita, alinhamento por borda
	TIM3->PSC = 0;
	TIM3->CR1 &= ~TIM_CR1_CEN;

	//frequency = 8 MHz/((ARR+1)*2)
	// ARR = (4M/frequency)-1

	int arr = 4000000/freq;
	TIM3->ARR = arr;

	//duty = (CCRn/(ARR+1))*100
	//CCRn = duty*(ARR+1)/100

	TIM3->CCR2 = duty*(arr+1)/100;
	TIM3->EGR = 1;							// UG = 1
	TIM3->CR1 |= TIM_CR1_CEN;			   // Habilita timer
}
