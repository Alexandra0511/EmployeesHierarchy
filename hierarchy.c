#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hierarchy.h"


/* Adauga un angajat nou in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 * manager_name: numele sefului noului angajat
 *
 * return: intoarce ierarhia modificata. Daca tree si manager_name sunt NULL, 
           atunci employee_name e primul om din ierarhie.
 */
Tree search_node(Tree tree, char *name) //intoarce pointer la angajatul cu numele "name"
{
    if(strcmp(tree->name, name) == 0)
        return tree;

    Tree p;
    for(int i = 0; i < tree->direct_employees_no; i++)
    {
        if(strcmp(tree->team[i]->name, name) == 0)
            return tree->team[i];
        p = search_node(tree->team[i], name);
        if(p != NULL)
            return p;
    }
    return NULL;
    
}    
Tree addInTeam(Tree tree, Tree employee)
{
    int poz = 0;
    if(tree->direct_employees_no == 0)
    {
        tree->team[0] = employee;
        tree->direct_employees_no++;
        return tree;
    }

    while(poz < tree->direct_employees_no) //adaug angajatul in echipa pastrand vectorul sortat
    {
        if(strcmp(employee->name, tree->team[poz]->name) > 0)
            poz++;
        else break;
    }
    for(int i = tree->direct_employees_no-1; i >= poz; i--)
        tree->team[i+1] = tree->team[i];
    tree->team[poz] = employee;
    tree->direct_employees_no++;
    return tree; 
}
   
Tree hire(Tree tree, char *employee_name, char *manager_name) {
    
    TreeNode* newemployee = (TreeNode*) malloc(sizeof(TreeNode));
    newemployee->name = malloc(20*sizeof(char));
    strcpy(newemployee->name, employee_name);
    newemployee->manager = NULL;
    newemployee->team = (TreeNode**) malloc(100*sizeof(TreeNode*));
    newemployee->direct_employees_no = 0;
    
    if(tree == NULL && manager_name == NULL)
    {
        tree = newemployee;
        return tree;
    }

    Tree manager = search_node(tree, manager_name);
    newemployee->manager = manager;
    manager = addInTeam(manager, newemployee);
    return tree;
}

/* Sterge un angajat din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului concediat
 *
 * return: intoarce ierarhia modificata.
 */
Tree del(Tree tree, Tree node) //sterge angajatul din echipa managerului
{
    for(int i = 0; i < tree->direct_employees_no; i++)
    {
        if(tree->team[i] == node)
        {
            for(int j = i; j < tree->direct_employees_no-1; j++)
                tree->team[j] = tree->team[j+1];
            tree->direct_employees_no--;
            
            return tree;
        }
    }
    return tree;
}
Tree fire(Tree tree, char *employee_name) {
    
    if(strcmp(tree->name, employee_name) == 0)
        return tree;
    Tree p = search_node(tree, employee_name);
    if(p == NULL)
        return tree;

    Tree manager = p->manager;
    for(int i = 0; i < p->direct_employees_no; i++) //mut echipa celui concediat
    {
        p->team[i]->manager = manager;
        manager = addInTeam(manager, p->team[i]);
    }

    manager = del(manager, p); //sterg angajatul din echipa
    free(p->team); 
    free(p->name);
    free(p);
    return tree;
}

/* Promoveaza un angajat in ierarhie. Verifica faptul ca angajatul e cel putin 
 * pe nivelul 2 pentru a putea efectua operatia.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 *
 * return: intoarce ierarhia modificata.
 */
Tree promote(Tree tree, char *employee_name) {
    
    Tree p = search_node(tree, employee_name);
    Tree manager = p->manager;
    if(p->manager == NULL || manager->manager == NULL)
        return tree;

    for(int i = 0; i < p->direct_employees_no; i++) //mut echipa angajatului
    {
        p->team[i]->manager = manager;
        manager = addInTeam(manager, p->team[i]);
    }

    manager = del(manager, p); //il sterg din echipa precedenta
    p->manager = p->manager->manager;
    p->manager = addInTeam(p->manager, p);//il adaug in noua echipa 
    p->direct_employees_no = 0;
    return tree;
}

