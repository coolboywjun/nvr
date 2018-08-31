#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <parted/parted.h>



int delete_all_partition(const char *dev_path)
{
	PedDevice *pDevice = NULL;
	PedDisk	*pDisk = NULL;
	PedDiskType *pDiskType = NULL;
	PedPartition *pPartition = NULL;

	pDevice = ped_device_get(dev_path);
	if(pDevice == NULL)
	{
		printf("Failed to get device.\n");
		return -1;
	}
	if(!ped_device_open(pDevice))
	{
		printf("Failed to open device.\n");
		ped_device_destroy(pDevice);
		return -1;
	}
	pDisk = ped_disk_new(pDevice);
	if(pDisk == NULL)
	{
		printf("Failed to new disk.\n");
		ped_device_close(pDevice);
		ped_device_destroy(pDevice);
		return -1;
	}
	ped_disk_print(pDisk);
	printf("primary number: %d\n", ped_disk_get_primary_partition_count(pDisk));
	printf("partion number: %d\n", ped_disk_get_last_partition_num(pDisk));
	printf("Delete all partion in disk.\n");
	if(!ped_disk_delete_all(pDisk))
	{
		printf("Failed to delete disk.\n");
		ped_disk_destroy(pDisk);
		ped_device_close(pDevice);
		ped_device_destroy(pDevice);
		return -1;
	}
	ped_disk_commit_to_os(pDisk);
	ped_disk_commit_to_dev(pDisk);
	ped_disk_destroy(pDisk);
	ped_device_sync (pDevice);
	ped_device_close(pDevice);
	ped_device_destroy(pDevice);

	
	return 0;
}

int add_primary_partition(const char *dev_path)
{
	PedDevice *pDevice = NULL;
	PedDisk	*pDisk = NULL;
	PedDiskType *pDiskType = NULL;
	PedPartition *pPartition = NULL;
	PedPartitionType part_type = PED_PARTITION_NORMAL;
	const PedFileSystemType* fs_type = ped_file_system_type_get ("ext2");
	PedConstraint *pConstraint;  
	PedGeometry *pGeometry;
	
	pDevice = ped_device_get(dev_path);
	if(pDevice == NULL)
	{
		printf("Failed to get device.\n");
		return -1;
	}
	if(!ped_device_open(pDevice))
	{
		printf("Failed to open device.\n");
		ped_device_destroy(pDevice);
		return -1;
	}
	pConstraint = ped_device_get_constraint(pDevice);
	if(pConstraint == NULL)
	{
		printf("Failed to get constraint.\n");
		ped_device_close(pDevice);
		ped_device_destroy(pDevice);
		return -1;
	}
	pDisk = ped_disk_new(pDevice);
	if(pDisk == NULL)
	{
		printf("Failed to new disk.\n");
		ped_constraint_destroy(pConstraint);
		ped_device_close(pDevice);
		ped_device_destroy(pDevice);
		return -1;
	}
	pPartition = ped_partition_new (pDisk, part_type, fs_type, 1, 2);
	if(pPartition == NULL)
	{
		printf("Failed to new partition.\n");
		ped_disk_destroy(pDisk);
		ped_constraint_destroy(pConstraint);
		ped_device_close(pDevice);
		ped_device_destroy(pDevice);
		return -1;
	}
	
	if(!ped_disk_add_partition(pDisk, pPartition, pConstraint))
	{
		printf("111111111111111\n");
	}
	
	if(!ped_disk_maximize_partition(pDisk, pPartition, pConstraint))
	{
		printf("Failed to grow partition.\n");
		ped_disk_destroy(pDisk);
		ped_constraint_destroy(pConstraint);
		ped_device_close(pDevice);
		ped_device_destroy(pDevice);
		return -1;
	}
	
	ped_disk_commit_to_os(pDisk);
	ped_disk_commit_to_dev(pDisk);
	ped_disk_destroy(pDisk);
	ped_constraint_destroy(pConstraint);
	ped_device_close(pDevice);
	ped_device_destroy(pDevice);
	
	return 0;	
}

int main(int argc, char *argv[])
{
	int ret;
	if(argc < 3)
	{
		printf("usage: %s dev\n", argv[0]);
		exit(1);
	}
	if(strcmp(argv[2], "add") == 0)
		ret = add_primary_partition(argv[1]);
	else if(strcmp(argv[2], "del") == 0)
		ret = delete_all_partition(argv[1]);
	else
	{
		printf("the %s command don't support!\n", argv[2]);
		exit(1);
	}	
	printf("ret = %d\n", ret);
	return 0;
}


