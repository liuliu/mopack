#include <string.h>

typedef struct {
	const char* name;
	short int fd;
} scan_resource_file_t;

typedef void (*read_resource_t)(void* data);

static void __read_resource_0(void* data)
{
	unsigned char* ptr = (unsigned char*)data;
	{
		unsigned char buf[] = "\x00\x23";
		memcpy(ptr, buf, 4096); ptr += 4096;
	}
}

const scan_resource_file_t __scan_resource_files[] = {
	{ "2w", 7 },
	{ "ae", 4 },
	{ "bc", 0 },
	{ "fe", 5 },
	{ "ff", 2 },
	{ "ge", 1 },
	{ "me", 3 },
	{ "mmwef", 6 },
};

const read_resource_t __read_resources[] = {
	__read_resource_0,
};

const int __resource_size[] = {
	1
};

const int __file_total = 7;
const scan_resource_file_t* __last_scan_resource_file = __scan_resource_files + __file_total;

int get_resource_fd(const char* text)
{
	int len;
	char word[1024];
	const scan_resource_file_t* low;
	const scan_resource_file_t* high;

	len = strlen(text);
	if (len >= 1024)
		return -1;
	memcpy(word, text, len);
	word[len] = '\0';

	low = &__scan_resource_files[0];
	high = __last_scan_resource_file - 1;
	while (low <= high)
	{
		const scan_resource_file_t *middle;
		int	difference;

		middle = low + (high - low) / 2;
		difference = strcmp(middle->name, word);
		if (difference == 0)
			return middle->fd;
		else if (difference < 0)
			low = middle + 1;
		else
			high = middle - 1;
	}

	return -1;
}

int get_resource_size(int fd)
{
	if (fd >= 0 && fd < __file_total)
		return __resource_size[fd];
}

void read_resource_binary(int fd, void* data)
{
	if (fd >= 0 && fd < __file_total)
		__read_resources[fd](data);
}