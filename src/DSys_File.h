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

#ifndef ___DFILE_H
#define ___DFILE_H

/*
TODO List:
- ...
*/


typedef int file_t;

file_t fs_open( const char *fname, const char *reason);
void fs_close( file_t& handle);
int fs_length( file_t handle);
int fs_load( char *buffer, file_t handle);
int fs_read( char *buffer, int size, file_t handle);
int fs_write( char *buffer, int size, file_t handle);
int fs_printf( file_t handle, char *fmt, ...);
int fs_scanf( file_t handle, char *fmt, ...);
bool fs_readln(char	*buffer, file_t handle);
bool fs_writeln(const char *fmt, file_t handle, ...);
void fs_flush(file_t handle);

#endif // ___DFILE_H