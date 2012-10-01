// mopack.cpp : Defines the entry point for the console application.
//

#if defined(_WIN32) || defined(_WIN64)
#include "stdafx.h"
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int strvcmp(const void* a, const void* b)
{
	return strcmp(*(char**)a, *(char**)b);
}

#if defined(_WIN32) || defined(_WIN64)
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char** argv)
#endif
{
	qsort(argv + 1, argc - 1, sizeof(argv[0]), strvcmp);
	FILE** r = (FILE**)malloc(sizeof(FILE*) * (argc - 1));
	for (int i = 0; i < argc - 1; i++)
	{
		r[i] = fopen(argv[i + 1], "rb");
		if (r[i] == NULL)
		{
			printf("file : %s is not exist. abort.\n", argv[i + 1]);
			for (int j = 0; j < i; j++)
			   fclose(r[j]);
			free(r);
			return -1;
		}
	}
	int* sizes = (int*)malloc(sizeof(int) * (argc - 1));
	printf("#include <string.h>\n"
		   "typedef struct {\n"
		   "\tconst char* name;\n"
		   "\tshort int fd;\n"
		   "} scan_resource_file_t;\n"
		   "typedef void (*read_resource_t)(void* data);\n");
	for (int i = 0; i < argc - 1; i++)
	{
		printf("static void __read_resource_%d(void* data)\n{\n\tunsigned char* ptr = (unsigned char*)data;\n", i);
		unsigned char buffer[4096];
		sizes[i] = 0;
		size_t re = fread(buffer, 1, 4096, r[i]);
		while (re == 4096)
		{
			sizes[i] += re;
			printf("\t{\n\t\tunsigned char buf[] = \"");
			for (size_t j = 0; j < re; j++)
				printf("\\x%x", buffer[j]);
			printf("\";\n\t\tmemcpy(ptr, buf, %d); ptr += %d;\n\t}\n", re, re);
			re = fread(buffer, 1, 4096, r[i]);
		}
		if (re > 0)
		{
			sizes[i] += re;
			printf("\t{\n\t\tunsigned char buf[] = \"");
			for (size_t j = 0; j < re; j++)
				printf("\\x%x", buffer[j]);
			printf("\";\n\t\tmemcpy(ptr, buf, %d); ptr += %d;\n\t}\n", re, re);
		}
		printf("}\n");
	}
	printf("const scan_resource_file_t __scan_resource_files[] = {\n");
	for (int i = 0; i < argc - 1; i++)
		printf("\t{ \"%s\", %d },\n", argv[i + 1], i);
	printf("};\n");
	printf("const read_resource_t __read_resources[] = {\n");
	for (int i = 0; i < argc - 1; i++)
		printf("\t__read_resource_%d,\n", i);
	printf("};\n");
	printf("const int __resource_size[] = {\n");
	for (int i = 0; i < argc - 1; i++)
		printf("\%d,\n", sizes[i]);
	printf("};\n");
	printf("const int __file_total = %d;\n", argc - 1);
	printf("const scan_resource_file_t* __last_scan_resource_file = __scan_resource_files + %d;\n"
		   "int get_resource_fd(const char* text)\n"
		   "{\n"
		   "	int len;\n"
		   "	char word[1024];\n"
		   "	const scan_resource_file_t* low;\n"
		   "	const scan_resource_file_t* high;\n"
		   "	len = strlen(text);\n"
		   "	if (len >= 1024)\n"
		   "		return -1;\n"
		   "	memcpy(word, text, len);\n"
		   "	word[len] = 0;\n"

		   "	low = &__scan_resource_files[0];\n"
		   "	high = __last_scan_resource_file - 1;\n"
		   "	while (low <= high)\n"
		   "	{\n"
		   "		const scan_resource_file_t *middle;\n"
		   "		int	difference;\n"
		   "		middle = low + (high - low) / 2;\n"
		   "		difference = strcmp(middle->name, word);\n"
		   "		if (difference == 0)\n"
		   "			return middle->fd;\n"
		   "		else if (difference < 0)\n"
		   "			low = middle + 1;\n"
		   "		else\n"
		   "			high = middle - 1;\n"
		   "	}\n"

		   "	return -1;\n"
		   "}\n"
		   "int get_resource_size(int fd)\n"
		   "{\n"
		   "	if (fd >= 0 && fd < __file_total)\n"
		   "		return __resource_size[fd];\n"
		   "	return -1;\n"
		   "}\n"
		   "void read_resource_binary(int fd, void* data)\n"
		   "{\n"
		   "	if (fd >= 0 && fd < __file_total)\n"
		   "		__read_resources[fd](data);\n"
		   "}\n", argc - 1);
	free(sizes);
	for (int i = 0; i < argc - 1; i++)
		fclose(r[i]);
	free(r);
	return 0;
}
