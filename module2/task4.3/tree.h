#ifndef TREE_H
#define TREE_H

typedef struct
{
    char* family;
    char* name;
    TreeNode* left;
    TreeNode* right;
    int height;
} TreeNode;



#endif