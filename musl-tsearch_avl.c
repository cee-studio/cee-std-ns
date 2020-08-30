#ifdef CEE_AMALGAMATION
#undef S
#define S(x)    _musl_aval_##x
#else
#include "musl-search.h"
#include <stdlib.h>
#endif 
;
/*
avl tree implementation using recursive functions
the height of an n node tree is less than 1.44*log2(n+2)-1
(so the max recursion depth in case of a tree with 2^32 nodes is 45)
*/

struct S(node) {
	const void *key;
	struct S(node) *left;
	struct S(node) *right;
	int height;
};

static int delta(struct S(node) *n) {
	return (n->left ? n->left->height:0) - (n->right ? n->right->height:0);
}

static void updateheight(struct S(node) *n) {
	n->height = 0;
	if (n->left && n->left->height > n->height)
		n->height = n->left->height;
	if (n->right && n->right->height > n->height)
		n->height = n->right->height;
	n->height++;
}

static struct S(node) *rotl(struct S(node) *n) {
	struct S(node) *r = n->right;
	n->right = r->left;
	r->left = n;
	updateheight(n);
	updateheight(r);
	return r;
}

static struct S(node) *rotr(struct S(node) *n) {
	struct S(node) *l = n->left;
	n->left = l->right;
	l->right = n;
	updateheight(n);
	updateheight(l);
	return l;
}

static struct S(node) *balance(struct S(node) *n) {
	int d = delta(n);

	if (d < -1) {
		if (delta(n->right) > 0)
			n->right = rotr(n->right);
		return rotl(n);
	} else if (d > 1) {
		if (delta(n->left) < 0)
			n->left = rotl(n->left);
		return rotr(n);
	}
	updateheight(n);
	return n;
}

static struct S(node) *find(struct S(node) *n, const void *k,
	int (*cmp)(const void *, const void *))
{
	int c;

	if (!n)
		return 0;
	c = cmp(k, n->key);
	if (c == 0)
		return n;
	if (c < 0)
		return find(n->left, k, cmp);
	else
		return find(n->right, k, cmp);
}

static struct S(node) *insert(struct S(node) **n, const void *k,
	int (*cmp)(const void *, const void *), int *new_)
{
	struct S(node) *r = *n;
	int c;

	if (!r) {
		*n = r = (struct S(node) *)malloc(sizeof **n);
		if (r) {
			r->key = k;
			r->left = r->right = 0;
			r->height = 1;
		}
		*new_ = 1;
		return r;
	}
	c = cmp(k, r->key);
	if (c == 0)
		return r;
	if (c < 0)
		r = insert(&r->left, k, cmp, new_);
	else
		r = insert(&r->right, k, cmp, new_);
	if (*new_)
		*n = balance(*n);
	return r;
}

static struct S(node) *movr(struct S(node) *n, struct S(node) *r) {
	if (!n)
		return r;
	n->right = movr(n->right, r);
	return balance(n);
}

static struct S(node) *remove(struct S(node) **n, const void *k,
	int (*cmp)(const void *, const void *), struct S(node) *parent)
{
	int c;

	if (!*n)
		return 0;
	c = cmp(k, (*n)->key);
	if (c == 0) {
		struct S(node) *r = *n;
		*n = movr(r->left, r->right);
		free(r);
    if (r == parent)
      return NULL;
    else
		  return parent;
	}
	if (c < 0)
		parent = remove(&(*n)->left, k, cmp, *n);
	else
		parent = remove(&(*n)->right, k, cmp, *n);
	if (parent)
		*n = balance(*n);
	return parent;
}

void *musl_tdelete(const void * key, void ** rootp,
	int(*compar)(const void *, const void *))
{
	struct S(node) *n = (struct S(node) *)*rootp;
	struct S(node) *ret;
	/* last argument is arbitrary non-null pointer
	   which is returned when the root node is deleted */
	ret = remove(&n, key, compar, n);
	*rootp = n;
	return ret;
}

void *musl_tfind(const void *key, void *const *rootp,
	int(*compar)(const void *, const void *))
{
	return find((struct S(node) *)*rootp, key, compar);
}

void *musl_tsearch(const void *key, void **rootp,
	int (*compar)(const void *, const void *))
{
	int new_ = 0;
	struct S(node) *n = (struct S(node) *)*rootp;
	struct S(node) *ret;
	ret = insert(&n, key, compar, &new_);
	*rootp = n;
	return ret;
}

static void walk(struct S(node) *r, void (*action)(const void *, VISIT, int), int d)
{
	if (r == 0)
		return;
	if (r->left == 0 && r->right == 0)
		action(r, leaf, d);
	else {
		action(r, preorder, d);
		walk(r->left, action, d+1);
		action(r, postorder, d);
		walk(r->right, action, d+1);
		action(r, endorder, d);
	}
}

void musl_twalk(const void *root, void (*action)(const void *, VISIT, int))
{
	walk((struct  S(node) *)root, action, 0);
}

void musl_tdestroy(void *root, void (*freekey)(void *))
{
	struct S(node) *r = (struct S(node) *)root;

	if (r == 0)
		return;
	musl_tdestroy(r->left, freekey);
	musl_tdestroy(r->right, freekey);
	if (freekey) freekey((void *)r->key);
	free(r);
}