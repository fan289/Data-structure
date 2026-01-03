#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#define file "test3(2).html" 

#define OK 0
#define ERROR (-1)
typedef char* ElementType;

//顺序表
#define LIST_INIT_SIZE 100
#define LIST_INCREMENT 100

typedef struct {
    ElementType * list;
    int length;
    int size;
}SqList;

int InitList(SqList *l) {
    if (l == NULL) return ERROR;
    l->list = (ElementType *)malloc(LIST_INIT_SIZE * sizeof(ElementType));
    if (!l->list) return ERROR;
    l->length = 0;
    l->size = LIST_INIT_SIZE;
    return OK;
}

int reInitList(SqList * l) {
    l->size += LIST_INCREMENT;
    ElementType *new_list = (ElementType *)realloc(l->list, l->size * sizeof(ElementType));
    if (!new_list) return ERROR;
    l->list = new_list;
    return OK;
}

int ListAdd(SqList * l,ElementType elem) {
    if (!l || !elem) return ERROR;
    if (l->length==l->size) {
        reInitList(l);
    }
    l->list[l->length] = (char *)malloc(sizeof(char)*(strlen(elem)+1));
    if (!l->list[l->length]) return ERROR;
    strcpy(l->list[l->length],elem);
    l->length++;
    return OK;
}


//栈
#define STACK_INIT_SIZE 100
#define STACK_INCREMENT 100

// 标签类型枚举
typedef enum {
    TAG_TYPE_BLOCK,       // 标准块级元素, e.g., <div>
    TAG_TYPE_INLINE,      // 标准行内元素, e.g., <span>
    TAG_TYPE_SPECIAL_P,   // 特殊块: <p>, <h1>-<h6> (不能包含其他块)
    TAG_TYPE_SPECIAL_A,   // 特殊行内: <a> (不能包含其他 <a>)
    TAG_TYPE_SELF_CLOSING,// 自闭合标签, e.g., <img>, <br>
    TAG_TYPE_UNKNOWN      // 未知标签 (保持宽容)
} TagType;

// 栈元素结构体
typedef struct {
    char* tagName;
    TagType type;
} StackElement;

// 栈结构体
typedef struct{
    StackElement *list; 
    int top;
    int size;
}Stack;

int InitStack(Stack *s){
    if(!s)return ERROR;
    s->list=(StackElement*)malloc(sizeof(StackElement)*STACK_INIT_SIZE);
    if(!s->list){
        return ERROR;
    }
    s->top=0;
    s->size=STACK_INIT_SIZE;
    return OK;
}

int reInitStack(Stack *s){
    s->size+=STACK_INCREMENT;
    StackElement* new_list=(StackElement*)realloc(s->list,s->size*sizeof(StackElement));
    if(!new_list){
        return ERROR;
    }
    s->list=new_list;
    return OK;
}

int push(Stack *s, const char* tagName, TagType type){
    if(!s || !tagName) return ERROR;
    if(s->top == s->size){
        reInitStack(s);
    }
    
    s->list[s->top].tagName = (char*)malloc(strlen(tagName) + 1);
    if (!s->list[s->top].tagName) return ERROR;
    
    strcpy(s->list[s->top].tagName, tagName);
    s->list[s->top].type = type;
    
    s->top++;
    return OK;
}

int pop(Stack *s, StackElement* elem){ 
    if(s->top==0) return ERROR;
    
    s->top--;
    
    if(elem){
        *elem = s->list[s->top]; 
        s->list[s->top].tagName = NULL; 
    } else {
        free(s->list[s->top].tagName);
        s->list[s->top].tagName = NULL;
    }
    return OK;
}


void extractTagName(const char* fullTag, char* outTagName) {
    int i = 1; 
    if (fullTag[i] == '/') { 
        i++;
    }
    int j = 0;
    while (fullTag[i] != '\0' && !isspace((unsigned char)fullTag[i]) && fullTag[i] != '>' && j < 99) {
        outTagName[j] = tolower((unsigned char)fullTag[i]);
        i++;
        j++;
    }
    outTagName[j] = '\0';
}

