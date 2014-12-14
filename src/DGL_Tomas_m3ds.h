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

#ifndef M3DS_H_INCLUDED
#define M3DS_H_INCLUDED

#include "DGL_Tomas_Model.h" // base class

namespace DGL {
	namespace Tomas {

		class M3ds : public Model {
				void Load(std::istream &instream);
				void ReadChunks(std::istream &instream,int chunklength);
			public:
				M3ds(const char *filename);
				M3ds(std::istream &instream);
		};

	} // namespace Tomas
}// namespace DGL

#endif
