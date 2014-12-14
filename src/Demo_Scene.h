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

#ifndef ___DEMO_SCENE_H
#define ___DEMO_SCENE_H

namespace Demo {

	///////////////////////
	// Scene class
	class Scene {
		public:
			virtual void	Init() = 0;
			virtual void	Update(float msec) = 0;
			virtual void	Render() = 0;
			virtual void	Destroy() = 0;
			virtual void	Restart() = 0;
			virtual bool	Done() {return false;}
	};
}

#endif // ___DEMO_SCENE_H