TagType getTagType(const char* tagName) {
    // 自闭合标签
    if (strcmp(tagName, "meta") == 0 || strcmp(tagName, "base") == 0 ||
        tagName[0] == '!' ||// <!DOCTYPE...>
        strcmp(tagName, "hr") == 0 || strcmp(tagName, "br") == 0 ||
        strcmp(tagName, "img") == 0 || strcmp(tagName, "link") == 0 ||
        strcmp(tagName, "input") == 0 || strcmp(tagName, "area") == 0 ||
        strcmp(tagName, "col") == 0 || strcmp(tagName, "embed") == 0 ||
        strcmp(tagName, "param") == 0 || strcmp(tagName, "source") == 0 ||
        strcmp(tagName, "track") == 0 || strcmp(tagName, "wbr") == 0) {
        return TAG_TYPE_SELF_CLOSING;
    }
    
    // 特殊块级 (<p>, <h1>-<h6>, <dt>) - 不能包含其他块
    if (strcmp(tagName, "p") == 0 || strcmp(tagName, "h1") == 0 ||
        strcmp(tagName, "h2") == 0 || strcmp(tagName, "h3") == 0 ||
        strcmp(tagName, "h4") == 0 || strcmp(tagName, "h5") == 0 ||
        strcmp(tagName, "h6") == 0 || strcmp(tagName, "dt") == 0) {
        return TAG_TYPE_SPECIAL_P;
    }
    
    // 特殊行内 (<a>) - 不能包含其他 <a>
    if (strcmp(tagName, "a") == 0||strcmp(tagName, "u") == 0) {
        return TAG_TYPE_SPECIAL_A;
    }
    
    // 其他标准块级
    if (strcmp(tagName, "div") == 0 || strcmp(tagName, "ul") == 0 ||
        strcmp(tagName, "ol") == 0 || strcmp(tagName, "li") == 0 ||
        strcmp(tagName, "body") == 0 || strcmp(tagName, "html") == 0 ||
        strcmp(tagName, "head") == 0 || strcmp(tagName, "title") == 0 ||
        strcmp(tagName, "footer") == 0 || strcmp(tagName, "header") == 0 ||
        strcmp(tagName, "nav") == 0 || strcmp(tagName, "section") == 0 ||
        strcmp(tagName, "table") == 0 || strcmp(tagName, "tr") == 0 ||
        strcmp(tagName, "td") == 0 || strcmp(tagName, "th") == 0) {
        return TAG_TYPE_BLOCK;
    }
    
    // 其他标准行内
    if (strcmp(tagName, "span") == 0 || strcmp(tagName, "b") == 0 ||
        strcmp(tagName, "i") == 0 || strcmp(tagName, "em") == 0 ||
        strcmp(tagName, "strong") == 0 || strcmp(tagName, "code") == 0 ||
        strcmp(tagName, "sub") == 0 || strcmp(tagName, "sup") == 0) {
        return TAG_TYPE_INLINE;
    }
    
    // 默认为块级
    return TAG_TYPE_BLOCK; 
}

int is_needed(const char* str){
    if(!str)return 0;
    if(str[0]=='\0')return 0;
    int flag=0;
    for(int i=0;i<strlen(str);i++){
        if(!isspace((unsigned char)str[i])){
            flag=1;
        }
    }
    return flag;
}

void del_space(char* str){
    if(!str)return;
    int i=0,j=0;
    int last_space=0;
    while(str[i]!='\0'){
        if(isspace(str[i])){
            if(last_space==0){
                str[j]=' ';
                j++;
                last_space=1;
            }
        }else{
            str[j]=str[i];
            j++;
            last_space=0;
        }
        i++;
    }
    str[j]='\0';
}


