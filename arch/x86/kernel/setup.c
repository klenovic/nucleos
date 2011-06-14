#include <nucleos/types.h>
#include <nucleos/compiler.h>
#include <nucleos/init.h>
#include <nucleos/cache.h>
#include <asm/processor.h>

#ifdef CONFIG_X86_32
/* cpu data as detected by the assembly code in head.S */
struct cpuinfo_x86 new_cpu_data  = {0, 0, 0, 0, -1, 1, 0, 0, -1};
/* common cpu data for all cpus */
struct cpuinfo_x86 boot_cpu_data = {0, 0, 0, 0, -1, 1, 0, 0, -1};
#endif /* CONFIG_X86_32 */
