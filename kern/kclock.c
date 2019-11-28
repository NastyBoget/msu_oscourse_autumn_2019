/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <kern/kclock.h>
#include <inc/time.h>

static int
get_timestamp(void)
{
    struct tm time;

	time.tm_sec = BCD2BIN(mc146818_read(RTC_SEC));
	time.tm_min = BCD2BIN(mc146818_read(RTC_MIN));
	time.tm_hour = BCD2BIN(mc146818_read(RTC_HOUR));
	time.tm_mday = BCD2BIN(mc146818_read(RTC_DAY));
	time.tm_mon = BCD2BIN(mc146818_read(RTC_MON));
	time.tm_year = BCD2BIN(mc146818_read(RTC_YEAR));

	return timestamp(&time);
}

int gettime(void)
{
	int t;
	nmi_disable();
	// LAB 12: your code here
	while (mc146818_read(RTC_AREG) & RTC_UPDATE_IN_PROGRESS) {}

    if ((t = get_timestamp()) != get_timestamp()) {
        t = get_timestamp();
    }
	nmi_enable();
	return t;
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