/* Muta un angajat in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_employee(Tree tree, char *employee_name, char *new_manager_name) {
    Tree p = search_node(tree, employee_name);
    Tree manager = p->manager;
    Tree newmanager = search_node(tree, new_manager_name);

    if(p->manager == NULL || manager == newmanager)
        return tree;

    for(int i=0; i<p->direct_employees_no; i++)
    {
        p->team[i]->manager = manager;
        manager=addInTeam(manager, p->team[i]);
    }

    manager = del(manager, p);
    p->manager = newmanager;
    p->manager = addInTeam(p->manager, p);
    p->direct_employees_no = 0;
    return tree;
    
}

/* Muta o echipa in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei mutate
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_team(Tree tree, char *employee_name, char *new_manager_name) {
    
    Tree p = search_node(tree, employee_name);
    Tree manager = p->manager;
    Tree newmanager = search_node(tree, new_manager_name);
    
    if(p->manager == NULL)
        return tree;

    p->manager = newmanager;
    newmanager = addInTeam(newmanager, p);
    manager = del(manager, p);
    return tree;
}

/* Concediaza o echipa din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei concediate
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire_team(Tree tree, char *employee_name) {
    Tree p = search_node(tree, employee_name);
    
    if(p->manager == NULL)
        return tree;

    while(p->direct_employees_no != 0) //sterg echipa
    {
        p = fire(p, p->team[0]->name);
    }
    tree = fire(tree, p->name); //apoi sterg angajatul din varf
    return tree;
}

/* Afiseaza toti angajatii sub conducerea unui angajat.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei
 */

void get_by_manager_rec(Tree tree, char v[30][30], int *ct) //creaza vectorul cu angajatii de sub manager
{
    if(tree == NULL)
        return;
    for(int i = 0; i < tree->direct_employees_no; i++)
    {
        strcpy(v[*ct], tree->team[i]->name);
        *ct = *ct + 1;
        get_by_manager_rec(tree->team[i], v, ct);
    }

}
void get_employees_by_manager(FILE *f, Tree tree, char *employee_name) {
    
    char v[30][30]; //vectorul de angajati
    
    Tree p = search_node(tree, employee_name);
    
    if(p==NULL)
    {
        fprintf(f, "\n");
        return;
    }

    int *ct = malloc(sizeof(int)); //dimensiunea vectorului
    *ct = 0;
    get_by_manager_rec(p, v, ct);
    strcpy(v[*ct], p->name); //adaug angajatul din varf
    *ct = *ct + 1; //contorizez dimensiunea vectorului
    int ok = 0;
    do //sortez vectorul
    {
        ok = 0;
        for(int i = 0; i < *ct - 1; i++)
        {
            if(strcmp(v[i], v[i + 1]) > 0)
            {
                char aux[30];
                strcpy(aux, v[i + 1]);
                strcpy(v[i + 1], v[i]);
                strcpy(v[i], aux);
                ok = 1;
            }
        }
    } while (ok);
    for(int i = 0; i < *ct; i++) //afisarea
        fprintf(f, "%s ", v[i]);
    fprintf(f, "\n");
    free(ct); //eliberarea memoriei pentru contor
    return;
}

/* Afiseaza toti angajatii de pe un nivel din ierarhie.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * level: nivelul din ierarhie
 */
void get_by_level_rec(Tree tree, char v[30][30], int *ct, int level)
{
    if(tree == NULL)
        return;
    
    for(int i = 0; i < tree->direct_employees_no; i++)
    {
        if(level == 0) //ma aflu pe nivelul care trebuie
        {
            strcpy(v[*ct], tree->team[i]->name);
            *ct = *ct + 1;
        }
        else get_by_level_rec(tree->team[i], v, ct, level - 1); //altfel cobor in ierarhie
    }

}
void get_employees_by_level(FILE *f, Tree tree, int level) {
   
   if(level < 0) //cazul pentru nivel inexistent
   {
       fprintf(f, "\n");
       return;
   }
   if(level == 0) //cazul pentru radacina
   {
       fprintf(f, "%s \n", tree->name);
       return;
   }
   char v[30][30];
   int *ct = malloc(sizeof(int));
    *ct = 0;
    get_by_level_rec(tree, v, ct, level-1); //sarim nivelul radacinii
    int ok = 0;
    do
    {
        ok = 0;
        for(int i = 0; i < *ct - 1; i++)
        {
            if(strcmp(v[i], v[i + 1])>0)
            {
                char aux[30];
                strcpy(aux, v[i + 1]);
                strcpy(v[i + 1], v[i]);
                strcpy(v[i], aux);
                ok=1;
            }
        }
    } while (ok);
    for(int i = 0; i < *ct; i++)
        fprintf(f, "%s ", v[i]);
    fprintf(f, "\n");
    free(ct);
    return;
}

