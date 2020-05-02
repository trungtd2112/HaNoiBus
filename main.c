#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <stdio_ext.h>
#include "jrb.h"
#include "dllist.h"

#define INFINITIVE_VALUE  10000000

typedef struct {
    JRB edges; //cạnh
    JRB vertices; //đỉnh
    JRB normal_edges;
} Graph;

typedef struct{
    JRB ten_tuyen;
    int weight;

} data;

int so_ben = 0;

Graph createGraph();
void addVertex(Graph g, char *key, int val);
int getVertex(Graph graph, char *key);
void addEdge(Graph graph, char *v1, char *v2, char *cur_tuyen);
int getEdgeValue(Graph graph, char *v1, char *v2, char *cur_tuyen);
int indegree(Graph graph, int v, int* output);
void getName(Graph g, int val, char *name);
double shortestPath(Graph g, char *start, char *target, int* path, int*length);
void dropGraph(Graph graph);
void DocFile(FILE *f, Graph graph);
void display_dinh(Graph g);
void display_canh(Graph g, char* tenBen);

void themCanh(Graph graph, int v1, int v2, double weight);
double layGiaTriCanh(Graph graph, int v1, int v2);
int layDinhKe(Graph graph, int v, int* output);
double ngan_nhat(Graph graph, int s, int t, int *path, int *length);



int main()
{
	FILE *f;
	data *p; //khoi tao con tro tro den cau truc luu trong so va cac tuyen di qua 1 ben
	p = malloc(sizeof(data));
	p->weight = 0;
	p->ten_tuyen = make_jrb();
	JRB node_ben_1, node_ben_2, node_ten_tuyen, tree;
	JRB cay_luu_tuyen = make_jrb();//cay nay de luu cac tuyen bus di qua 1 ben

	Graph g = createGraph();
	char start[50];
	char target[50];
	char findBS[50];
	char ten_ben_1[50];
	char ten_ben_2[50];

	int i, length, path[100], s, t;
	double w;
	f = fopen("input1.txt","r");
	DocFile(f,g);
	int choice;
	display_dinh(g);
	do
    {
        printf("\n1) Find information of 1 bus stop: \n2) Find path: \n3) Exit \nWhat do you want? ");
        scanf("%d",&choice);
        if (choice < 1 || choice > 3) printf("Try Again\n");
        switch (choice)
        {
            case 1: printf("Input name: ");
                    __fpurge(stdin);
                    gets(findBS);
                    __fpurge(stdin);
                    display_canh(g, findBS);
                    findBS[0] = '\0';
                    break;
            case 2: printf("Input start: ");
                    __fpurge(stdin);
                    gets(start);
                    __fpurge(stdin);
                    printf("Input target: ");
                    __fpurge(stdin);
                    gets(target);
                    __fpurge(stdin);
                    s = getVertex(g, strdup(start));
                    t = getVertex(g, strdup(target));
                    w = ngan_nhat(g, s, t, path, &length);
                    if (w != INFINITIVE_VALUE){
                        printf("Shortest distance from s --> t: %lf\n", w);
                        printf("Path: ");
                        for (int i = length-1; i >= 0; i--){
                            getName(g, path[i], ten_ben_1);
                            printf("%s ", ten_ben_1);
                            int kt_dung = i - 1;
                            if (kt_dung >= 0)
                            {
                                getName(g, path[kt_dung], ten_ben_2);
                                printf("=>");
                                node_ben_1 = jrb_find_str(g.edges, strdup(ten_ben_1));
                                tree = (JRB) jval_v(node_ben_1->val);
                                node_ben_2 = jrb_find_str(tree, strdup(ten_ben_2));
                                p = (data*) node_ben_2->val.v;
                                cay_luu_tuyen = p->ten_tuyen;//cay_luu_tuyen da make_jrb() roi nen khong can ep kieu khi gan nua
                                printf("(Bus route: ");
                                jrb_traverse(node_ten_tuyen, cay_luu_tuyen)
                                {
                                    printf("%s ", jval_s(node_ten_tuyen->key));
                                }
                                printf(")");
                            }
                        }
                    printf("\n");
                    }
                    else{
                        printf("No Path from s --> t\n");
                    }
                    start[0] = '\0';
                    target[0] = '\0';
                    break;
            }
    } while (choice != 3);


	fclose(f);
	dropGraph(g);
}

