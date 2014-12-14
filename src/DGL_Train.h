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

#ifndef ___DGL_TRAIN_H
#define ___DGL_TRAIN_H

namespace DGL {
	/////////////////////////////
	// Train
	class Train {
		private:
			class node_t{
				public:
					Vector		position;
					Vector		direction;
					node_t		*next;
					node_t(){
						next = NULL;
					};
			};

			static const int MillisecondsPerNode = 600;

			node_t		*nodes;
			node_t		*lastnode;
			float		time;
			int			nodeCounter;
			bool		loop;
			bool		done;

			Vector		dst_position;
			Vector		dst_direction;

			void addNode(Vector& cpos, Vector& cdir){
				this->lastnode->position = cpos;
				this->lastnode->direction = cdir;
				this->lastnode->next = new node_t;
				this->lastnode = this->lastnode->next;

					// set as destination.
				dst_position = cpos;
				dst_direction = cdir;
			}
			node_t *extractNode(int numOfNode) const {
				int	i			= 0;
				node_t *node	= this->nodes;
				while(node != this->lastnode && node->next != this->lastnode){
					if(i == numOfNode){
						return node;
					}
					node = node->next->next;
					i++;
				}
				return NULL;
			}

			__forceinline
				void interpolate (
				node_t *n1,
				node_t *n2,
				node_t *out,
				float ratio) const
			{
				if(n2->next != this->lastnode){
					out->position = Interpolate2<Vector>(n1->position, n2->position, n2->next->position,ratio);
					out->direction = Interpolate2<Vector>(n1->direction, n2->direction, n2->next->direction,ratio);
				} else {
					out->position = Interpolate<Vector>(n1->position, n2->position, ratio);
					out->direction = Interpolate<Vector>(n1->direction, n2->direction, ratio);
				}
			}
		public:
			void PushNode(Vector& position, Vector& direction){
				LogPrint("Adding to train: node number %d", ++this->nodeCounter);
				this->addNode(position,direction);
			}

			bool RemoveLast(){
				LogPrint("Removing from train: node number %d", --this->nodeCounter);
				if(this->nodes == this->lastnode){
					LogError("No nodes to remove");
					return false;
				}

				node_t *n = this->nodes;
				while(n->next != this->lastnode)
					n = n->next;
				delete this->lastnode;
				this->lastnode = n;
				return true;
			}

			void Update(float msec, Vector& position, Vector& direction)
				{
				time += msec;

				node_t *n1,*n2;


				int numofnode	= ((int)this->time/(float) this->MillisecondsPerNode);

				n1 = extractNode(numofnode);
				if(!n1 || n1 == this->lastnode  || n1->next == this->lastnode )
					{
					this->Restart();
					if(nodes != this->lastnode) // if there is a first node
						{
						if( this->loop )
							{
							position = nodes->position;
							direction = nodes->direction;
							}
						else
							{ // if not looping, stay at the last node
							this->done = true;
							position = dst_position;
							direction = dst_direction;
							}
						}
					else
						{
						position.Set(0,0,0);
						direction.Set(0,0,-1);
						}
					
					return;
					}
				n2 = n1->next;

				node_t *current = new node_t;
				// find node time
				float nodetime = (this->time - numofnode * (float) this->MillisecondsPerNode);
				float ratio = nodetime / (float) this->MillisecondsPerNode;

				if( !loop )
					DMath::Clamp<float>( 0, 1, ratio );

				this->interpolate(n1,n2, current, ratio);
				position = current->position;
				direction = current->direction;
				delete current;
			}

			void Restart(){
				this->time = 0;
				this->done = false;
			}

			bool Done()
				{
				return this->done;
				}

			void Destroy(){	// this frees the nodes but still can accept nodes
				node_t *node,*nodeprev;
				node = this->nodes;
				while(node){
					nodeprev = node;
					node = node->next;
					delete nodeprev;
				}

				this->nodes = new node_t;
				this->lastnode= this->nodes;
				this->time = 0;
				this->nodeCounter = 0;
			}

			Train(){
				this->nodes = new node_t;
				this->lastnode= this->nodes;
				this->time = 0;
				this->nodeCounter = 0;
				this->loop = false;
				this->done = false;
			}

			~Train(){
				LogPrint ("Freeing train nodes...");
				node_t *node,*nodeprev;
				node = this->nodes;
				while(node){
					nodeprev = node;
					node = node->next;
					delete nodeprev;
				}
				this->nodes = NULL;
				this->lastnode = NULL;
			}


			// IO
			void Dump(const char *name) const
			{
				LogPrint("Dumping Train Script \"scripts/%s.train\"",name);

				FILE *fp = fopen(va("scripts/%s.train", name), "wt");
				if(!fp) {
					LogPrint("Error: Not enough disk space");
					return;
				}
				node_t *node = this->nodes;
				int n = 0;
				fprintf(fp,"// Train script generated by DaherEngine\n// Do not modify.\n\n");
				while(node && node != this->lastnode){
					fprintf(fp, "%f , %f , %f , %f , %f , %f\n",
						node->position.x_(),node->position.y_(),node->position.z_(),
						node->direction.x_(),node->direction.y_(),node->direction.z_());
					node = node->next;
					n++;
				}
				LogPrint("Script dumped successfully: Total of %d nodes",n);
			}

			void Load(const char *name){
				LogPrint("Loading train script \"scripts/%s.train\"",name);
				FILE *fp = fopen(va("scripts/%s.train", name), "rt");
				if(!fp) {
					LogPrint("Error: File not found \"scripts/%s.train\"",name);
					return;
				}
				Vector p,d;
				int n = 0;
				char data[1024];
				while(D_fGetLine(fp, data)){
					if(sscanf(data, "%f , %f , %f , %f , %f , %f",
						&p.x_(),&p.y_(),&p.z_(),&d.x_(),&d.y_(),&d.z_())){
							this->addNode(p,d);
							n++;
						}
				}
				fclose(fp);
				LogPrint("Script loaded successfully: Found %d nodes",n);
			}
	};
}// namespace DGL

#endif // ___DGL_TRAIN_H