/* Afiseaza angajatul cu cei mai multi oameni din echipa.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void get_manager_rec(Tree tree, char v[30][30], int *ct, int *max)
{
    if(tree == NULL)
        return;
    for(int i = 0; i < tree->direct_employees_no; i++)
    {
        get_manager_rec(tree->team[i], v, ct, max);
    }
    if(tree->direct_employees_no > *max) //numar mai mare de angajati
    {
        *max = tree->direct_employees_no; //modific max-ul
        strcpy(v[0], tree->name); //suprascriu vectorul
        *ct = 1; //dimensiunea ramane constanta
    }    
    else if(tree->direct_employees_no == *max) //numar egal de angajati
    {
        strcpy(v[*ct], tree->name); //adaug angajatul in vector
        *ct = *ct + 1; //contorizez dimensiune
    }
}
void get_best_manager(FILE *f, Tree tree) {
    char v[30][30];
    int *ct = malloc(sizeof(int));
    int *max = malloc(sizeof(int));
    *ct = 0;
    *max = 0;
    get_manager_rec(tree, v, ct, max);

    int ok = 0;
    do
    {
        ok=0;
        for(int i = 0; i < *ct - 1; i++)
        {
            if(strcmp(v[i], v[i + 1]) > 0)
            {
                char aux[30];
                strcpy(aux, v[i + 1]);
                strcpy(v[i + 1], v[i]);
                strcpy(v[i], aux);
                ok = 1;
            }
        }
    } while (ok);
    for(int i = 0; i < *ct; i++)
        fprintf(f, "%s ", v[i]);
    fprintf(f, "\n");
    free(ct);
    free(max);
    return;
}

/* Reorganizarea ierarhiei cu un alt angajat in varful ierarhiei.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului care trece in varful ierarhiei
 */

Tree reorganize(Tree tree, char *employee_name) {
    Tree p = search_node(tree, employee_name);
    if(p == NULL || p == tree)
        return tree;
    
    Tree m = p->manager;
    Tree node = p;
    
   
    while(m != tree)
    {
        Tree mmanager = m->manager;
        m = del(m, node); //rup legatura managerului cu arborele
        node = addInTeam(node, m); // adaug managerul sub angajatul sau anterior
        m->manager = node; //actualizez legaturile
        node = m;
        m = mmanager;
    }
    //in final adaug root-ul
    m = del(m, node);
    m->manager = node;
    node = addInTeam(node, m);
    p->manager = NULL; //actualizez managerul angajatului din varf
    return p;
    
}

/* Parcurge ierarhia conform parcurgerii preordine.
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void preorder_rec(FILE *f, Tree tree)
{
    if(tree == NULL)
        return;
    for(int i = 0; i < tree->direct_employees_no; i++)
    {
        fprintf(f, "%s-%s ", tree->team[i]->name, tree->name);
        preorder_rec(f, tree->team[i]);
    }

}
void preorder_traversal(FILE *f, Tree tree) {
    
    fprintf(f, "%s ", tree->name);
    preorder_rec(f, tree);
    fprintf(f, "\n");
    return;
}

/* Elibereaza memoria alocata nodurilor din arbore
 *
 * tree: ierarhia existenta
 */
void destroy_tree(Tree tree) {
    Tree p = tree;
    for(int i = 0; i < p->direct_employees_no; i++)
    {
        destroy_tree(p->team[i]);
    }
    free(p->team);
    free(p->name);
    free(p);

    return;
}