Graph createGraph()
{
	Graph g;
	g.edges = make_jrb();
	g.vertices = make_jrb();
	g.normal_edges = make_jrb();
	return g;
}

void addVertex(Graph g, char *key, int val)
{
	JRB node = jrb_find_str(g.vertices, strdup(key));
	if (node==NULL) // only add new vertex
		jrb_insert_str(g.vertices, strdup(key), new_jval_i(val));
}

int getVertex(Graph g, char *key) //lay dinh
{
	JRB node = jrb_find_str(g.vertices, strdup(key)); //tim dinh trong graph
	if (node == NULL)
		return -9999;
	else
		return jval_i(node->val); //return so ung voi key
}

void addEdge(Graph graph, char *v1, char *v2, char *cur_tuyen)//ham then chot 2
{
	JRB node, node_find_v2, tree;
	if (getEdgeValue(graph, v1, v2, cur_tuyen) == 0)
	{
		node = jrb_find_str(graph.edges, strdup(v1));//tim xem v1 co chua
		if (node==NULL) {//chua co thi tao cay moi, luc nay v2 chac chan chua co trong cay cua v1 => gan value moi toanh cho v2
			tree = make_jrb();
			jrb_insert_str(graph.edges, strdup(v1), new_jval_v(tree));
			data *p;
			p = malloc(sizeof(data));
			p->weight = 1;
			p->ten_tuyen = make_jrb();
			jrb_insert_str(p->ten_tuyen, strdup(cur_tuyen), new_jval_s(strdup(cur_tuyen)));
			jrb_insert_str(tree, strdup(v2), new_jval_v((void*)p));
		} else//co roi thi gan cay con vao bien tree
		{
			tree = (JRB) jval_v(node->val);
			node_find_v2 = jrb_find_str(tree, strdup(v2));
			if (node_find_v2 == NULL)//v2 chua co trong cay con cua v1 => gan value moi toanh cho v2
			{
			    data *p;
                p = malloc(sizeof(data));
                p->weight = 1;
                p->ten_tuyen = make_jrb();
                jrb_insert_str(p->ten_tuyen, strdup(cur_tuyen), new_jval_s(strdup(cur_tuyen)));
                jrb_insert_str(tree, strdup(v2), new_jval_v((void*)p));
			}
			else //v2 co nhung trong value cua v2 chua co ten_tuyen dinh truyen
            {
                data *val_of_v2;
                val_of_v2 = malloc(sizeof(data));
                val_of_v2 = (data*) node_find_v2->val.v; //gan value cua v2 vao val_of_v2
                jrb_insert_str(val_of_v2->ten_tuyen, strdup(cur_tuyen), new_jval_s(strdup(cur_tuyen)));
            }
		}
	}
}

int getEdgeValue(Graph graph, char *v1, char *v2, char *cur_tuyen)//ham then chot 1
{
	JRB node, tree;
	node = jrb_find_str(graph.edges, strdup(v1));
	if (node==NULL)
		return 0;
	tree = (JRB) jval_v(node->val);
	node = jrb_find_str(tree, strdup(v2));
	if (node==NULL)
		return 0;
	if(node != NULL) //da co v2, tim xem ten_tuyen da co trong data->ten_tuyen chua?
    {
        data *p;
        p = malloc(sizeof(data));
        p = (data*) node->val.v;
        JRB tmp;
        tmp = jrb_find_str(p->ten_tuyen, strdup(cur_tuyen));
        if(tmp == NULL)
            return 0;
    }
    return 1;
}

int indegree (Graph graph, int v, int* output)//ham nay khong dung den
{
	JRB tree, node;
	int total = 0;
	jrb_traverse(node, graph.edges)
	{
		tree = (JRB) jval_v(node->val);
		if (jrb_find_int(tree, v))
		{
			output[total] = jval_i(node->key);
			total++;
		}
	}
	return total;
}

