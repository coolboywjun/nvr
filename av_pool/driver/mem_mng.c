#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/io.h>
//#include <plat/ti81xx_ram.h>

#include "mem_mng.h"


struct __sun7i_reserved_addr {
	unsigned int		paddr;    
	unsigned int		size; 
};
extern void sun7i_get_gpu_addr(struct __sun7i_reserved_addr *gpu_addr);

/* Register the module parameters. */
#if 0
MODULE_PARM_DESC(phys_start, "\n\t\t Start Address for CMEM Pool Memory");
static char *phys_start = NULL;
MODULE_PARM_DESC(phys_end, "\n\t\t End Address for CMEM Pool Memory");
static char *phys_end = NULL;
module_param(phys_start, charp, S_IRUGO);
module_param(phys_end, charp, S_IRUGO);
#endif

MODULE_PARM_DESC(alloc_size, "\n\t\t The size for alloc CMEM Pool Memory");
static int alloc_size = 0;
module_param(alloc_size, int, 0644);

unsigned long block_start = 0;
unsigned long block_end = 0;
unsigned long block_len = 0;
unsigned long block_virtp = 0;
MemBlock mem_block[AV_POOL_MAX_SUPPORT_CHN][AV_POOL_MAX_SUPPORT_STREAM];

#ifdef __LINUX_ARM_ARCH__

static unsigned long start_phyaddr, start_virtaddr;
static unsigned long mem_len;

int mem_mng_init(void)
{
	int i, j;
	unsigned long ch_block_size;
	unsigned long host_start_phyaddr;
	unsigned long host_start_virtaddr;
	struct __sun7i_reserved_addr gpu_addr;
#if 0	
	if(phys_start == NULL || phys_end == NULL)
	{
		printk(KERN_ERR "The phys_start or phys_end parameter is null, please set it!\n");
		return -EINVAL;
	}
	printk(KERN_INFO "phys_start: %s, phys_end: %s\n", phys_start, phys_end);

	block_start = PAGE_ALIGN(simple_strtol(phys_start, NULL, 16));
	block_end = PAGE_ALIGN(simple_strtol(phys_end, NULL, 16));
	if(block_end < block_start)
	{
		printk(KERN_ERR "The phys_end is bigger than phys_start.\n");
		return -EINVAL;
	}
	if(block_end - block_start < 0x1000000)	// less than 16M
	{
		printk(KERN_ERR "The phys space is too small!\n");
		return -EINVAL;
	}
	block_len = block_end - block_start;
#endif
/*
	if(alloc_size <= 0x1000000)	 // less than 16M
	{
		printk(KERN_ERR "The phys space is too small!\n");
		return -EINVAL;
	}
*/
	sun7i_get_gpu_addr(&gpu_addr);
	printk(KERN_ERR "gpu addr 0x%08x, size 0x%x\n", gpu_addr.paddr, gpu_addr.size);
	if(gpu_addr.size < 0x2000000)
	{
		printk(KERN_ERR "The gpu space is too small.\n");
		return -EINVAL;
	}
	start_phyaddr = PAGE_ALIGN(gpu_addr.paddr + (gpu_addr.size - 0x2000000) / 2);
	mem_len = 0x2000000;
	
	if (!request_mem_region(start_phyaddr, mem_len, "avpool")) {
		printk(KERN_ERR "Failed to request_mem_region(%#lx, %ld)\n", start_phyaddr, mem_len);
		return -EINVAL;
	}
	start_virtaddr = (unsigned long) ioremap_nocache(start_phyaddr, mem_len);
	if(start_virtaddr == 0)
	{
		release_mem_region(start_phyaddr, mem_len);
		printk(KERN_ERR "Failed to ioremap for avpool.\n");
		return -EINVAL;
	}
	
	ch_block_size = 0x600000UL; //mem_len / (AV_POOL_MAX_SUPPORT_CHN - 1);
	printk(KERN_INFO "av pool alloc size is 0x%lx\n", mem_len);
	printk(KERN_INFO "every channal size is 0x%lx\n", ch_block_size);
	
	for(i = 0; i < AV_POOL_MAX_SUPPORT_CHN - 1; i++)
	{
	    	mem_block[i][0].memBlockLen = 0x400000UL;//PAGE_ALIGN(ch_block_size / 2);
	    	mem_block[i][0].startVirtAddr = start_virtaddr + ch_block_size * i;
	    	mem_block[i][0].startPhysAddr = start_phyaddr + ch_block_size * i;
	    	
	    	mem_block[i][1].memBlockLen = 0x200000UL;//PAGE_ALIGN(ch_block_size / 2);
	    	mem_block[i][1].startVirtAddr = start_virtaddr + ch_block_size * i + mem_block[i][0].memBlockLen;
	    	mem_block[i][1].startPhysAddr = start_phyaddr + ch_block_size * i + mem_block[i][0].memBlockLen;
	    	
	    	//mem_block[i][2].memBlockLen = PAGE_ALIGN(ch_block_size / 8);
	    	//mem_block[i][2].startVirtAddr = (unsigned long)kmalloc(mem_block[i][1].memBlockLen, GFP_KERNEL);
	    	//mem_block[i][2].startPhysAddr = virt_to_phys((void *)mem_block[i][1].startVirtAddr);
		for(j = 0; j < AV_POOL_MAX_SUPPORT_STREAM; j++)
	    	{
			printk(KERN_INFO "memory block %d.%d ==> startPhysAddress: 0x%08lx, startVirtAddress: 0x%08lx, len: 0x%lx\n",
						i, j, mem_block[i][j].startPhysAddr, mem_block[i][j].startVirtAddr, mem_block[i][j].memBlockLen);
		}
	}
	host_start_virtaddr = start_virtaddr + ch_block_size * (AV_POOL_MAX_SUPPORT_CHN-1);
	host_start_phyaddr = start_phyaddr + ch_block_size * (AV_POOL_MAX_SUPPORT_CHN-1);
	for(i = 0; i < AV_POOL_MAX_SUPPORT_STREAM; i++)
	{
		mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].memBlockLen = 0x100000UL;
	    	mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].startVirtAddr = host_start_virtaddr + i * 0x100000UL;
	    	mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].startPhysAddr = host_start_phyaddr + i * 0x100000UL;
    	}
    	

	return 0;
}

