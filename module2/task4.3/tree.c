
#include <string.h>
#include "tree.h"

int getHeight(TreeNode*n)
{
    return n ? n->height : 0;
}
int compareNodes (TreeNode* a, TreeNode* b)
{
    int r = strcmp(a->family, b->family);
    if (r != 0)
        return r;
    return strcmp(a->name, b->name);
}

TreeNode *rotateRight(TreeNode* n1)
{
    TreeNode *n2 = n1->left;
    TreeNode *t2 = n2->right;

    n2->right = n1;
    n1->left = t2;

    n1->height = max(getHeight(n1->left), getHeight(n1->right)) + 1;
    n2->height = max(getHeight(n2->left), getHeight(n2->right)) +1;

    return n2;
}

TreeNode *rotateLeft(TreeNode* n1)
{
    TreeNode *n2 = n1->right;
    TreeNode *t2 = n2->left;

    n2->left=n1;
    n1->right=t2;

    n1->height = max(getHeight(n1->left), getHeight(n1->right)) + 1;
    n2->height = max(getHeight(n2->left), getHeight(n2->right)) +1;
    
    return n2;
}

TreeNode