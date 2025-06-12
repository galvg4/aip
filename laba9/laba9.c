#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h> // Для высокоточного таймера

#include "athlete.h"
#include "athlete_generator.h"

// Высокоточный таймер
static double get_current_time() {
    LARGE_INTEGER freq, time;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&time);
    return (double)time.QuadPart / freq.QuadPart * 1000.0; 
}


static int linear_search(Athlete* array, int size, const char* key) {
    for (int i = 0; i < size; i++) {
        if (strcmp(array[i].name, key) == 0) {
            return i;
        }
    }
    return -1;
}


typedef struct BTNode {
    Athlete data;
    struct BTNode* left;
    struct BTNode* right;
} BTNode;

static BTNode* create_bt_node(Athlete data) {
    BTNode* node = malloc(sizeof * node);
    node->data = data;
    node->left = node->right = NULL;
    return node;
}

static void bt_insert(BTNode** root, Athlete data) {
    if (!*root) {
        *root = create_bt_node(data);
    }
    else if (strcmp(data.name, (*root)->data.name) < 0) {
        bt_insert(&(*root)->left, data);
    }
    else {
        bt_insert(&(*root)->right, data);
    }
}

static BTNode* bt_search(BTNode* root, const char* key) {
    if (!root) return NULL;
    int cmp = strcmp(key, root->data.name);
    if (cmp == 0) return root;
    return (cmp < 0)
        ? bt_search(root->left, key)
        : bt_search(root->right, key);
}

static void free_bt(BTNode* root) {
    if (!root) return;
    free_bt(root->left);
    free_bt(root->right);
    free(root);
}


typedef struct RBNode {
    Athlete data;
    char     color;  /* 'R' или 'B' */
    struct RBNode* left, * right, * parent;
} RBNode;

static RBNode* create_rb_node(Athlete data) {
    RBNode* n = malloc(sizeof * n);
    n->data = data;
    n->color = 'R';
    n->left = n->right = n->parent = NULL;
    return n;
}

static void left_rotate(RBNode** root, RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent)            *root = y;
    else if (x == x->parent->left)  x->parent->left = y;
    else                            x->parent->right = y;
    y->left = x;
    x->parent = y;
}

static void right_rotate(RBNode** root, RBNode* y) {
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right) x->right->parent = y;
    x->parent = y->parent;
    if (!y->parent)             *root = x;
    else if (y == y->parent->left)  y->parent->left = x;
    else                             y->parent->right = x;
    x->right = y;
    y->parent = x;
}

static void rb_insert_fixup(RBNode** root, RBNode* z) {
    while (z->parent && z->parent->color == 'R') {
        RBNode* gp = z->parent->parent;
        if (z->parent == gp->left) {
            RBNode* y = gp->right;
            if (y && y->color == 'R') {
                z->parent->color = y->color = 'B';
                gp->color = 'R';
                z = gp;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(root, z);
                }
                z->parent->color = 'B';
                gp->color = 'R';
                right_rotate(root, gp);
            }
        }
        else {
            RBNode* y = gp->left;
            if (y && y->color == 'R') {
                z->parent->color = y->color = 'B';
                gp->color = 'R';
                z = gp;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(root, z);
                }
                z->parent->color = 'B';
                gp->color = 'R';
                left_rotate(root, gp);
            }
        }
    }
    (*root)->color = 'B';
}

static void rb_insert(RBNode** root, Athlete data) {
    RBNode* z = create_rb_node(data);
    RBNode* y = NULL;
    RBNode* x = *root;
    while (x) {
        y = x;
        x = (strcmp(z->data.name, x->data.name) < 0)
            ? x->left
            : x->right;
    }
    z->parent = y;
    if (!y)             *root = z;
    else if (strcmp(z->data.name, y->data.name) < 0) y->left = z;
    else                                             y->right = z;
    rb_insert_fixup(root, z);
}

static RBNode* rb_search(RBNode* root, const char* key) {
    if (!root) return NULL;
    int cmp = strcmp(key, root->data.name);
    if (cmp == 0) return root;
    return (cmp < 0)
        ? rb_search(root->left, key)
        : rb_search(root->right, key);
}

static void free_rbt(RBNode* root) {
    if (!root) return;
    free_rbt(root->left);
    free_rbt(root->right);
    free(root);
}


static void test_search_algorithms(int data_size) {
    Athlete* array = malloc(data_size * sizeof * array);
    BTNode* bst_root = NULL;
    RBNode* rbt_root = NULL;

    
    generate_random_athletes(array, data_size);

    
    for (int i = 0; i < data_size; i++) {
        bt_insert(&bst_root, array[i]);
        rb_insert(&rbt_root, array[i]);
    }

   
    char search_key[100];
    printf("\n=== Тест размера = %d ===\n", data_size);
    printf("Введите полное имя для поиска: ");
    getchar(); 
    fgets(search_key, sizeof search_key, stdin);
    search_key[strcspn(search_key, "\n")] = '\0';

    const int iterations = 1;
    double t_lin, t_bst, t_rbt;
    double start, end;

    
    start = get_current_time();
    for (int i = 0; i < iterations; i++) {
        linear_search(array, data_size, search_key);
    }
    end = get_current_time();
    t_lin = (end - start) / iterations;

    
    start = get_current_time();
    for (int i = 0; i < iterations; i++) {
        bt_search(bst_root, search_key);
    }
    end = get_current_time();
    t_bst = (end - start) / iterations;

    
    start = get_current_time();
    for (int i = 0; i < iterations; i++) {
        rb_search(rbt_root, search_key);
    }
    end = get_current_time();
    t_rbt = (end - start) / iterations;

    printf("Линейный поиск: время = %.6f мс/операцию\n", t_lin);
    printf("Поиск BST:      время = %.6f мс/операцию\n", t_bst);
    printf("Поиск RBT:      время = %.6f мс/операцию\n", t_rbt);

    free(array);
    free_bt(bst_root);
    free_rbt(rbt_root);
}


int main(void) {
    setlocale(LC_ALL, "rus");
    srand((unsigned)time(NULL));

    int option;
    do {
        printf(
            "\nМеню:\n"
            "1. Тест на 100 спортсменах\n"
            "2. Тест на 1,000 спортсменах\n"
            "3. Тест на 10,000 спортсменах\n"
            "4. Тест на 50,000 спортсменах\n"
            "5. Тест на 100,000 спортсменах\n"
            "6. Тест на 200,000 спортсменах\n"
            "7. Тест на 300,000 спортсменах\n"
            "8. Запустить все тесты последовательно\n"
            "9. Тест на 1 000 000 спортсменах\n"
            "0. Выход\n"
            "Выберите: "
        );
        scanf("%d", &option);
        switch (option) {
        case 1:  test_search_algorithms(100);     break;
        case 2:  test_search_algorithms(1000);    break;
        case 3:  test_search_algorithms(10000);   break;
        case 4:  test_search_algorithms(50000);   break;
        case 5:  test_search_algorithms(100000);  break;
        case 6:  test_search_algorithms(200000);  break;
        case 7:  test_search_algorithms(300000);  break;
        case 8:
            test_search_algorithms(100);
            test_search_algorithms(1000);
            test_search_algorithms(10000);
            test_search_algorithms(50000);
            test_search_algorithms(100000);
            test_search_algorithms(200000);
            test_search_algorithms(300000);
            break;
        case 9: test_search_algorithms(1000000); break;
        case 0:
            printf("До свидания!\n");
            break;
        default:
            printf("Неверный вариант, попробуйте снова.\n");
        }
    } while (option != 0);

    return 0;
}