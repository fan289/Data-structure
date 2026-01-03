#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//基础定义
#define OK 0
#define ERROR (-1)
typedef char* ElementType;

//一、实现顺序表
#define LIST_INIT_SIZE 100
#define LIST_INCREMENT 10

/*结构体定义*/
typedef struct {
    ElementType * list;
    int length;
    int size;
}SqList;

int InitList(SqList *l) {
    /*初始化顺序表*/
    if (l == NULL) return ERROR;
    l->list = (ElementType *)malloc(LIST_INIT_SIZE * sizeof(ElementType));
    if (!l->list) {
        return ERROR;
    }
    l->length = 0;
    l->size = LIST_INIT_SIZE;
    return OK;
}

int reInitList(SqList * l) {
    /*容量不够，重新初始化增大容量*/
    l->size += LIST_INCREMENT;
    ElementType *new_list = (ElementType *)realloc(l->list, l->size * sizeof(ElementType));
    if (!new_list) {
        return ERROR;
    }
    l->list = new_list;
    return OK;
}

void DestroyList(SqList * l) {
    /*销毁线性表*/
    for (int i = 0; i < l->length; i++) {
        free(l->list[i]);
    }
    free(l->list);
}

void ClearList(SqList * l) {
    /*清空线性表*/
    l->length = 0;
}

int Is_ListEmpty(SqList * l) {
    /*判断线性表是否为空*/
    return l->length == 0;
}

int GetElem(SqList * l, int i, ElementType * e) {
    /*获取线性表第i个数据*/
    if (i < 0 || i > l->length) {
        return ERROR;
    }
    if (e) {
        strcpy(*e, l->list[i]);
    }
    return OK;
}

int compare(ElementType a,ElementType b) {
    /*对比两个数据*/
    return strcmp(a,b);
}

int* LocateElem(SqList * l,ElementType elem) {
    /*定位elem在线性表中位置*/
    int count=0;
    for (int i = 0; i < l->length; i++) {
        if (compare(l->list[i],elem)==0) {
            count++;
        }
    }
    if (count==0) {
        return NULL;
    }
    int* pos=(int*)malloc(sizeof(int)*(count+1));
    int index=1;
    pos[0]=count;
    for (int i = 0; i < l->length; i++) {
        if (compare(l->list[i],elem)==0) {
            pos[index++]=i+1;
        }
    }
    return pos;
}

int PriorElem(SqList * l,ElementType cur_e,ElementType * pre_e) {
    /*找到数据cur_e的前驱*/
    int i=0;
    while (i < l->length) {
        if (compare(l->list[i],cur_e)==0) {
            break;
        }
        i++;
    }
    if (i!=0&&i!=l->length) {
        strcpy(*pre_e,l->list[i]);
        return 1;
    }
    return 0;
}

int NextElem(SqList * l,ElementType cur_e,ElementType * next_e) {
    /*找到数据cur_e的后继*/
    int i=0;
    while (i < l->length) {
        if (compare(l->list[i],cur_e)==0) {
            break;
        }
        i++;
    }
    if (i!=l->length-1&&i!=l->length) {
        strcpy(*next_e,l->list[i+1]);
        return 1;
    }
    return 0;
}

int ListInsert(SqList * l, int i, ElementType elem) {
    /*插入数据*/
    if (i<1||i>l->length+1) {
        return ERROR;
    }
    if (l->length==l->size) {
        reInitList(l);
    }
    for (int k=l->length;k>=i;k--) {
        l->list[k]=l->list[k-1];
    }
    l->list[i-1] = (char*)malloc((strlen(elem)+1) * sizeof(char));
    if (!l->list[i-1]) {
        return ERROR;
    }
    strcpy(l->list[i-1],elem);
    l->length++;
    return OK;
}

int ListDelete(SqList * l, int i, ElementType * elem) {
    /*删除数据*/
    if (i>l->length||i<1||l->length==0) {
        return ERROR;
    }
    if (elem) {
        strcpy(*elem,l->list[i-1]);
    }
    free(l->list[i-1]);
    for (int k=i-1;k<l->length;k++) {
        l->list[k]=l->list[k+1];
    }
    l->list[l->length-1]=NULL;
    l->length--;
    return OK;
}

int ListAdd(SqList * l,ElementType elem) {
    /*在最后添加数据*/
    if (!l || !elem) {
        return ERROR;
    }

    if (l->length==l->size) {
        reInitList(l);
    }
    l->list[l->length] = (char *)malloc(sizeof(char)*(strlen(elem)+1));
    if (!l->list[l->length]) {
        return ERROR;
    }
    strcpy(l->list[l->length],elem);
    l->length++;
    return OK;
}

