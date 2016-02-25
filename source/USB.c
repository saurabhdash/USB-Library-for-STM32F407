#include "USB.h"
#include <string.h>

/* HOW TO USE THIS LIBRARY
DO NOT FORGET TO ADD THE .h AND THE .c FILE IN THE CMSIS_BOOT FOLDER TO THE PROJECT ELSE THE USB WILL NOT WORK AS THE PLL CHOSEN ARE DIFFERENT
In main call SystemInit();
then call init();
the function to send string is sendstring()
the function to send a float is sendfloat()
if you observe all the four LEDs flashing and making a ring, some error has occured, all the cases of error are listed below

*/
void sendfloat(float n)
{

	    int flag=0;
	    if(n<0)
	    {
	        flag=1;
	        n=-n;
	    }
	    float temp=n;
	    int tempi;
	    int i=0; //length before the decimal
	    while((int)temp >0)
	    {
	        i++;
	        temp/=10;
	    }
	    //printf("%d\n",i);
	    tempi=i;
	    char res[i+7];
	    temp=n;
	    while(tempi>=0)
	    {
	    res[tempi]= (int)temp%10 +'0';
	    tempi--;
	    temp/=10;
	    }
	    res[i+1]='.';
	    res[i+6]='\0';
	    int j;
	    temp=n;
	    for(j=0;j<4;j++)
	    {
	        res[i+2+j]=(int)(temp*10)%10+'0';
	         temp*=10;
	    }
	    if(flag==1)
	        res[0]='-';
	    else
	        res[0]='+';
	    /*for(tempi=0;tempi<i+7;tempi++)
	    {
	    	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
	    	USART_SendData(USART1, res[tempi]);
	    }*/
	    VCP_send_str(res);


}

/*void sendstring(char* s)
{
    int l=strlen(s);
    int i;
    //printf("%d",l);
    for(i=0;i<strlen(s);i++)
    {
    	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
        USART_SendData(USART1, s[i]);
    }
    //printf("%s",s);
}*/

    void sendstring(char* s)
    {
    	VCP_send_str(s);
    }

void init()
{
	/* STM32F4 discovery LEDs */
	GPIO_InitTypeDef LED_Config;

	/* Always remember to turn on the peripheral clock...  If not, you may be up till 3am debugging... */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	LED_Config.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	LED_Config.GPIO_Mode = GPIO_Mode_OUT;
	LED_Config.GPIO_OType = GPIO_OType_PP;
	LED_Config.GPIO_Speed = GPIO_Speed_25MHz;
	LED_Config.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &LED_Config);



	/* Setup SysTick or CROD! */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		ColorfulRingOfDeath();
	}


	/* Setup USB */
	USBD_Init(&USB_OTG_dev,
	            USB_OTG_FS_CORE_ID,
	            &USR_desc,
	            &USBD_CDC_cb,
	            &USR_cb);

	return;
}

/*
 * Call this to indicate a failure.  Blinks the STM32F4 discovery LEDs
 * in sequence.  At 168Mhz, the blinking will be very fast - about 5 Hz.
 * Keep that in mind when debugging, knowing the clock speed might help
 * with debugging.
 */
void ColorfulRingOfDeath(void)
{
	uint16_t ring = 1;
	while (1)
	{
		uint32_t count = 0;
		while (count++ < 500000);

		GPIOD->BSRRH = (ring << 12);
		ring = ring << 1;
		if (ring >= 1<<4)
		{
			ring = 1;
		}
		GPIOD->BSRRL = (ring << 12);
	}
}

/*
 * Interrupt Handlers
 */

void SysTick_Handler(void)
{
	ticker++;
	if (downTicker > 0)
	{
		downTicker--;
	}
}

void NMI_Handler(void)       {}
void HardFault_Handler(void) { ColorfulRingOfDeath(); }
void MemManage_Handler(void) { ColorfulRingOfDeath(); }
void BusFault_Handler(void)  { ColorfulRingOfDeath(); }
void UsageFault_Handler(void){ ColorfulRingOfDeath(); }
void SVC_Handler(void)       {}
void DebugMon_Handler(void)  {}
void PendSV_Handler(void)    {}

void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}
