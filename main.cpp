#include <stdio.h>
#include "tree.h"
#include <chrono>
#include <ctime>

char* text = 0;
size_t V = 1, R = 0;
node* NODES = 0;

inline void find_child(node*& AP, node*& APT, const unsigned int& k)
{
	if (AP->children)
	{
		APT = AP->children[text[k] - 'A'];
	}
}
inline void canonize(node*& AP, node*& APT, const unsigned int& i, unsigned int& k)
{
	if (i == k) return;
	find_child(AP, APT, k);
	unsigned int range;
	while ((range = APT->r - APT->l) < i - k) // Canonize
	{
		AP = APT;
		k += range + 1;
		if (k == i)
		{
			APT = 0;
			return;
		}
		find_child(AP, APT, k);
	}
}
inline void split(node*& AP, node*& APT, const unsigned int& i, unsigned int& k)
{
	//NODES[V].num_children = 2;
	NODES[V].children = new node*[26]();
	NODES[V].l = APT->l;
	NODES[V].r = APT->l + (i - k - 1);
	NODES[V].children[text[i] - 'A'] = NODES + V + 1;
	NODES[V + 1].l = i;
	AP->children[text[APT->l] - 'A'] = NODES + V;
	APT->l = NODES[V].r + 1;
	NODES[V].children[text[APT->l] - 'A'] = APT;
	APT = NODES + V;
	V += 2;
}
inline void add_child(node*& AP, const unsigned int& i)
{
	AP->resize();
	AP->children[text[i] - 'A'] = NODES + V;
	NODES[V].l = i;
	++V;
}
void smetni_teq_rebra(unsigned int inf)
{
	node* T;
	for (unsigned int i = 1; i < V; ++i)
	{
		T = NODES + i;
		if (T->r == (unsigned int)-1)
		{
			R += inf - T->l + 1;
		}
		else
		{
			R += T->r - T->l + 1;
		}
	}
}
void algo()
{
	node* root = NODES;
	node* active_point = root;
	node* active_point_transition = 0;
	unsigned int k = 0;
	unsigned int i = 0;
	unsigned int index;
	while (text[i])
	{
		if (!active_point_transition)
		{
			k = i;
			find_child(active_point, active_point_transition, i);
			if (!active_point_transition) // We don't have a transition with that letter
			{
				add_child(active_point, i);
				while(active_point != root)
				{
					active_point = NODES + active_point->slink;
					find_child(active_point, active_point_transition, i);
					if (active_point_transition)
					{
						if (active_point_transition->l == active_point_transition->r) // Canonize
						{
							active_point = active_point_transition;
							active_point_transition = 0;
						}
						break;
					}
					else
					{
						add_child(active_point, i);
					}
				}
			}
			else if (active_point_transition->l == active_point_transition->r) // Canonize
			{
				active_point = active_point_transition;
				active_point_transition = 0;
			}
		}
		else
		{
			index = active_point_transition->l + (i - k);
			if (text[index] != text[i]) // Split
			{
				node* old_node;
				split(active_point, active_point_transition, i, k);
				old_node = active_point_transition;
 				while (k != i)
				{
					if (active_point != root)
					{
						active_point = NODES + active_point->slink;
					}
					else
					{
						++k;
					}
					active_point_transition = 0;
					canonize(active_point, active_point_transition, i, k);
					if (active_point_transition)
					{
						index = active_point_transition->l + (i - k);
						if (text[index] != text[i])
						{
							split(active_point, active_point_transition, i, k);
							old_node->slink = (active_point_transition - NODES);
							old_node = active_point_transition;
						}
						else
						{
							break;
						}
					}
					else
					{
						old_node->slink = (active_point - NODES);
						break;
					}
				}
				while (true) // k == i
				{
					find_child(active_point, active_point_transition, i);
					if (active_point_transition)
					{
						if (active_point_transition->l == active_point_transition->r) // Canonize
						{
							active_point = active_point_transition;
							active_point_transition = 0;
						}
						break;
					}
					else
					{
						add_child(active_point, i);
						if (active_point != root)
							active_point = NODES + active_point->slink;
						else
							break;
					}
				}
			}
			else if (index == active_point_transition->r) // Canonize
			{
				active_point = active_point_transition;
				active_point_transition = 0;
			}
		}
		++i;
	}
	smetni_teq_rebra(i - 1);
}
using namespace std::chrono;
int main(int argc, char* argv[])
{
	auto t1 = high_resolution_clock::now();
	if (argc == 1)
	{
		printf("No file argument passed\n");
		return -1;
	}
	FILE* fp = fopen(argv[1], "r");
	if (fp == 0)
	{
		perror("Failed to open file");
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	rewind(fp);

	text = new char[size + 2];
	NODES = new node[size * 2 + 2];
	
	size_t res = fread(text, 1, size, fp);
	//printf("Text size: %zu\n", size + 1);
	text[size] = 0;// '$';
	text[size + 1] = 0;
	if (res != size)
	{
		perror("Failed to read file");
		delete[] text;
		delete[] NODES;
		return -1;
	}
	fclose(fp);
	
	algo();

	printf("Number of vertecies = %u\n", V);
	printf("Number of links = %u\n", R);

	delete[] NODES;
	delete[] text;

	auto t2 = high_resolution_clock::now();
	auto time_span = (t2 - t1);
	printf("Finished in %lf seconds.\n", time_span.count() / 1e9);
	return 0;
}
