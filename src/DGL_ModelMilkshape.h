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

#ifndef ___DGL_MODELMILKSHAPE_H
#define ___DGL_MODELMILKSHAPE_H

/*
TODO List:
- ...
*/


namespace DGL {

	// Declerations of base class
	class Model;


	class ModelMilkshape : public Model
	{
		public:
			/*	Constructor. */
			ModelMilkshape();

			/*	Destructor. */
			virtual ~ModelMilkshape();

			/*	
				Load the model data into the private variables. 
					filename			Model filename
			*/
			virtual bool LoadModelData( std::string filename );
	};
}//namespace DGL
#endif // ___DGL_MODELMILKSHAPE_H