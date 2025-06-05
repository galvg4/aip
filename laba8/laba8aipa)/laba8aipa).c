#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <locale.h>

typedef struct Node {
    int deg, coeff;
    struct Node* next;
} Node;

typedef struct linked_list {
    Node* head;
    Node* tail;
    int size;
} linked_list;

void start_list(linked_list* list) {
    list->head = list->tail = NULL;
    list->size = 0;
}

Node* make_node(int deg, int coeff) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Ошибка выделения памяти\n");
        exit(1);
    }
    newNode->deg = deg;
    newNode->coeff = coeff;
    newNode->next = NULL;
    return newNode;
}

void add_end(linked_list* list, int coeff, int deg) {
    if (coeff == 0) {
        return;
    }

    Node* newNode = make_node(deg, coeff);
    if (list->tail == NULL) {
        list->tail = list->head = newNode;
    }
    else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
}

void clear_list(linked_list* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void show_list(linked_list* list) {
    Node* current = list->head;
    bool first_node = true;

    while (current != NULL) {
        if (!first_node) {
            printf(current->coeff > 0 ? " + " : " - ");
        }
        else {
            first_node = false;
            if (current->coeff < 0) printf("-");
        }

        int abs_coeff = abs(current->coeff);
        if (abs_coeff != 1 || current->deg == 0) {
            printf("%d", abs_coeff);
        }

        if (current->deg > 0) {
            printf("x");
            if (current->deg > 1) printf("^%d", current->deg);
        }

        current = current->next;
    }
    printf("\n");
}

void get_poly(linked_list* list) {
    int max_degree;
    printf("Введите максимальную степень многочлена (n): ");
    scanf_s("%d", &max_degree);
    getchar();

    for (int deg = max_degree; deg >= 0; deg--) {
        int coeff;
        if (deg > 1) {
            printf("Введите коэффициент при x^%d: ", deg);
        }
        else if (deg == 1) {
            printf("Введите коэффициент при x: ");
        }
        else {
            printf("Введите коэффициент при x^0 (константе): ");
        }

        scanf_s("%d", &coeff);

        if (coeff != 0) {
            add_end(list, coeff, deg);
        }
    }
}

void swap_odd(linked_list* L1, linked_list* L2) {
    if (L2->head == NULL) {
        printf("L2 пуст! Замена не произведена.\n");
        return;
    }

    int min_deg = L2->head->deg;
    int min_coeff = L2->head->coeff;

    Node* current = L2->head->next;
    while (current != NULL) {
        if (current->deg < min_deg) {
            min_deg = current->deg;
            min_coeff = current->coeff;
        }
        current = current->next;
    }

    current = L1->head;
    Node* prev = NULL;
    while (current != NULL) {
        if (current->coeff % 2 != 0) {
            current->coeff = min_coeff;

            if (min_coeff == 0) {
                Node* toDelete = current;
                if (prev == NULL) {
                    L1->head = current->next;
                    current = current->next;
                    free(toDelete);
                    L1->size--;
                    if (L1->head == NULL) {
                        L1->tail = NULL;
                    }
                    continue;
                }
                else {
                    prev->next = current->next;
                    if (current == L1->tail) {
                        L1->tail = prev;
                    }
                    current = current->next;
                    free(toDelete);
                    L1->size--;
                    continue;
                }
            }
        }
        prev = current;
        current = current->next;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    linked_list L1, L2;
    start_list(&L1);
    start_list(&L2);
    int choice;

    while (1) {
        printf("\nМеню:\n");
        printf("1. Создать L1\n");
        printf("2. Создать L2\n");
        printf("3. Выполнить замену нечетных коэффициентов в L1 (используя L2)\n");
        printf("4. Вывести L1\n");
        printf("5. Вывести L2\n");
        printf("6. Выход\n");
        printf("Выберите опцию (1-6): ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            clear_list(&L1);
            start_list(&L1);
            get_poly(&L1);
            printf("L1 создан: ");
            show_list(&L1);
            break;
        case 2:
            clear_list(&L2);
            start_list(&L2);
            get_poly(&L2);
            printf("L2 создан: ");
            show_list(&L2);
            break;
        case 3:
            if (L1.size == 0 || L2.size == 0) {
                printf("L1 или L2 не созданы! Сначала создайте оба многочлена.\n");
                break;
            }
            swap_odd(&L1, &L2);
            printf("Замена выполнена. L1 после замены: ");
            show_list(&L1);
            break;
        case 4:
            if (L1.size == 0) {
                printf("L1 пуст\n");
            }
            else {
                printf("L1: ");
                show_list(&L1);
            }
            break;
        case 5:
            if (L2.size == 0) {
                printf("L2 пуст\n");
            }
            else {
                printf("L2: ");
                show_list(&L2);
            }
            break;
        case 6:
            clear_list(&L1);
            clear_list(&L2);
            printf("Программа завершена.\n");
            return 0;
        default:
            printf("Неверный выбор! Попробуйте снова.\n");
        }
    }
    return 0;
}