/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <kern/kclock.h>

int gettime(void)
{
	nmi_disable();
	// LAB 12: your code here

	nmi_enable();
	return 0;
}

void
rtc_init(void)
{
	nmi_disable();
	// LAB 4: your code here
	uint8_t A, B;
	outb(IO_RTC_CMND, RTC_BREG); //Переключение на регистр часов B.
	B = inb(IO_RTC_DATA); //Чтение значения регистра B из порта ввода-вывода.
	B |= RTC_PIE; // Установка бита RTC_PIE - periodic interrupt enable.
	outb(IO_RTC_DATA, B); //Запись обновленного значения регистра в порт ввода-вывода.
	outb(IO_RTC_CMND, RTC_AREG); //Переключение на регистр часов A.
	A = inb(IO_RTC_DATA); //Чтение значения регистра A из порта ввода-вывода.
	A |= 0xF; // doc/rtc.pdf table 4 (page 14) -> RS3-RS0 = 1 FREQUENCY = 2Hz
	outb(IO_RTC_DATA, A); //Запись обновленного значения регистра в порт ввода-вывода.
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

unsigned
mc146818_read(unsigned reg)
{
	outb(IO_RTC_CMND, reg);
	return inb(IO_RTC_DATA);
}

void
mc146818_write(unsigned reg, unsigned datum)
{
	outb(IO_RTC_CMND, reg);
	outb(IO_RTC_DATA, datum);
}

