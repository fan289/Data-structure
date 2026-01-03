#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>

#define TOP_N 5
#define INFINITY_WEIGHT 999999.0

#define MAX_WORD_LEN 64
#define MAX_POS_LEN 16
#define TABLE_SIZE 400009
// #define OK 1
// #define ERROR 0
// #define INITSIZE 10000
#define dict_file "C:\\Users\\luo yifan\\Desktop\\GSAI-2th-mystudy\\Data_structure\\labs\\lab4\\dict.txt"
#define big_dict_file "C:\\Users\\luo yifan\\Desktop\\GSAI-2th-mystudy\\Data_structure\\labs\\lab4\\dict_big.txt"


typedef struct WordNode {
    char word[MAX_WORD_LEN];
    int freq;
    char pos[MAX_POS_LEN];
    struct WordNode *next;
} WordNode;

typedef struct {
    WordNode* buckets[TABLE_SIZE];
    int total_count;
    long total_freq;
} WordDict;

// 经典的 djb2 哈希算法
unsigned long hash_func(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % TABLE_SIZE; // 保证结果在 0 ~ TABLE_SIZE-1 之间
}

// 初始化字典
WordDict* init_dict() {
    WordDict* dict = (WordDict*)malloc(sizeof(WordDict));
    if (!dict) return NULL;
    
    for (int i = 0; i < TABLE_SIZE; i++) {
        dict->buckets[i] = NULL;
    }
    dict->total_count = 0;
    return dict;
}

// 插入词
void insert_word(WordDict* dict, char* word, int freq, char* pos) {
    unsigned long index = hash_func(word);
    WordNode* new_node = (WordNode*)malloc(sizeof(WordNode));
    strcpy(new_node->word, word);
    new_node->freq = freq;
    strcpy(new_node->pos, pos);
    new_node->next = dict->buckets[index];
    dict->buckets[index] = new_node;
    dict->total_count++;
    dict->total_freq += freq;
}

int get_word_freq(WordDict* dict, char* word) {
    unsigned long index = hash_func(word);
    WordNode* current = dict->buckets[index];
    
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            return current->freq;
        }
        current = current->next;
    }
    return -1;
}

void free_dict(WordDict* dict) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        WordNode* current = dict->buckets[i];
        while (current != NULL) {
            WordNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(dict);
}


// 线性表存储词典，效率低
// 
// typedef struct word{
//     char word[MAX_WORD_LEN];
//     int freq;
//     char pos[MAX_POS_LEN];
// }word,*word_ptr;

// typedef struct word_dict{
//     word_ptr* word_list;
//     int size;
//     int len;
// }word_dict;

// int init_word_dict(word_dict* word_dict){
//     if(!word_dict)return ERROR;
//     word_dict->word_list=(word_ptr*)malloc(sizeof(word_ptr)*INITSIZE);
//     if(!word_dict->word_list)return ERROR;
//     word_dict->size=INITSIZE;
//     word_dict->len=0;
//     return OK;
// }

// int reinit_word_dict(word_dict* word_dict){
//     if(!word_dict)return ERROR;
//     word_dict->size*=2;
//     word_ptr* new_word_list=(word_ptr*)realloc(word_dict->word_list,sizeof(word_ptr)*word_dict->size);
//     if(!new_word_list)return ERROR;
//     word_dict->word_list=new_word_list;
//     return OK;
// }

// void append(word_dict* word_dict,word_ptr a_word){
//     if(!word_dict||!a_word)return;
//     if(word_dict->size<=word_dict->len){
//         int state=reinit_word_dict(word_dict);
//         if(!state)perror("内存重开辟失败\n");
//     }
//     word_dict->word_list[word_dict->len++]=a_word;
// }

// void free_dict(word_dict* dict){
//     if(!dict)return;
//     for(int i=0;i<dict->len;i++){
//         free(dict->word_list[i]);
//     }
//     free(dict->word_list);
//     free(dict);
// }

// word_dict* load_dict(char* dict_path){
//     FILE* fp=fopen(dict_path,"r");
//     if(!fp){
//         perror("词表文件打开失败\n");
//         return NULL;
//     }
//     word_dict* my_dict=(word_dict*)malloc(sizeof(word_dict));
//     int state=init_word_dict(my_dict);
//     if(!state){
//         perror("内存开辟失败，词典构建失败\n");
//         return NULL;
//     }
//     char temp_word[MAX_WORD_LEN];
//     int temp_freq;
//     char temp_pos[MAX_POS_LEN];
//     while(fscanf(fp, "%s %d %s", temp_word, &temp_freq, temp_pos) != EOF){
//         word_ptr a_word=(word_ptr)malloc(sizeof(word));
//         if (!a_word) {
//             perror("单个词条内存分配失败");
//             break; 
//         }
//         strcpy(a_word->word,temp_word);
//         a_word->freq=temp_freq;
//         strcpy(a_word->pos,temp_pos);
//         append(my_dict,a_word);
//     }
//     fclose(fp);
//     printf("词典加载完成，共 %d 个词条\n", my_dict->len);
//     return my_dict;
// }
// 
// int get_word_freq(word_dict* dict, char* key) {
//     for (int i = 0; i < dict->len; i++) {
//         if (strcmp(dict->word_list[i]->word, key) == 0) {
//             return dict->word_list[i]->freq;
//         }
//     }
//     return -1;
// }