int main(){
    /*读取html文件*/
    SqList *l=(SqList*)malloc(sizeof(SqList));
    InitList(l);
    Stack *s=(Stack*)malloc(sizeof(Stack));
    InitStack(s);

    FILE* fp=fopen(file,"r");
    if(!fp){
        printf("文件加载失败.\n");
        free(l->list);
        free(l);
        free(s->list);
        free(s);
        return ERROR;
    }
    
    int is_valid=1;
    char next=fgetc(fp);
    char content[10000];
    int index=0;
    int right_n=0;
    while(next!=EOF){
        if(next=='<'){
            content[index]=next;
            index++;
            next=fgetc(fp);
            while(next!=EOF && index < 9998){ 
                if(next=='<'){
                    right_n++;
                }
                if(next=='>'){
                    if(right_n==0){
                        break;
                    }else{
                        right_n--;
                    }
                }
                content[index]=next;
                index++;
                next=fgetc(fp);
            }
            if(is_valid==0) break; // 提前退出
            if (index >= 9998) { // 标签过长
                printf("Error: Tag is too long.\n");
                is_valid = 0;
                break;
            }
            
            content[index]=next; 
            index++;
            content[index]='\0';

            char tagName[100];
            extractTagName(content, tagName);
            if (content[1] != '/' && (strcmp(tagName, "script") == 0 || strcmp(tagName, "style") == 0)) {
                char closingTagName[100];
                sprintf(closingTagName, "/%s", tagName); 
                while ( (next = fgetc(fp)) != EOF ) {
                    if (next == '<') {
                        char tempTag[100] = {0};
                        int tempIdx = 0;
                        while( (next = fgetc(fp)) != EOF && next != '>' && !isspace((unsigned char)next) && tempIdx < 99) {
                            tempTag[tempIdx++] = tolower((unsigned char)next);
                        }
                        if (strcmp(tempTag, closingTagName) == 0) {
                            if (next != '>') {
                                while ( (next = fgetc(fp)) != EOF && next != '>') {
                                }
                            }
                        break; // 跳出内容 skipping 循环
                        }
                    }
                }
                index = 0;
                next = fgetc(fp); // 读取闭标签后的第一个字符
                continue; // 继续外层 while 循环
            }

            ListAdd(l,content);
            index=0;
            next=fgetc(fp);
        }
        else{
            content[index]=next;
            index++;
            next=fgetc(fp);
            while(next!='<' && next!=EOF && index < 9999){ 
                content[index]=next;
                index++;
                next=fgetc(fp);
            }
            if (index >= 9999) { // 文本内容过长
                printf("Error: Text content is too long.\n");
                is_valid = 0;
                break;
            }

            content[index]='\0';
            if(is_needed(content)){
                del_space(content);
                ListAdd(l,content);
            }
            index=0;
        }
    }
    printf("文件加载完成.\n");
    fclose(fp);

    /*CheckHTML*/
    if (is_valid) { 
        for(int i=0; i < l->length; i++){
            char* cur = l->list[i];
            if(cur[0] != '<'){
                continue; // 忽略文本节
            }

            char tagName[100];
            extractTagName(cur, tagName);

            if(cur[1] != '/'){ 
                TagType currentType = getTagType(tagName);

                if(currentType == TAG_TYPE_SELF_CLOSING) {
                    continue; // 自闭合标签，不入栈
                }

                if (s->top > 0) { // 检查父标签
                    TagType parentType = s->list[s->top - 1].type;
                    const char* parentName = s->list[s->top - 1].tagName;

                    // 规则 1: 块 (div) 不能在 行内 (span) 中
                    if (parentType == TAG_TYPE_INLINE && currentType == TAG_TYPE_BLOCK) {
                        printf("不合法: 块级标签 <%s> 不能嵌套在行内标签 <%s> 中。\n", tagName, parentName);
                        is_valid = 0;
                        break;
                    }

                    // 规则 2: 块 (p, div) 不能在 特殊块 (p) 中
                    if (parentType == TAG_TYPE_SPECIAL_P && (currentType == TAG_TYPE_BLOCK || currentType == TAG_TYPE_SPECIAL_P)) {
                        printf("不合法: 块级标签 <%s> 不能嵌套在标签 <%s> 中。\n", tagName, parentName);
                        is_valid = 0;
                        break;
                    }

                    // 规则 3: <a> 不能在 <a> 中
                    if (parentType == TAG_TYPE_SPECIAL_A && currentType == TAG_TYPE_SPECIAL_A) {
                        printf("不合法: 标签 <a> 不能嵌套在另一个 <a> 标签中。\n");
                        is_valid = 0;
                        break;
                    }
                }
                
                // 检查通过，将此开标签压入栈
                push(s, tagName, currentType);

            } else { // 闭标签
                if(s->top == 0){
                    printf("不合法: 发现多余的闭标签 </%s>。\n", tagName);
                    is_valid = 0;
                    break;
                }
                
                // 检查栈顶的标签名是否匹配
                if(strcmp(s->list[s->top - 1].tagName, tagName) == 0){
                    pop(s, NULL); // 匹配成功，出栈
                } else {
                    printf("不合法: 标签不匹配。期望 </%s>，但得到了 </%s>。\n", s->list[s->top - 1].tagName, tagName);
                    char* newString = (char*)realloc(l->list[i], strlen(s->list[s->top - 1].tagName)+4);
                    if(!newString){
                        printf("内存出错！");
                        return ERROR;
                    }
                    l->list[i]=newString;
                    sprintf(l->list[i], "</%s>", s->list[s->top - 1].tagName);
                    is_valid = 0;
                    break;
                }
            }
        }
    } 

    if(is_valid && s->top == 0){
        printf("html合法\n");
    }else if (is_valid && s->top > 0) {
        printf("html不合法: 文件结束时，标签 <%s> 未正确闭合。\n", s->list[s->top - 1].tagName);
    } else {
        printf("html不合法\n已修正为合法版本存储\n");
    }
    //释放栈
    while (s->top > 0) {
        pop(s, NULL);
    }
    free(s->list);
    free(s);

    // for(int i=0;i<10;i++){
    //     printf("%s",l->list[i]);
    // }

    char operation[10];
    char path[100];
    /*操作OuterHTML和Text*/
    while(1){
        printf("请输入操作OuterHTML /或Text /:\n");
        scanf("%s",&operation);
        if(strcmp(operation,"Exit")==0){
            printf("程序退出！");
            //SqList
            for (int i = 0; i < l->length; i++) {
                free(l->list[i]);
            }
            free(l->list);
            free(l);
            return OK;
        }
        scanf("%s",&path);
        SqList* paths=(SqList*)malloc(sizeof(SqList));
        InitList(paths);
        const char* break_token="/";
        char* token=strtok(path,break_token);
        while(token){
            ListAdd(paths,token);
            token=strtok(NULL,break_token);
        }

        // printf("%d\n",paths->length);

        Stack *ss=(Stack*)malloc(sizeof(Stack));
        InitStack(ss);
        //OuterHtml
        if(strcmp(operation,"OuterHtml")==0){
            int path_flag=0;
            for(int i=0;i<l->length;i++){
                char* cur=l->list[i];
                if(cur[0]=='<'){
                    if(cur[1]=='/'){
                        char tagname[100];
                        extractTagName(cur,tagname);
                        TagType tType=getTagType(tagname);
                        if(ss->top>0&&strcmp(tagname,ss->list[ss->top-1].tagName)==0){
                            pop(ss,NULL);
                            if(path_flag>0){
                                path_flag--;
                            }
                        }
                    }else{
                        char tagname[100];
                        extractTagName(cur,tagname);
                        TagType tType=getTagType(tagname);
                        if(tType==TAG_TYPE_SELF_CLOSING&&strcmp(tagname,paths->list[path_flag])!=0){
                            continue;
                        }
                        if(strcmp(paths->list[path_flag],tagname)==0){
                            if(path_flag==paths->length-1){
                                printf("%s",cur);
                                if(tType==TAG_TYPE_SELF_CLOSING){
                                    printf("\n");
                                    continue;
                                }
                                int next_n=1;
                                while(next_n>0&&(i+1<l->length)){
                                    i++;
                                    char* next_cur = l->list[i];
                                    if (next_cur[0] != '<') {
                                        printf("%s",next_cur);
                                        continue;
                                    }
                                    char next_tagname[100];
                                    extractTagName(next_cur,next_tagname);
                                    TagType next_tType=getTagType(next_tagname);
                                    if(next_tType==TAG_TYPE_SELF_CLOSING){
                                        printf("%s",next_cur);
                                        continue;
                                    }
                                    if(next_cur[1]=='/'){
                                        if(strcmp(tagname,next_tagname)==0){
                                            next_n--;
                                        }
                                        printf("%s",next_cur);
                                    }else{
                                        if(strcmp(tagname,next_tagname)==0){
                                            next_n++;
                                        }
                                        printf("%s",next_cur);
                                    }
                                }
                                printf("\n");
                            }else{
                                path_flag++;
                                push(ss,tagname,tType);
                            } 
                        }else{
                            int next_n=1;
                            while(next_n>0&&(i+1<l->length)){
                                i++;
                                char* next_cur = l->list[i];
                                if (next_cur[0] != '<') {
                                    continue;
                                }
                                char next_tagname[100];
                                extractTagName(next_cur,next_tagname);
                                TagType next_tType=getTagType(next_tagname);
                                if(next_tType==TAG_TYPE_SELF_CLOSING){
                                    continue;
                                }
                                if(next_cur[1]=='/'){
                                    if(strcmp(tagname,next_tagname)==0){
                                        next_n--;
                                    }
                                }else{
                                    if(strcmp(tagname,next_tagname)==0){
                                        next_n++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            //释放栈
            while (ss->top > 0) {
                pop(ss, NULL);
            }
            free(ss->list);
            free(ss);
        //Text
        }else if(strcmp(operation,"Text")==0){
            int path_flag=0;
            for(int i=0;i<l->length;i++){
                char* cur=l->list[i];
                if(cur[0]=='<'){
                    // if(path_flag>=paths->length){
                    //     continue;
                    // }
                    if(cur[1]=='/'){
                        char tagname[100];
                        extractTagName(cur,tagname);
                        TagType tType=getTagType(tagname);
                        if(ss->top>0&&strcmp(tagname,ss->list[ss->top-1].tagName)==0){
                            pop(ss,NULL);
                            if(path_flag>0){
                                path_flag--;
                            }
                        }
                    }else{
                        char tagname[100];
                        extractTagName(cur,tagname);
                        TagType tType=getTagType(tagname);
                        if(tType==TAG_TYPE_SELF_CLOSING){
                            continue;
                        }
                        if(path_flag>=paths->length){
                            continue;
                        }
                        if(strcmp(paths->list[path_flag],tagname)==0){
                            path_flag++;
                            push(ss,tagname,tType);
                        }else{
                            int next_n=1;
                            while(next_n>0&&(i+1<l->length)){
                                i++;
                                char* next_cur = l->list[i];
                                if (next_cur[0] != '<') {
                                    continue;
                                }
                                char next_tagname[100];
                                extractTagName(next_cur,next_tagname);
                                TagType next_tType=getTagType(next_tagname);
                                if(next_tType==TAG_TYPE_SELF_CLOSING){
                                    continue;
                                }
                                if(next_cur[1]=='/'){
                                    if(strcmp(tagname,next_tagname)==0){
                                        next_n--;
                                    }
                                }else{
                                    if(strcmp(tagname,next_tagname)==0){
                                        next_n++;
                                    }
                                }
                            }
                        }
                    }
                }else{
                    if(path_flag>=paths->length){
                        printf("%s\n",cur);
                    }
                }
            }
            //释放栈
            while (ss->top > 0) {
                pop(ss, NULL);
            }
            free(ss->list);
            free(ss);
        }else{
            printf("输入操作不合法\n");
            return ERROR;
        }
    }
}