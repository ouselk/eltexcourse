#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "phonebook.h"
#include "tree.h"

void initContactBook(ContactBook *cb)
{
    cb->root = NULL;
    cb->count = 0;
    cb->size = CONTACT_BOOK_SIZE;
}

int findContact(const ContactBook *cb, const char *family, const char *name,
                Contact *out)
{
    if (cb == NULL || family == NULL || name == NULL)
        return -1;

    TreeNode *n = getNode(cb->root, family, name);
    if (n == NULL)
        return -1;

    if (out != NULL)
        *out = n->data;
    return 0;
}

int addContact(ContactBook *cb, const Contact *contact)
{
    if (cb == NULL || contact == NULL)
        return -1;
    if (contact->family[0] == '\0' || contact->name[0] == '\0')
        return -1;

    if (cb->count >= cb->size)
        return -1;
    if (findContact(cb, contact->family, contact->name, NULL) == 0)
        return -1;
    TreeNode *node = malloc(sizeof(TreeNode));
    if (node == NULL)
        return -1;
    node->data = *contact;

    cb->root = insertTree(cb->root, node);
    cb->count++;
    return 0;
}

int delContact(ContactBook *cb, const char *family, const char *name)
{
    if (cb == NULL || family == NULL || name == NULL)
        return -1;

    if (getNode(cb->root, family, name) == NULL)
        return -1;

    cb->root = deleteNode(cb->root, family, name);
    cb->count--;
    return 0;
}

int updateContact(ContactBook *cb, const char *family, const char *name,
                  const Contact *new_contact)
{
    if (cb == NULL || family == NULL || name == NULL || new_contact == NULL)
        return -1;

    if (new_contact->family[0] == '\0' || new_contact->name[0] == '\0')
        return -1;

    TreeNode *old = getNode(cb->root, family, name);
    if (old == NULL)
        return -1;

    if (strcmp(new_contact->family, old->data.family) == 0 &&
        strcmp(new_contact->name, old->data.name) == 0) {
        old->data = *new_contact;
        return 0;
    }

    if (getNode(cb->root, new_contact->family, new_contact->name) != NULL)
        return -1;

    TreeNode *node = malloc(sizeof(TreeNode));
    if (node == NULL)
        return -1;
    node->data = *new_contact;

    cb->root = deleteNode(cb->root, family, name);
    cb->root = insertTree(cb->root, node);
    return 0;
}

void printContacts(const ContactBook *cb)
{
    printf("PREORDER:\n");
    preorder(cb->root,0);
    printf("POSTORDER:\n");
    postorder(cb->root,0);
    printf("INORDER\n");
    inorder(cb->root,0);
}