void dropGraph(Graph graph)
{
	JRB node, tree;
	jrb_traverse(node, graph.edges)
	{
		tree = (JRB) jval_v(node->val);
		jrb_free_tree(tree);
	}
	jrb_traverse(node, graph.normal_edges)
	{
		tree = (JRB) jval_v(node->val);
		jrb_free_tree(tree);
	}
	jrb_free_tree(graph.normal_edges);
	jrb_free_tree(graph.edges);
	jrb_free_tree(graph.vertices);
}

void DocFile(FILE *f, Graph g)
{
    data *p;
    p = malloc(sizeof(data));
	f = fopen("input1.txt", "r");
	char tmp[100];
	fgets(tmp, 100, f);
	char ten_ben_1[100];
	ten_ben_1[0] = '\0';
	char ten_tuyen[100];
	char ten_ben_2[100];
	ten_ben_2[0] = '\0';
	while(tmp[0] != '?')
	{
		fscanf(f, "%s", ten_tuyen);
		fscanf(f, "%s", tmp);
		fscanf(f, "%s", tmp);
		fscanf(f, "%s", tmp);//doc cum dau tien cua ten ben
		while(1)
		{
			while(tmp[0] != '-' && tmp[0] != '.' && tmp[0] != '?')
			{
				strcat(ten_ben_1, tmp);
				strcat(ten_ben_1, " ");
				fscanf(f, "%s", tmp);

			}
            if( jrb_find_str(g.vertices, strdup(ten_ben_1)) == NULL)
            {
                addVertex(g, strdup(ten_ben_1), so_ben++);
            }

			tmp[0] = '\0';
			fscanf(f, "%s", tmp);
			while(tmp[0] != '-' && tmp[0] != '.' && tmp[0] != '?')
			{
				strcat(ten_ben_2, tmp);
				strcat(ten_ben_2, " ");
				fscanf(f, "%s", tmp);
			}
            if( jrb_find_str(g.vertices, strdup(ten_ben_2)) == NULL)
            {
                addVertex(g, strdup(ten_ben_2), so_ben++);
            }
			addEdge(g, ten_ben_1, ten_ben_2, ten_tuyen);
			themCanh(g, getVertex(g, strdup(ten_ben_1)), getVertex(g, strdup(ten_ben_2)), 1);

			if (tmp[0] == '.' || tmp[0] == '?')
			{
			    ten_tuyen[0] = '\0';
			    ten_ben_1[0] = '\0';
			    ten_ben_2[0] = '\0';
				break;
			}else
			{
				fseek(f, -((strlen(ten_ben_2) -1) + 2), SEEK_CUR);//vi luc nay ten_ben_2 co dau ' ' o cuoi
				fscanf(f, "%s", tmp);
			}
			ten_ben_1[0] = '\0';
			ten_ben_2[0] = '\0';
		}
	}
}

void getName(Graph g, int val, char *name) //truyen vao so, luu ten ung vs so vao bien name
{
    JRB node, tree;
    jrb_traverse(node, g.vertices) //duyet do thi g.dinh
    {
        char hehe[50];
        strcpy(hehe, jval_s(node->key));
        if( getVertex(g, jval_s(node->key)) == val)//so sanh value cua dinh va val truyen vao, neu bang nhau => val la value cua dinh
            {
                strcpy(name, jval_s(node->key)); //luu ten dinh vao name
                return;
            }
    }
    return;
}

void display_dinh(Graph g)
{
    JRB node;
    node = make_jrb();
    jrb_traverse(node, g.vertices)
    {
        printf("%s\n%d\n", node->key, node->val);
    }
}


