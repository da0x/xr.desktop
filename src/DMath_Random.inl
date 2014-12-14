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

	inline void Random::Randomize()
	{
		Random::Seed (time(NULL));
	}

	inline void Random::Seed (unsigned int seed)
	{
		Random::holdrand = seed;
	}

	inline int Random::Int ( void )
	{
		return (((Random::holdrand = Random::holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
	}
	
	inline float Random::Float ()
	{
		return (float)Random::Int() / (float)0x7fff;
	}