void visit(ElementType elem) {
    /*访问数据*/
    if (elem == NULL || strcmp(elem, "") == 0) {
        return;
    }
    printf("%s\n", elem);
}

void ListTraverse(const SqList * l) {
    /*遍历顺序表*/
    for (int i = 0; i < l->length; i++) {
        visit(l->list[i]);
    }
}



//二、实现单链表

/*结构体定义*/
typedef struct Node {
    ElementType data;
    struct Node *next;
}Node,*LinkList;

int InitLinkList(LinkList* l) {
    /*初始化单链表*/
    *l = (LinkList)malloc(sizeof(Node));
    if (!(*l)) {
        return ERROR;
    }
    (*l)->data = 0;
    (*l)->next = NULL;
    return OK;
}

void DestroyLinkList(LinkList* l) {
    /*销毁单链表*/
    free(*l);
    *l = NULL;
}

void ClearLinkList(LinkList* l) {
    /*清空单链表*/
    Node* p = (*l)->next;
    while (p) {
        Node* tmp = p;
        p = p->next;
        free(tmp);
    }
}

int insertFirst(LinkList* l, ElementType e) {
    /*在头节点前插入*/
    Node* p = (Node*)malloc(sizeof(Node));
    if (!p) {
        return ERROR;
    }
    p->data = e;
    p->next = (*l)->next;
    (*l)->next = p;
    return OK;
}

int append(LinkList* l, ElementType e) {
    /*在单链表最后插入*/
    Node* p=(*l);
    while (p->next) {
        p = p->next;
    }
    p->next = (Node*)malloc(sizeof(Node));
    if (!p->next) {
        return ERROR;
    }
    p->next->data = e;
    p->next->next = NULL;
    return OK;
}

int InsertLinkList(LinkList* l, int i, ElementType e) {
    /*在第i个节点前插入*/
    Node* p=(*l);
    for (int j = 0; j < i-1; j++) {
        p = p->next;
        if (p == NULL) {
            return ERROR;
        }
    }
    Node* q= (Node*)malloc(sizeof(Node));
    if (!q) {
        return ERROR;
    }
    q->data = e;
    q->next = p->next;
    p->next = q;
    return OK;
}

int DeleteLinkList(LinkList* l,int i) {
    /*删除第i个节点*/
    Node* p=(*l);
    for (int j = 0; j < i-1; j++) {
        p = p->next;
        if (p == NULL) {
            return ERROR;
        }
    }
    if (p->next == NULL) {
        return ERROR;
    }
    Node* q= p->next;
    p->next = q->next;
    free(q);
    return OK;
}



//三、通过顺序表，实现字符串的操作功能
const char* StrBreak=";:(),.?! ";

void BuildList(SqList * l,char* s) {
    /*通过命令行输入创建一个字符串顺序表*/
    char *token = strtok(s, StrBreak);
    while (token != NULL) {
        ListAdd(l,token);
        token = strtok(NULL, StrBreak);
    }
}


void InsertStr(SqList * l,char* s,int i) {
    /*插入一个或多个字符串*/
    int m=i;
    char *token = strtok(s, StrBreak);
    while (token != NULL) {
        ListInsert(l,m,token);
        m++;
        token = strtok(NULL, StrBreak);
    }
}

void DeleteStr(SqList * l,int i,int n) {
    /*删除一个或多个字符串*/
    for (int j = 0; j < n; j++) {
        ListDelete(l,i,NULL);
    }
}

void swap(SqList * l,int i,int j) {
    ElementType tmp = l->list[i];
    l->list[i] = l->list[j];
    l->list[j] = tmp;
}

void flip(SqList * l) {
    /*翻转字符串*/
    int i = l->length-1;
    int j=0;
    while (j<=i) {
        swap(l,i--,j++);
    }
}

int Is_palindrome(SqList * l) {
    /*判断是否是回文字符串*/
    int i = l->length-1;
    int j=0;
    while (j<=i) {
        if (strcmp(l->list[j],l->list[i])!=0) {
            return 0;
        }
        j++;
        i--;
    }
    return 1;
}

