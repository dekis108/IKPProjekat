
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct linked_list {
    struct linked_list* next;
    int value;
}LIST;


void InputElementAtEnd(struct linked_list** head, int value) {
    if (*head == NULL) {
        *head = (struct linked_list*)malloc(sizeof(struct linked_list));
        (*head)->next = NULL;
        (*head)->value = value;
        return;
    }

    struct linked_list* temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = (struct linked_list*)malloc(sizeof(struct linked_list));
    temp->next->next = NULL;
    temp->next->value = value;

}

void TraverseAndPrint(struct linked_list* head) {
    int i = 0;
    while (head != NULL) {
        printf("Value %d: %d\n", ++i, head->value);
        head = head->next;
    }
}

int FindValue(struct linked_list* head, int value) {
    while (head != NULL) {
        if (head->value == value) {
            return 1;
        }
        head = head->next;
    }

    return 0;
}

void InputElementAtStart(struct linked_list** head, int value) {
    if (*head == NULL) {
        *head = (struct linked_list*)malloc(sizeof(struct linked_list));
        (*head)->next = NULL;
        (*head)->value = value;
        return;
    }

    struct linked_list* node = (struct linked_list*)malloc(sizeof(struct linked_list));
    node->value = value;
    node->next = *head;
    *head = node;
}



