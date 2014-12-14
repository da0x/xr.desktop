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

namespace DMath {

	/*
	=================================================
	SORT SYSTEMS
	=================================================
	*/

	class NaturalMerge;

	/*
	===============
	Sortable Object
	===============
	*/
	class Sortable
	{
		friend class NaturalMerge;
		protected :
			int			index;
			float		value;
			Sortable	*next;
		public:
			Sortable()
				:
					value(NULL),
					index(0),
					next(NULL)
			{
			}
	};

	/*
	===================
	Natural MergeSort 
	===================
	*/

	class NaturalMerge {
	protected:		

		class Sortable	*data;		// data list to compare and sort
		class Sortable	*zdata;		// terminator

		inline class Sortable *Merge(
									class Sortable *a,
									class Sortable *b,
									class Sortable *z)
		{
			class Sortable *c;

			c = z;
			do
			{
				if (a->value >= b->value)
				{
					c->next = a;
					c = a;
					a = a->next;
				}
				else
				{
					c->next = b;
					c = b;
					b = b->next;
				}
			} 
			while ( c != z);
			c = z->next;
			z->next = z;
			return c;
		}

		inline class Sortable *MergeSort(
			class Sortable *c,
			class Sortable *z)
		{
			int i, N;
			class Sortable *a, *b, *head, *todo, *t;
			head = new Sortable;
			head->next = c; 
			a = z;
			for (N = 1; a != head->next; N = N+N) 
			{
				todo = head->next;
				c = head;
				while (todo != z)
				{
					t = todo; a = t; 
					for (i = 1; i < N; i++)
						t = t->next;
					b = t->next;             t->next = z; 
					t = b; 
					for (i = 1; i < N; i++)
						t = t->next;
					todo = t->next;
					t->next = z;
					c->next = Merge(a, b,z);

					for (i = 1; i <= N+N; i++)
						c = c->next;
				}
			}
			return head->next;
		}

	public:
		NaturalMerge():data(NULL),zdata(NULL) {	}
		~NaturalMerge(){	freeData(); }

		/*
		------------------------------------------------------------
		'size' is the array size, 'values' are float values to compare,
		and 'indecies' are current indecies.
		------------------------------------------------------------
		*/

		void pointer(
			size_t		size,
			const float *values,
			const int	*indecies
			)
		{
			delete data;
			delete zdata;

			data = new Sortable;
			zdata = data;
			unsigned int i=0;
			while ( i< size)
			{
				zdata-> value = values[i];
				zdata-> index = indecies[i];
				zdata-> next = new Sortable;
				zdata = zdata->next;
				i++;
			}
		}

		/*
		------------------------------------------------------
		'indecies' are the new indecies of the sorted array.
		------------------------------------------------------
		*/

		void sort(int *indecies)
		{
			if(!data) return;
			MergeSort(data, zdata);
			Sortable *p = data;
			int *i = indecies;
			while(p != zdata){
				*(i++) = p->index;
				p= p->next;
			}
		}

		void freeData()
		{
			if(!data) return;
			Sortable *p = data;
			Sortable *_p;
			while (p && p!=zdata){
				_p = p->next;
				delete p;
				p = _p;
			}
			if(zdata) delete zdata;
		}
	};

	template <class dataType>
	/*
	Note:
		dataType should be a structure or a class
		that has the folowing public member data:
		- 'sortValue' : which is going to be used for comparisons.
		- 'next'	  : a pointer to the next data sequence.
	*/
	class NaturalMergeSortT {
	protected:
		dataType		*data;		// data list to compare and sort
		dataType		*zdata;		// terminator

		inline dataType *Merge(
			dataType *a,
			dataType *b,
			dataType *z)
		{
			dataType *c;

			c = z;
			do {
				if (a->sortValue >= b->sortValue){
					c->next = a;
					c = a;
					a = a->next;
				} else {
					c->next = b;
					c = b;
					b = b->next;
				}
			} while ( c != z);
			c = z->next;
			z->next = z;
			return c;
		}

		inline dataType *MergeSort(
			dataType *c,
			dataType *z)
		{
			int i, N;
			dataType *a, *b, *head, *todo, *t;
			head = new dataType;
			head->next = c; 
			a = z;
			for (N = 1; a != head->next; N = N+N) {
				todo = head->next;
				c = head;
				while (todo != z){
					t = todo; a = t; 
					for (i = 1; i < N; i++)
						t = t->next;
					b = t->next;             t->next = z; 
					t = b; 
					for (i = 1; i < N; i++)
						t = t->next;
					todo = t->next;
					t->next = z;
					c->next = Merge(a, b,z);

					for (i = 1; i <= N+N; i++)
						c = c->next;
				}
			}
			return head->next;
		}

	public:
		NaturalMergeSortT():data(NULL),zdata(NULL) {	}
		~NaturalMergeSortT(){	 }

		void pointer(dataType first, dataType last){
			data = first;
			zdata = last;
		}

		void sort(){
			if(!data) return;
			MergeSort(data, zdata);
		}
	};

	/*
	============
	Quick Sort
	============
	*/

	class Quick {
		protected:
			inline int	compare (const void *, const void *, unsigned);
			inline void shortsort (char *,char *,unsigned);
			inline void swap (char *,char *, unsigned);
		public:
			/* sort the array between lo and hi (inclusive) */
			inline void Sort (void *base,unsigned num,unsigned width);
	};
}//namespace DMath