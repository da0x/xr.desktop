//C++

/*
----------------------------------------------------
The Desktop Project
------------------

Copyright 2004 Daher Alfawares

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

----------------------------------------------------
*/

#include <stdio.h>
#include <stdarg.h>

typedef struct File_s{
	bool	linked;
	FILE	*stream;
} file_s;

#define FS_MAX		1024
static	file_s		file[FS_MAX];

int fs_findfree(){
	int i,c;
	c= FS_MAX;
	for ( i=1; i< c; i++){
		if(! file[i].linked )
			break;
	}
	if(i==c)
		return 0;
	return i;
}

int fs_open(const char *fname, const char *reason){
	int i;

	i = fs_findfree();
	if(i){
		file[i].stream= fopen ( fname, reason );
		if(!file[i].stream)
			return 0;
		file[i].linked = true;
	}
	return i;
}

void fs_close(int &handle){
	if(!handle || !file[handle].linked) return;
	fclose(file[handle].stream);
	file[handle].linked = false;
	handle = 0;
}

int fs_length( int handle){
	int len, pos;
	if(!handle || !file[handle].linked) return 0;
	pos = ftell(file[handle].stream);
	fseek(file[handle].stream, 0, SEEK_END);
	len = ftell(file[handle].stream);
	fseek(file[handle].stream, pos, SEEK_SET);
	return len;
}

int fs_load( char *buffer, int handle){
	int len;

	if(!handle || !file[handle].linked) return 0;
	len = fs_length(handle);
	return (int)fread(buffer, len, 1, file[handle].stream);
}

int fs_read(char *buffer, int size, int handle){
	if(!handle || !file[handle].linked) return 0;
	return (int)fread(buffer, size, 1, file[handle].stream);
}

int fs_write( char *buffer, int size, int handle){
	if(!handle || !file[handle].linked) return 0;
	return (int)fwrite(buffer, size, 1, file[handle].stream);
}

int fs_printf( int handle, char *fmt, ...){
	va_list	ap;
	char text[1024];

	if(!handle || !file[handle].linked) return 0;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	return fprintf(file[handle].stream, text);
}

int fs_scanf( int handle, char *fmt, ...){
	va_list ap;
	int r;

	if(!handle || !file[handle].linked) return 0;
	va_start (ap, fmt);
	r= fscanf(file[handle].stream, fmt, ap);
	va_end(ap);
	return r;
}


bool fs_readln(char	*buffer, int handle){
	if(!handle || !file[handle].linked) return false;

	do
	{
		if(feof(file[handle].stream))	{
			buffer[0] = (char) 0;
			return false;
		}
		fgets(buffer, 255, file[handle].stream);
	} while (
		(buffer[0] == '/') ||
		(buffer[0] == '\n')/*||
		(buffer[0] == '{') ||
		(buffer[0] == '}') */);
	return true;
}

bool fs_writeln(const char *fmt, int handle, ...){
	if(!handle || !file[handle].linked) return false;

	va_list ap;
	char format[1024];
	va_start(ap, handle);
	vsprintf(format, fmt, ap);
	fs_printf(handle, "%s\n", format);
	va_end(ap);
	return true;
}

void fs_flush(int handle){
	if(!handle || !file[handle].linked) return;

	fflush(file[handle].stream);
}