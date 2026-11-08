#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "phonebook.h"

typedef struct TreeNode
{
    Contact data;
    struct TreeNode *left;
    struct TreeNode *right;
    int height;
} TreeNode;

int max(int a, int b);
int getHeight(TreeNode *n);
int compareNodes(TreeNode *a, TreeNode *b);
TreeNode *rotateRight(TreeNode *n1);
TreeNode *rotateLeft(TreeNode *n1);
TreeNode *insertTree(TreeNode *tree, TreeNode *node);
TreeNode *deleteNode(TreeNode *tree, const char *family, const char *name);
TreeNode *getNode(TreeNode *tree, const char *family, const char *name);
void preorder(TreeNode *tree,int depth);
void inorder(TreeNode *tree,int depth);
void postorder(TreeNode *tree,int depth);

#endif