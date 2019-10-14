/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <kern/kclock.h>

void
rtc_init(void)
{
	nmi_disable();
	// LAB 4: your code here
	uint8_t A, B;
	outb(IO_RTC_CMND, RTC_BREG); //Переключение на регистр часов B.
	B = inb(IO_RTC_DATA); //Чтение значения регистра B из порта ввода-вывода.
	B |= RTC_PIE; // Установка бита RTC_PIE.
	outb(IO_RTC_DATA, B); //Запись обновленного значения регистра в порт ввода-вывода.
	outb(IO_RTC_CMND, RTC_AREG);
	A = inb(IO_RTC_DATA);
	A |= 0xF;
	outb(IO_RTC_DATA, A);
	nmi_enable();
}

uint8_t
rtc_check_status(void)
{
	uint8_t status;
	// LAB 4: your code here
	outb(IO_RTC_CMND, RTC_CREG);//Переключение на регистр часов C.
	status = inb(IO_RTC_DATA);//Чтение значения регистра C из порта ввода-вывода
	return status;
}

