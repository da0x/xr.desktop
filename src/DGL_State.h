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

#ifndef ___DGL_STATE_H
#define ___DGL_STATE_H

/* TODO List:
- inhirt from std::stack
- ...
*/

namespace DGL {

	class State {
		public:
			//////////////////////////////////////////////
			// State constructor needs the the stack name
			State(int name) :	stateName(name),
								depth(0)
			{
			}

			/////////////////////////////////////
			// Enable the state
			void Enable()
			{
				glEnable(this->stateName);
			}

			/////////////////////////////////////
			// Disable the state
			void Disable()
			{
				glDisable(this->stateName);
			}

			/////////////////////////////////////
			// Push current value into the stack
			void Push()
			{
				this->stack[this->depth++] = (bool) glIsEnabled(this->stateName);
				if(this->depth >= _STACK_DEPTH )
					throw Daher::Exception("DGL::State::depth exceeded its maximum value");
			}

			////////////////////////////////////
			// Pop current value into the stack
			void Pop()
			{
				if(this->depth-- <= 0)
					throw Daher::Exception("DGL::State::depth is lower than its minimum value");
				if(this->stack[this->depth])
					Disable();
				else
					Enable();
			}


		private:
			int				stateName;
			static const int _STACK_DEPTH = 10;
			bool			stack[_STACK_DEPTH];
			short int		depth;
	};


	/////////////////////////////////////
	// Gloabal instances of opengl states
//	State Lighting(GL_LIGHTING);
//	State Texture2D(GL_TEXTURE_2D);
//	State CullFace(GL_CULL_FACE);
}

#endif // ___DGL_STATE_H