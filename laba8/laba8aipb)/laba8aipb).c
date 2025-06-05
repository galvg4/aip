#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct WordNode {
    char word[50]; 
    struct WordNode* prev;
    struct WordNode* next;
} WordNode;


WordNode* createWordNode(char* word) {
    if (strlen(word) >= 50) {
        printf("������: ����� '%s' ������� �������\n", word);
        return NULL;
    }
    WordNode* newNode = (WordNode*)malloc(sizeof(WordNode));
    if (!newNode) {
        printf("������ ��������� ������!\n");
        exit(1);
    }
    strncpy(newNode->word, word, sizeof(newNode->word) - 1);
    newNode->word[sizeof(newNode->word) - 1] = '\0';
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}


void addWordNode(WordNode** head, char* word) {
    if (strlen(word) == 0) return; 
    WordNode* newNode = createWordNode(word);
    if (!*head) {
        *head = newNode;
        return;
    }
    WordNode* current = *head;
    while (current->next) {
        current = current->next;
    }
    current->next = newNode;
    newNode->prev = current;
}


void printWordList(WordNode* head) {
    if (!head) {
        printf("������ ����\n");
        return;
    }
    WordNode* current = head;
    while (current) {
        printf("%s ", current->word);
        current = current->next;
    }
    printf("\n");
}


void freeWordList(WordNode* head) {
    WordNode* current = head;
    while (current) {
        WordNode* temp = current;
        current = current->next;
        free(temp);
    }
}


int isValidWord(const char* str) {
    if (str[0] == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] < 'a' || str[i] > 'z') return 0;
    }
    return 1;
}


void removeMaxWords(WordNode** head) {
    if (!*head) return;

    
    WordNode* current = *head;
    char maxWord[50] = "";
    while (current) {
        if (strcmp(current->word, maxWord) > 0) {
            strncpy(maxWord, current->word, sizeof(maxWord) - 1);
            maxWord[sizeof(maxWord) - 1] = '\0';
        }
        current = current->next;
    }

    
    current = *head;
    while (current) {
        WordNode* next = current->next;
        if (strcmp(current->word, maxWord) == 0) {
            if (current->prev) current->prev->next = current->next;
            else *head = current->next; 
            if (current->next) current->next->prev = current->prev;
            free(current);
        }
        current = next;
    }
}


void processWords() {
    WordNode* list = NULL;
    printf("������� ������ �� ���� (�������� ��������� �����, ��������� ������� � ��������, ����� - ����� ������ � ��������� ������): ");
    char input[256];
    while (1) {
        if (!fgets(input, sizeof(input), stdin)) {
            printf("������ ������ ������. ���������� �����: ");
            continue;
        }
        input[strcspn(input, "\n")] = '\0';

        if (input[strlen(input) - 1] != '.') {
            printf("������: ������ ������ ������������� ������. ���������� �����: ");
            continue;
        }

        char* token = strtok(input, ", ");
        int valid = 1;
        while (token) {
            char word[50];
            if (token[strlen(token) - 1] == '.') {
                strncpy(word, token, strlen(token) - 1);
                word[strlen(token) - 1] = '\0';
            }
            else {
                strcpy(word, token);
            }

            if (!isValidWord(word)) {
                printf("������: ����� '%s' �������� ������������ �������. ����������� ������ �������� ��������� �����.\n���������� �����: ");
                valid = 0;
                break;
            }

            addWordNode(&list, word);
            token = strtok(NULL, ", ");
        }

        if (!list) {
            printf("������: ������ ���� ����. ���������� �����: ");
            valid = 0;
        }

        if (valid) break;
        freeWordList(list);
        list = NULL;
    }

    printf("�������� ������ ����: ");
    printWordList(list);
    removeMaxWords(&list);
    printf("������ ����� �������� ������������� �����: ");
    printWordList(list);

    freeWordList(list);
}

int main() {
    setlocale(LC_ALL, "Russian_Russia.1251"); 
    processWords();
    return 0;
}