int Is_conclude(SqList *l, char* s) {
    char s_copy[256];
    strcpy(s_copy, s);

    char *saveptr1, *saveptr2;
    char *token = strtok_r(s_copy, StrBreak, &saveptr1);
    if (token == NULL) return 0;

    int* start_pos = LocateElem(l, token);
    if (start_pos == NULL || start_pos[0] == 0) {
        if (start_pos) free(start_pos);
        return 0;
    }

    for (int i = 1; i <= start_pos[0]; i++) {
        int flag = 1;
        int current_index = start_pos[i] - 1;

        // 为每个起始位置重新解析字符串
        char s_temp[256];
        strcpy(s_temp, s);
        char *temp_token = strtok_r(s_temp, StrBreak, &saveptr2);
        temp_token = strtok_r(NULL, StrBreak, &saveptr2); // 跳过第一个已匹配的

        while (temp_token != NULL) {
            current_index++;
            if (current_index >= l->length ||
                compare(l->list[current_index], temp_token) != 0) {
                flag = 0;
                break;
                }
            temp_token = strtok_r(NULL, StrBreak, &saveptr2);
        }

        if (flag) {
            free(start_pos);
            return 1;
        }
    }

    free(start_pos);
    return 0;
}
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void count_words_num(SqList* l) {
    char* words[1024]={NULL};
    int nums[1024]={0};
    int len=0;
    for (int i = 0; i < l->length; i++) {
        int flage=0;
        char* s = l->list[i];
        for (int j = 0; j < len; j++) {
            if(strcmp(s,words[j])==0) {
                nums[j]++;
                flage=1;
                break;
            }
        }
        if (flage==0) {
            words[len]=s;
            nums[len]++;
            len++;
        }
    }
    for (int i = 0; i < len; i++) {
        printf("%s:%d\n",words[i],nums[i]);
    }
}

void find_diff(SqList * l1, SqList * l2) {
    int m=l1->length;
    int n=l2->length;
    int pos=0;
    if (m>=n) {
        for (int i = 0; i < n; i++) {
            if (strcmp(l1->list[i],l2->list[i])!=0) {
                break;
            }
            pos++;
        }
        printf("Different part:\n");
        for (int i = pos; i < m; i++) {
            printf("%s ",l1->list[i]);
        }
        printf("\n");
        for (int i = pos; i < n; i++) {
            printf("%s ",l2->list[i]);
        }
        printf("\n");
    }else {
        for (int i = 0; i < m; i++) {
            if (strcmp(l1->list[i],l2->list[i])!=0) {
                break;
            }
            pos++;
        }
        printf("Different part:\n");
        for (int i = pos; i < m; i++) {
            printf("%s ",l1->list[i]);
        }
        printf("\n");
        for (int i = pos; i < n; i++) {
            printf("%s ",l2->list[i]);
        }
        printf("\n");
    }
}

void merge(SqList* l1,SqList* l2,SqList* l3) {
    for (int i = 0; i < l1->length; i++) {
        ListAdd(l3,l1->list[i]);
    }
    for (int i = 0; i < l2->length; i++) {
        ListAdd(l3,l2->list[i]);
    }
}

