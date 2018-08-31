
#include <strg_header.h>




glong calc_checksum(gpointer buff, gint len)
{
	gint i;
	gulong checksum = 0;
	gulong *data = (gulong*)buff;
	g_assert(GPOINTER_TO_INT(data) % 4 == 0);
	g_warn_if_fail(len % 4 == 0);
	for(i = 0; i < len / 4; i++)
		checksum += data[i];
	return checksum;
}

void display_time(time_t t, const gchar *prefix)
{
	struct tm stm, *ptm;
	char timestr[NAME_LEN];
	
	ptm = localtime_r(&t, &stm);
	strftime(timestr, sizeof(timestr), "%F %T", ptm);
	if(prefix)
		printf("%s%s\n", prefix, timestr);
	else
		printf("%s\n", timestr);
}


int strg_write(int fd, const void *data, size_t len, off_t offset)
{
	int total_write = 0;
	int ret;
	g_return_val_if_fail(lseek(fd, offset, SEEK_SET) >= 0, -1);

	while(total_write < len)
	{
		ret = write(fd, data+total_write, len-total_write);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;
			return ret;
		}
		total_write += ret;
	}

	return total_write;
}

int strg_read(int fd, void *data, size_t len, off_t offset)
{
	int total_read = 0;
	int ret;
	g_return_val_if_fail(lseek(fd, offset, SEEK_SET) >= 0, -1);

	while(total_read < len)
	{
		ret = read(fd, data+total_read, len-total_read);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;
			return ret;
		}
		total_read += ret;
	}

	return total_read;
}

