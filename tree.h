#pragma once

struct node
{
	//unsigned char num_children;
	unsigned int l, r;
	unsigned int slink;
	node** children;

	node() :
		/*num_children(0),*/ l(0), r((unsigned int)-1), children(0), slink(0)
	{
	}
	~node()
	{
		delete[] children;
	}
	inline void resize()
	{
		if (children) return;
		children = new node*[26]();
	}
};