void display_canh(Graph g, char* tenBen)
{
    JRB dinh_ke, cay_dinh_ke, tmp, cay_luu_tuyen, dinh_tuyen;
    cay_luu_tuyen = make_jrb();

    tmp = jrb_find_str(g.edges, strdup(tenBen));
    if (tmp == NULL)
        return;
    cay_dinh_ke = (JRB) jval_v(tmp->val);//tree gio la cay con cua Ben xe Gia Lam
    printf("\n%s: Can go to ... by bus line: ...,...v.v", tenBen);
        //duyet cay dinh ke
        jrb_traverse(dinh_ke, cay_dinh_ke)
        {
            printf("\n%s by bus line: ", jval_s(dinh_ke->key));
            data *p;
            p = malloc(sizeof(data));
            p->weight = 0;
            p->ten_tuyen = make_jrb();
            p = (data*) dinh_ke->val.v;
            cay_luu_tuyen = p->ten_tuyen;//ky la la ro rang p->ten-tuyen la cay ma gan the nay lai sai :(( "cay_luu_tuyen = (JRB) jval_v(p->ten_tuyen);"
             //duyet cay luu tuyen
            jrb_traverse(dinh_tuyen, cay_luu_tuyen)
            {
                printf("%s, ", jval_s(dinh_tuyen->key));
            }
       }
}

// Make connection between v1 and v2
void themCanh(Graph graph, int v1, int v2, double weight){
    JRB node, tree;
    if (layGiaTriCanh(graph, v1, v2) == INFINITIVE_VALUE)
    {
        node = jrb_find_int(graph.normal_edges, v1);
        if (node == NULL)
        {
            tree = make_jrb();
            jrb_insert_int(graph.normal_edges, v1, new_jval_v(tree));
        }
        else
        {
            tree = (JRB)jval_v(node->val);
        }
        jrb_insert_int(tree, v2, new_jval_d(weight));
    }
}

// Get weight from v1 to v2
double layGiaTriCanh(Graph graph, int v1, int v2){
    JRB node = jrb_find_int(graph.normal_edges, v1);
    if (node == NULL)
    {
        return INFINITIVE_VALUE;
    }
    JRB tree = (JRB)jval_v(node->val);
    JRB node1 = jrb_find_int(tree, v2);
    if (node1 == NULL) {
        return INFINITIVE_VALUE;
    }
    return jval_d(node1->val);
}

int layDinhKe(Graph graph, int v, int* output){
    JRB tree, node;
    int total;
    node = jrb_find_int(graph.normal_edges, v);
    if (node==NULL)
        return 0;
    tree = (JRB) jval_v(node->val);
    total = 0;
    jrb_traverse(node, tree){
        output[total] = jval_i(node->key);
        total++;
    }
    return total;
}

double ngan_nhat(Graph graph, int s, int t, int *path, int *length){
    // Khoi tao cac distance = 0
    double distance[1000], min;
    int previous[1000], u, visit[1000];

    for (int i=0; i<1000; i++){
        distance[i] = INFINITIVE_VALUE;
        visit[i] = 0;
        previous[i] = 0;
    }
    distance[s] = 0;
    previous[s] = s;
    visit[s] = 1;

    Dllist ptr, queue, node;
    queue = new_dllist();
    dll_append(queue, new_jval_i(s));

    // Duyet Queue
    while (!dll_empty(queue)){
        min = INFINITIVE_VALUE;
        dll_traverse(ptr, queue){
            // Lay ra min{distance}
            u = jval_i(ptr->val);
            if (min > distance[u]){
                min = distance[u];
                node = ptr;
            }
        }
        u = jval_i(node->val);
        visit[u] = 1;
        dll_delete_node(node);
        if (u == t) break;

        int output[100];
        int n = layDinhKe(graph, u, output);

        // Cap nhap distance cua tat ca cac dinh ma lien ke voi dinh min
        for (int i=0; i<n; i++){
            int v = output[i];
            double w = layGiaTriCanh(graph, u, v);
            if (distance[v] > distance[u] + w){
                distance[v] = distance[u] + w;
                previous[v] = u;
            }
            if (visit[v] == 0){
                dll_append(queue, new_jval_i(v));
            }
        }
    }

    // Truy vet lai de lay duong di tu s ---> t va luu trong path[]
    double distance_s_t = distance[t];
    int count=0;
    if (distance[t] != INFINITIVE_VALUE){
        path[count++] = t;
        while (t != s){
            t = previous[t];
            path[count++] = t;
        }
        *length = count;
    }
    return distance_s_t;
}
