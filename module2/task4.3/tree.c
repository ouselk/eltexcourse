#include "tree.h"

int max(int a, int b)
{
    return a > b ? a : b;
}

int getHeight(TreeNode *n)
{
    return n ? n->height : 0;
}

int compareNodes(TreeNode *a, TreeNode *b)
{
    int r = strcmp(a->data.family, b->data.family);
    if (r != 0)
        return r;
    return strcmp(a->data.name, b->data.name);
}

static int compareKey(const char *family, const char *name, TreeNode *n)
{
    int r = strcmp(family, n->data.family);
    if (r != 0)
        return r;
    return strcmp(name, n->data.name);
}

TreeNode *rotateRight(TreeNode *n1)
{
    TreeNode *n2 = n1->left;
    TreeNode *t2 = n2->right;

    n2->right = n1;
    n1->left = t2;

    n1->height = max(getHeight(n1->left), getHeight(n1->right)) + 1;
    n2->height = max(getHeight(n2->left), getHeight(n2->right)) + 1;

    return n2;
}

TreeNode *rotateLeft(TreeNode *n1)
{
    TreeNode *n2 = n1->right;
    TreeNode *t2 = n2->left;

    n2->left = n1;
    n1->right = t2;

    n1->height = max(getHeight(n1->left), getHeight(n1->right)) + 1;
    n2->height = max(getHeight(n2->left), getHeight(n2->right)) + 1;

    return n2;
}
static int getBalance(TreeNode *n)
{
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
}
static TreeNode *rebalance(TreeNode *n)
{
    n->height = max(getHeight(n->left), getHeight(n->right)) + 1;
    int balance = getBalance(n);

    if (balance > 1)
    {
        if (getBalance(n->left) < 0) 
            n->left = rotateLeft(n->left);
        return rotateRight(n); 
    }
    if (balance < -1)
    {
        if (getBalance(n->right) > 0)
            n->right = rotateRight(n->right);
        return rotateLeft(n); 
    }
    return n;
}

static TreeNode *minNode(TreeNode *n)
{
    while (n->left)
        n = n->left;
    return n;
}

TreeNode *insertTree(TreeNode *tree, TreeNode *node)
{
    if (tree == NULL)
    {
        node->left = NULL;
        node->right = NULL;
        node->height = 1;
        return node;
    }

    int c = compareNodes(node, tree);
    if (c < 0)
        tree->left = insertTree(tree->left, node);
    else if (c > 0)
        tree->right = insertTree(tree->right, node);
    else
        return tree;

    return rebalance(tree);
}

TreeNode *deleteNode(TreeNode *tree, const char *family, const char *name)
{
    if (tree == NULL)
        return NULL;

    int c = compareKey(family, name, tree);
    if (c < 0)
        tree->left = deleteNode(tree->left, family, name);
    else if (c > 0)
        tree->right = deleteNode(tree->right, family, name);
    else
    {
        if (!tree->left || !tree->right)
        {
            TreeNode *child = tree->left ? tree->left : tree->right;
            free(tree);
            return child;
        }
        TreeNode *succ = minNode(tree->right);
        tree->data = succ->data;
        tree->right = deleteNode(tree->right, tree->data.family, tree->data.name);
    }

    return rebalance(tree);
}
TreeNode *getNode(TreeNode *tree, const char *family, const char *name)
{
    while (tree)
    {
        int c = compareKey(family, name, tree);
        if (c == 0)
            return tree;
        tree = (c < 0) ? tree->left : tree->right;
    }
    return NULL;
}


void preorder(TreeNode *tree, int depth)
{
    if (!tree)
        return;
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("%s %s\n", tree->data.family, tree->data.name);
    preorder(tree->left, depth + 1);
    preorder(tree->right, depth + 1);
}

void inorder(TreeNode *tree, int depth)
{
    if (!tree)
        return;
    inorder(tree->left, depth + 1);
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("%s %s\n", tree->data.family, tree->data.name);
    inorder(tree->right, depth + 1);
}
 
void postorder(TreeNode *tree, int depth)
{
    if (!tree)
        return;
    postorder(tree->left, depth + 1);
    postorder(tree->right, depth + 1);
    for (int i = 0; i < depth; i++)
        putchar('\t');
    printf("%s %s\n", tree->data.family, tree->data.name);
}