int get_utf8_len(char c) {
    if ((c & 0x80) == 0) return 1;        // ASCII (英文、数字、半角标点)
    if ((c & 0xE0) == 0xC0) return 2;     // 2字节字符
    if ((c & 0xF0) == 0xE0) return 3;     // 常用汉字、中文标点
    if ((c & 0xF8) == 0xF0) return 4;     // 4字节字符 (Emoji等)
    return 1; // 异常情况默认为1，防止死循环
}

WordDict* load_dict(char* dict_path) {
    FILE* fp = fopen(dict_path, "r");
    if (!fp) {
        perror("词表文件打开失败");
        return NULL;
    }

    WordDict* dict = init_dict();
    if (!dict) return NULL;

    char temp_word[MAX_WORD_LEN];
    int temp_freq;
    char temp_pos[MAX_POS_LEN];

    // 读取并插入哈希表
    while (fscanf(fp, "%s %d %s", temp_word, &temp_freq, temp_pos) != EOF) {
        insert_word(dict, temp_word, temp_freq, temp_pos);
    }

    fclose(fp);
    printf("哈希词典加载完成，共 %d 个词\n", dict->total_count);
    return dict;
}

// 在 build_graph 中，你现在可以直接调用新的查找函数
// int freq = get_word_freq(dict, temp_word);

typedef struct Graph{
    double **adj;
    int nodes_num;
    char *source;
}Graph;

Graph* build_graph(char* s, WordDict* dict) {
    int len = strlen(s);
    Graph* G = (Graph*)malloc(sizeof(Graph));
    G->nodes_num = len + 1;
    G->source = strdup(s);

    G->adj = (double**)malloc(sizeof(double*) * G->nodes_num);
    for (int i = 0; i < G->nodes_num; i++) {
        G->adj[i] = (double*)malloc(sizeof(double) * G->nodes_num);
        for (int j = 0; j < G->nodes_num; j++) {
            G->adj[i][j] = INFINITY_WEIGHT;
        }
    }

    char temp_word[MAX_WORD_LEN];
    long total_freq = dict->total_freq > 0 ? dict->total_freq : 1;

    for (int i = 0; i < len; i++) {
        int char_len = get_utf8_len(s[i]);
        if (i + char_len <= len) {G->adj[i][i + char_len] = 20.0; }

        for (int j = i + 1; j <= len && (j - i) < MAX_WORD_LEN; j++) {
            int sub_len = j - i;
            if (sub_len < char_len) continue;
            strncpy(temp_word, s + i, sub_len);
            temp_word[sub_len] = '\0';
            int freq = get_word_freq(dict, temp_word);
            if (freq > 0) {
                G->adj[i][j] = log((double)total_freq) - log((double)freq);
            } 
        }
    } 
    return G;
}

void free_graph(Graph* G) {
    if (!G) return;
    for (int i = 0; i < G->nodes_num; i++) {
        free(G->adj[i]);
    }
    free(G->adj);
    free(G->source);
    free(G);
}

void print_segmentation(Graph* G, int* parent) {
    int path[100]; // 假设分词数量不超过100个
    int count = 0;
    int curr = G->nodes_num - 1;
    
    // 如果终点不可达
    if (parent[curr] == -1) {
        printf("分词失败：无法找到完整路径。\n");
        return;
    }

    while (curr != 0) {
        path[count++] = curr;
        curr = parent[curr];
    }
    path[count++] = 0;

    printf("分词结果: ");
    
    for (int i = count - 1; i > 0; i--) {
        int start = path[i];
        int end = path[i - 1];
        for (int k = start; k < end; k++) {
            printf("%c", G->source[k]);
        }
        if (i > 1) printf("/"); 
    }
    printf("\n");
}

