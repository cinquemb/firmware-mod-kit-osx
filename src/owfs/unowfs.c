/*
 * Utility to extract files from OWFS file system images.
 * 
 * $ gcc -Wall unowfs.c -o unowfs
 *
 * Craig Heffner, 20 June 2011
 * http://www.devttys0.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define MAGIC "owowowowowowowowowowowowowowowow"
#define MAGIC_SIZE 32
#define FILE_NAME_MAX_SIZE 40
#define DEFAULT_DEST_DIR "owfs-root"
#define DIRECTORY_TRAVERSAL "../"
#define FILE_PREFIX "./"
#define FILE_PREFIX_SIZE 2
#define FILE_PATH_SIZE 43

/* OWFS structures */
#pragma pack(1)
struct owfs_header
{
	char magic[MAGIC_SIZE];
	uint32_t version;
	uint32_t num_entries;
	uint32_t unknown;
};

struct owfs_entry
{
	char name[FILE_NAME_MAX_SIZE];
	uint32_t size;
	uint32_t offset;
};
#pragma pack()

/* Function declarations */
char *file_read(char *file, size_t *fsize);
int file_write(char *file, char *data, size_t size);
int unowfs(char *data, size_t size);


int main(int argc, char *argv[])
{
	int file_count = 0, retval = EXIT_FAILURE;
	char *file = NULL, *dir = NULL, *owbuf = NULL;
	size_t file_size = 0;

	if(argc < 2 || argv[1][0] == '-')
	{
		fprintf(stderr, "\nUsage: %s <owfs image> [destination directory]\n\n", argv[0]);
		goto end;
	}

	file = argv[1];

	if(argc == 3)
	{
		dir = argv[2];
	}
	else
	{
		dir = DEFAULT_DEST_DIR;
	}

	owbuf = file_read(file, &file_size);
	if(!owbuf)
	{
		goto end;
	}

	if(mkdir(dir, (S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
	{
		perror(dir);
		goto end;
	}

	if(chdir(dir) < 0)
	{
		perror(dir);
		goto end;
	}

	file_count = unowfs(owbuf, file_size);
	if(file_count > 0)
	{
		fprintf(stderr, "\nExtracted %d files to ./%s/\n", file_count, dir);
		retval = EXIT_SUCCESS;
	}

end:
	if(owbuf) free(owbuf);
	return retval;
}

/* Parse and extract files from OWFS image */
int unowfs(char *data, size_t size)
{
	int count = 0, i = 0, offset = 0;
	struct owfs_header *header = NULL;
	struct owfs_entry * entry = NULL;
	char file_name[FILE_PATH_SIZE] = { 0 };

	/* Validate image size */
	if(size < sizeof(struct owfs_header))
	{
		fprintf(stderr, "Invalid image: size too small\n");
		goto end;
	}

	header = (struct owfs_header *) data;

	/* Validate magic signature */
	if(memcmp(header->magic, MAGIC, MAGIC_SIZE) != 0)
	{
		fprintf(stderr, "Invalid image: bad magic signature\n");
		goto end;
	}
	
	/* Values are in big endian format */
	header->version = htonl(header->version);
	header->num_entries = htonl(header->num_entries);

	fprintf(stderr, "Extracting %d files from OWFS version %d image...\n\n", header->num_entries, header->version);

	/* Loop through all file entries and write file data to disk */
	for(i=0,offset=sizeof(struct owfs_header); (i < header->num_entries && offset < size); i++,offset+=sizeof(struct owfs_entry))
	{
		entry = (struct owfs_entry *) (data + offset);

		entry->size = htonl(entry->size);	
		entry->offset = htonl(entry->offset);

		/* Make sure specially crafted file names don't write outside of the destination directory */
		if(strstr(entry->name, DIRECTORY_TRAVERSAL))
		{
			fprintf(stderr, "Refusing to extract potentially malicious file: %s\n", entry->name);
			continue;
		}

		/* Prepend './' to the file name to ensure it get written to a relative path */
		memset((char *) &file_name, 0, FILE_PATH_SIZE);
		strncpy((char *) &file_name, FILE_PREFIX, FILE_PREFIX_SIZE);
		strncat((char *) &file_name, entry->name, (FILE_PATH_SIZE - FILE_PREFIX_SIZE));

		fprintf(stderr, "%s [%d]\n", entry->name, entry->size);

		/* Write data to disk */
		if(file_write((char *) &file_name, (data + entry->offset), entry->size))
		{
			count++;
		}
		else
		{
			fprintf(stderr, "Failed to write data to disk for file %s\n", entry->name);
		}
	}

end:
	return count;
}

/* Read in file contents */
char *file_read(char *file, size_t *fsize)
{
        int fd = 0;
        struct stat _fstat = { 0 };
        char *buffer = NULL;

        if(stat(file,&_fstat) == -1)
        {
                goto end;
        }

        if(_fstat.st_size == 0)
        {
                goto end;
        }

        fd = open(file,O_RDONLY);
        if(!fd)
        {
                goto end;
        }

        buffer = malloc(_fstat.st_size);
        if(!buffer)
        {
                goto end;
        }
        memset(buffer,0,_fstat.st_size);

        if(read(fd,buffer,_fstat.st_size) != _fstat.st_size)
        {
                if(buffer) free(buffer);
                buffer = NULL;
        }
        else
        {
                *fsize = _fstat.st_size;
        }

end:
        if(fd) close(fd);
        return buffer;
}

/* Write data to disk */
int file_write(char *file, char *data, size_t size)
{
	FILE *fp = NULL;
	int retval = 0;

	fp = fopen(file, "w");
	if(!fp)
	{
		perror(file);
		goto end;
	}

	if(fwrite(data, 1, size, fp) != size)
	{
		perror(file);
		goto end;
	}

	retval = 1;

end:
	if(fp) fclose(fp);
	return retval;
}
