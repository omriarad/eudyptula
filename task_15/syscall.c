#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(void)
{
	syscall(380, 0x000091bc, 0x4039c624);
	syscall(380, 0x0, 0x0);

	return 0;
}
