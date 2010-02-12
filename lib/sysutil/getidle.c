/* getidle.c - by David van Moolenbroek <dcvmoole@cs.vu.nl> */
/* Ladislav Klenovic - adaption for Nucleos */
/* Usage:
 *
 * u64_t idlestart, idlestop;
 * u64_t timestart, timestop;
 * timestart = getidle(&idlestart);
 * ...
 * timestop = getidle(&idlestop);
 * printf("Measured time interval: %d\n", timestop - timestart);
 * printf("CPU usage (idle): %d\n", idlestop - idlestart);
 *
 * This routine goes through PM to get the idle time, rather than making the
 * sys_getinfo() call to the kernel directly. This means that it can be used
 * by non-system processes as well, but it will incur some extra overhead in
 * the system case. The overhead does not end up being measured, because the
 * system is clearly not idle while the system calls are being made. In any
 * case, for this reason, only one getidle() run is allowed at a time.
 *
 * Note that the kernel has to be compiled with CONFIG_IDLE_TSC support.
 */


#include <nucleos/sysinfo.h>
#include <nucleos/u64.h>
#include <nucleos/sysutil.h>

static u64_t start, idle;
static int running = 0;

u64_t getidle(u64_t *idle_time)
{
	u64_t stop, idle2;
	int r;

	if (!running) {
		r = getsysinfo_up(PM_PROC_NR, SIU_IDLETSC, sizeof(idle), &idle);
		if (r != sizeof(idle))
			return 0;

		running = 1;

		read_tsc_64(&start);

		if (idle_time)
			*idle_time = idle;

		return start;
	} else {
		read_tsc_64(&stop);

		running = 0;

		r = getsysinfo_up(PM_PROC_NR, SIU_IDLETSC, sizeof(idle2), &idle2);
		if (r != sizeof(idle2))
			return 0;

		if (idle_time)
			*idle_time = idle2;

		return stop;
	}

	running = !running;
}