void mem_mng_exit(void)
{
	iounmap((void *)start_virtaddr);
	release_mem_region(start_phyaddr, mem_len);
}


#else

int mem_mng_init(void)
{
	int i, j;
	unsigned long ch_block_size;
	
#if 0	
	if(phys_start == NULL || phys_end == NULL)
	{
		printk(KERN_ERR "The phys_start or phys_end parameter is null, please set it!\n");
		return -EINVAL;
	}
	printk(KERN_INFO "phys_start: %s, phys_end: %s\n", phys_start, phys_end);

	block_start = PAGE_ALIGN(simple_strtol(phys_start, NULL, 16));
	block_end = PAGE_ALIGN(simple_strtol(phys_end, NULL, 16));
	if(block_end < block_start)
	{
		printk(KERN_ERR "The phys_end is bigger than phys_start.\n");
		return -EINVAL;
	}
	if(block_end - block_start < 0x1000000)	// less than 16M
	{
		printk(KERN_ERR "The phys space is too small!\n");
		return -EINVAL;
	}
	block_len = block_end - block_start;
#endif
	if(alloc_size <= 0x1000000)	 // less than 16M
	{
		printk(KERN_ERR "The phys space is too small!\n");
		return -EINVAL;
	}
	
	ch_block_size = alloc_size / (AV_POOL_MAX_SUPPORT_CHN - 1);
	printk(KERN_INFO "av pool alloc size is 0x%x\n", alloc_size);
	printk(KERN_INFO "every channal size is 0x%lx\n", ch_block_size);
	
	for(i = 0; i < AV_POOL_MAX_SUPPORT_CHN - 1; i++)
	{
	    	mem_block[i][0].memBlockLen = PAGE_ALIGN(ch_block_size / 2);
	    	mem_block[i][0].startVirtAddr = (unsigned long)kmalloc(mem_block[i][0].memBlockLen, GFP_KERNEL);
	    	mem_block[i][0].startPhysAddr = virt_to_phys((void *)mem_block[i][0].startVirtAddr);
	    	
	    	mem_block[i][1].memBlockLen = PAGE_ALIGN(ch_block_size / 2);
	    	mem_block[i][1].startVirtAddr = (unsigned long)kmalloc(mem_block[i][1].memBlockLen, GFP_KERNEL);
	    	mem_block[i][1].startPhysAddr = virt_to_phys((void *)mem_block[i][1].startVirtAddr);
	    	
	    	//mem_block[i][2].memBlockLen = PAGE_ALIGN(ch_block_size / 8);
	    	//mem_block[i][2].startVirtAddr = (unsigned long)kmalloc(mem_block[i][1].memBlockLen, GFP_KERNEL);
	    	//mem_block[i][2].startPhysAddr = virt_to_phys((void *)mem_block[i][1].startVirtAddr);

	    	for(j = 0; j < AV_POOL_MAX_SUPPORT_STREAM; j++)
	    	{
	    		if(mem_block[i][j].startVirtAddr == 0)
	    		{
	    			int m, n;
	    			printk(KERN_ERR "Alloc memory for av pool failed.\n");
	    			for(m = 0; m<=i; m++)
	    			{
	    				for(n = 0; n<AV_POOL_MAX_SUPPORT_STREAM; n++)
	    				{
						if(mem_block[m][n].startVirtAddr)
						{
							kfree((void *)mem_block[m][n].startVirtAddr);
							printk(KERN_INFO "free memory block %d.%d ==> startPhysAddress: 0x%08lx, startVirtAddress: 0x%08lx, len: 0x%lx\n",
									m, n, mem_block[m][n].startPhysAddr, mem_block[m][n].startVirtAddr, mem_block[m][n].memBlockLen);
						}
	    				}
	    			}
	    			return -ENOMEM;
	    		}
	    		else
				printk(KERN_INFO "memory block %d.%d ==> startPhysAddress: 0x%08lx, startVirtAddress: 0x%08lx, len: 0x%lx\n",
						i, j, mem_block[i][j].startPhysAddr, mem_block[i][j].startVirtAddr, mem_block[i][j].memBlockLen);
		}
	}
	for(i = 0; i < AV_POOL_MAX_SUPPORT_STREAM; i++)
	{
		mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].memBlockLen = 0x100000UL;
	    	mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].startVirtAddr = (unsigned long)kmalloc(mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].memBlockLen, GFP_KERNEL);
	    	mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].startPhysAddr = virt_to_phys((void *)mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].startVirtAddr);
		if(mem_block[AV_POOL_MAX_SUPPORT_CHN-1][i].startVirtAddr == 0)
		{
			printk(KERN_INFO "Alloc for host channel failed.\n");
			return -ENOMEM;
		}
	}
	return 0;
}

void mem_mng_exit(void)
{
	int i, j;
	for(i = 0; i < AV_POOL_MAX_SUPPORT_CHN; i++)
	{
		for(j = 0; j < AV_POOL_MAX_SUPPORT_STREAM; j++)
		{
			if(mem_block[i][j].startVirtAddr)
				kfree((void *)mem_block[i][j].startVirtAddr);
		}
	}
}

#endif