void dijkstra(Graph* G) {
    int n = G->nodes_num;
    
    double* dist = (double*)malloc(n * sizeof(double));
    int* parent = (int*)malloc(n * sizeof(int));
    int* visited = (int*)malloc(n * sizeof(int));

    if (!dist || !parent || !visited) {
        perror("Dijkstra 内存分配失败");
        return;
    }
    for (int i = 0; i < n; i++) {
        dist[i] = INFINITY_WEIGHT;
        parent[i] = -1;
        visited[i] = 0;
    }

    dist[0] = 0.0;

    for (int i = 0; i < n; i++) {
        int u = -1;
        double min_dist = INFINITY_WEIGHT;
        
        for (int j = 0; j < n; j++) {
            if (!visited[j] && dist[j] < min_dist) {
                min_dist = dist[j];
                u = j;
            }
        }

        if (u == -1 || dist[u] == INFINITY_WEIGHT) break;
        
        visited[u] = 1;

        // 在我们的邻接矩阵中，只需要遍历 v > u 的情况 (因为是有向无环图 DAG)
        for (int v = u + 1; v < n; v++) {
            if (G->adj[u][v] < INFINITY_WEIGHT) {
                double new_dist = dist[u] + G->adj[u][v];
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    parent[v] = u;
                }
            }
        }
    }
    print_segmentation(G, parent);
    free(dist);
    free(parent);
    free(visited);
}

// 路径状态结构体
typedef struct {
    double cost;      // 路径总权重
    int parent_idx;   // 前驱节点索引 (从哪个字跳过来的)
    int parent_rank;  // 前驱节点的排名 (延续了前驱节点的第几条路径)
} PathState;

// 用于排序的临时结构
typedef struct {
    double cost;
    int from_node;
    int from_rank;
} Candidate;

// 比较函数，用于 qsort 排序 (升序)
int compare_candidates(const void* a, const void* b) {
    Candidate* c1 = (Candidate*)a;
    Candidate* c2 = (Candidate*)b;
    if (c1->cost < c2->cost) return -1;
    if (c1->cost > c2->cost) return 1;
    return 0;
}

void n_shortest_path(Graph* G) {
    int n = G->nodes_num;
    // dp[i][k] 表示到达节点 i 的第 k 短路径
    PathState **dp = (PathState**)malloc(n * sizeof(PathState*));
    for(int i = 0; i < n; i++) {
        dp[i] = (PathState*)malloc(TOP_N * sizeof(PathState));
        for(int k = 0; k < TOP_N; k++) {
            dp[i][k].cost = INFINITY_WEIGHT;
            dp[i][k].parent_idx = -1;
            dp[i][k].parent_rank = -1;
        }
    }
    dp[0][0].cost = 0.0;

    for (int i = 1; i < n; i++) {
        Candidate candidates[MAX_WORD_LEN * TOP_N + 100]; 
        int cand_count = 0;
        int start_j = (i - MAX_WORD_LEN > 0) ? (i - MAX_WORD_LEN) : 0;
        
        for (int j = start_j; j < i; j++) {
            if (G->adj[j][i] < INFINITY_WEIGHT) {
                double edge_weight = G->adj[j][i];
                for (int k = 0; k < TOP_N; k++) {
                    if (dp[j][k].cost < INFINITY_WEIGHT) {
                        candidates[cand_count].cost = dp[j][k].cost + edge_weight;
                        candidates[cand_count].from_node = j;
                        candidates[cand_count].from_rank = k;
                        cand_count++;
                    }
                }
            }
        }

        if (cand_count > 0) {
            qsort(candidates, cand_count, sizeof(Candidate), compare_candidates);
            
            int limit = (cand_count < TOP_N) ? cand_count : TOP_N;
            for (int k = 0; k < limit; k++) {
                dp[i][k].cost = candidates[k].cost;
                dp[i][k].parent_idx = candidates[k].from_node;
                dp[i][k].parent_rank = candidates[k].from_rank;
            }
        }
    }

    printf("N-最短路径分词结果(Top %d)\n", TOP_N);
    
    // 从终点 n-1 开始回溯每一条路径
    int end_node = n - 1;
    for (int k = 0; k < TOP_N; k++) {
        if (dp[end_node][k].cost >= INFINITY_WEIGHT) break; // 不足 N 条路径

        printf("Result %d: ", k + 1);
        
        // 回溯单条路径
        int path_nodes[256];
        int count = 0;
        int curr_node = end_node;
        int curr_rank = k;

        while (curr_node != 0) {
            path_nodes[count++] = curr_node;
            
            PathState state = dp[curr_node][curr_rank];
            int next_node = state.parent_idx;
            int next_rank = state.parent_rank;
            
            curr_node = next_node;
            curr_rank = next_rank;
        }
        path_nodes[count++] = 0;

        for (int p = count - 1; p > 0; p--) {
            int start = path_nodes[p];
            int end = path_nodes[p-1];
            for (int char_idx = start; char_idx < end; char_idx++) {
                printf("%c", G->source[char_idx]);
            }
            if (p > 1) printf("/");
        }
        printf("\n");
    }
    for(int i = 0; i < n; i++) free(dp[i]);
    free(dp);
}

int main(){
    WordDict* my_dict=load_dict(big_dict_file);
    while(1){
        printf("请输入中文文本\n");
        printf("> ");
        char s[500];
        scanf("%s",s);
        Graph* G=build_graph(s,my_dict);
        n_shortest_path(G);
        // dijkstra(G);
        free_graph(G);
        printf("\n");
    }  
}