int main() {
    SqList * myStr[10]={NULL};
    char* names[10]={NULL};
    int num=0;
    int opration=0;
    int str=0;
    int str2=0;
    int pos=-1;
    int build_mode=0;
    while (1) {
        for (int i = 0; i < 10; i++) {
            printf("==");
        }
        printf("\n");

        if (num==0) {
            printf("you have no strings.\n");
            for (int i = 0; i < 10; i++) {
                printf("=");
            }
            printf("\n");
        }else {
            printf("my strings:\n");
            for (int i = 1; i <= num; i++) {
                printf("%d.%s\n",i,names[i-1]);
            }
            for (int i = 0; i < 10; i++) {
                printf("==");
            }
            printf("\n");
        }

        printf("==please input num to choose your operation==\n");
        printf("*1.create strings    *2.print string    *3.insert\n"
               "*4.delete    *5.flip    *6.check is palindrome\n"
               "*7.get length    *8.check is innclude    *9.delete a strings\n"
               "=plus operation=\n"
               "*11.count the num of words    *12.merge two string\n");

        scanf("%d",&opration);
        clearInputBuffer();
        switch (opration) {
            case 12:
                printf("==please select two strings to merge==\n");
                scanf("%d%d",&str,&str2);
                clearInputBuffer();
                SqList* new2 = (SqList*)malloc(sizeof(SqList));
                if (InitList(new2) != OK) {
                    printf("==Fail!==\n");
                    break;
                }
                merge(myStr[str-1],myStr[str2-1],new2);
                myStr[num]=new2;
                names[num]=(char*)malloc(sizeof(char)*9);
                strcpy(names[num],"mergeStr");
                num++;
                break;

            /*case 12:
                printf("==Please select two strings to find diff==\n");
                scanf("%d %d",&str,&str2);
                clearInputBuffer();
                find_diff(myStr[str-1],myStr[str2-1]);
                break;*/

            case 11:
                printf("==Please select the string==\n");
                scanf("%d",&str);
                clearInputBuffer();
                count_words_num(myStr[str-1]);
                break;

            case 9:
                printf("==Please select the string==\n");
                scanf("%d",&str);
                clearInputBuffer();
                for (int i = str-1; i < num; i++) {
                    myStr[i]=myStr[i+1];
                    names[i]=names[i+1];
                }
                names[num]=NULL;
                num--;
                printf("==success!==\n");
                break;
            case 1:
                printf("==select mode:1.input mode  2.file mode==\n");
                scanf("%d",&build_mode);

                char name_buffer[100];
                printf("==Please input your string name==\n");
                scanf("%99s", name_buffer);
                clearInputBuffer();
                names[num] = malloc(strlen(name_buffer) + 1);
                strcpy(names[num], name_buffer);

                SqList* new = (SqList*)malloc(sizeof(SqList));
                if (InitList(new) != OK) {
                    printf("==Fail!==\n");
                    free(names[num]);
                    break;
                }

                if (build_mode==1) {
                    printf("==Please input your string content==\n");
                    char content[1000];
                    fgets(content, sizeof(content), stdin);
                    content[strcspn(content, "\n")] = '\0';

                    BuildList(new,content);
                    myStr[num]=new;
                    printf("==success!==\n");
                    num++;
                    break;
                }
                if (build_mode == 2) {
                    char file[256];
                    printf("==Please input your file address==\n");
                    if (fgets(file, sizeof(file), stdin) != NULL) {
                        file[strcspn(file, "\n")] = '\0';
                        FILE* fp = fopen(file, "r");
                        if (fp) {
                            fseek(fp, 0, SEEK_END);
                            long file_size = ftell(fp);
                            fseek(fp, 0, SEEK_SET);
                            if (file_size <= 0) {
                                printf("Error: File is empty or cannot be read.\n");
                                fclose(fp);
                                break;
                            }
                            char* content = (char*)malloc(file_size + 1);
                            if (content == NULL) {
                                printf("Error: Memory allocation failed.\n");
                                fclose(fp);
                                break;
                            }
                            size_t bytes_read = fread(content, 1, file_size, fp);
                            content[bytes_read] = '\0';
                            for (size_t i = 0; i < bytes_read; i++) {
                                if (content[i] == '\n' || content[i] == '\r') {
                                    content[i] = ' ';
                                }
                            }

                            BuildList(new, content);
                            myStr[num] = new;
                            printf("==success!==");
                            num++;

                            free(content);
                            fclose(fp);
                            break;
                        } else {
                            printf("Error: Could not open file '%s'\n", file);
                            perror("Error details");
                        }
                        break;
                    } else {
                        printf("Error reading input.\n");
                        break;
                    }
                }


            case 2:
                printf("==select string to print==\n");
                scanf("%d",&str);
                if (str<1||str>num) {
                    printf("==this strings doesn't exit==\n");
                    break;
                }
                clearInputBuffer();
                ListTraverse(myStr[str-1]);
                printf("\n");
                break;

            case 3:
                printf("==select string to insert==\n");
                scanf("%d",&str);
                if (str<1||str>num) {
                    printf("==this strings doesn't exit==\n");
                    break;
                }
                clearInputBuffer();
                char content[1000];
                printf("==Please input your string content==\n");
                fgets(content, sizeof(content), stdin);
                content[strcspn(content, "\n")] = '\0';
                printf("==select position to insert==\n");
                scanf("%d",&pos);
                clearInputBuffer();
                InsertStr(myStr[str-1],content,pos);
                break;

            case 4:
                int n=0;
                printf("==Please select the string==\n");
                scanf("%d",&str);
                printf("==Please input the position and word nums to delete==\n");
                scanf("%d",&pos);
                scanf("%d",&n);
                clearInputBuffer();
                DeleteStr(myStr[str-1],pos,n);
                break;

            case 5:
                printf("==Please select the string==\n");
                scanf("%d",&str);
                clearInputBuffer();
                flip(myStr[str-1]);
                break;

            case 6:
                printf("==Please select the string==\n");
                scanf("%d",&str);
                clearInputBuffer();
                if (Is_palindrome(myStr[str-1])) {
                    printf("==Palindrome!==\n");
                }else {
                    printf("==Not a palindrome!==\n");
                }
                break;

            case 7:
                printf("==Please select the string==\n");
                scanf("%d",&str);
                clearInputBuffer();
                printf("==this string has %d words==\n",myStr[str-1]->length);
                break;

            case 8:
                char words[256];
                printf("==Please select the string==\n");
                scanf("%d",&str);
                clearInputBuffer();
                printf("==input the words to check==\n");
                fgets(words, sizeof(words), stdin);
                words[strcspn(words, "\n")] = '\0';
                if (Is_conclude(myStr[str-1],words)) {
                    printf("==include!==\n");
                }else {
                    printf("==Not include!==\n");
                }
                break;

            default:
                printf("==unsupported command!==\n");
                break;
        }
    }
}
