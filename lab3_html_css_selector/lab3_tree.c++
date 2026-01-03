#include<iostream>
#include<vector>
#include<string>
#include<map>
#include <cstdlib>
#include<cstdio>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>

using namespace std;

enum NodeType {
    ELEMENT_NODE,
    TEXT_NODE
};

typedef struct Node{
    NodeType type;
    string tagname;
    string content;
    string id;
    vector<string> classlist;
    map<string,string> attributions;
    struct Node* parent;
    vector<Node*> children;
}Node;

string delet_space(const string& str){
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

vector<string> split_classes(const string& s) {
    vector<string> res;
    stringstream ss(s);
    string item;
    while (ss >> item) res.push_back(item);
    return res;
}

bool is_selfclose_element(const string& tag) {
    static set<string> void_tags = {"img", "br", "hr", "input", "meta", "link"};
    return void_tags.count(tag);
}

Node* creat_dom(string path){
    Node* html=new Node();
    html->tagname="document";
    FILE* fp=fopen(path.c_str(),"rb");
    if(!fp){
        cout<<"错误：html文件打开失败."<<endl;
        delete html;
        return NULL;
    }
    fseek(fp,0,SEEK_END);
    long long fsize=ftell(fp);
    fseek(fp,0,SEEK_SET);
    string html_content(fsize,0);
    fread(&html_content[0],1,fsize,fp);
    fclose(fp);
    // TODO:开始构造dom树
    Node* cur=html;
    long long pos=0;
    long long len=html_content.length();

    while(pos<len){
        long long cur_pos=html_content.find('<',pos);
        // 文本内容
        if(cur_pos!=string::npos&&cur_pos>pos){
            string text_o=html_content.substr(pos,cur_pos-pos);
            string text=delet_space(text_o);
            if(!text.empty()){
                Node* textNode=new Node();
                textNode->type=TEXT_NODE;
                textNode->content=text;
                textNode->parent=cur;
                cur->children.push_back(textNode);
            }
        }

        //tag
        if(cur_pos==string::npos)break;
        long long next_pos=html_content.find('>',cur_pos);
        if(next_pos==string::npos)break;
        string tag_content=html_content.substr(cur_pos+1,next_pos-cur_pos-1);
        if(!tag_content.empty()&&tag_content[0]=='/'){
            cur=cur->parent;
        }else if(!tag_content.empty() && (tag_content[0] == '!' || tag_content[0] == '?')){
            //防止：<!-- document.querySelectorAll('div > .class1') Result: NodeList [span.class1] -->
            long long next_pos_2=html_content.find('>',next_pos+1);
            long long cur_pos_2=html_content.find('<',next_pos+1);
            if(next_pos_2<cur_pos_2&&cur_pos_2!=string::npos&&cur_pos_2!=string::npos){
                next_pos=next_pos_2;
            }
        }else{
            Node* newNode=new Node();
            newNode->type=ELEMENT_NODE;
            newNode->parent=cur;

            long long space_pos=tag_content.find(' ');
            if(space_pos==string::npos){
                newNode->tagname=tag_content;
            }else{
                newNode->tagname = tag_content.substr(0, space_pos);
                string attr_str = tag_content.substr(space_pos + 1);

                long long attr_pos = 0;
                while (attr_pos < attr_str.length()) {
                    long long eq = attr_str.find('=', attr_pos);
                    if (eq == string::npos) break;    
                    string key = delet_space(attr_str.substr(attr_pos, eq - attr_pos));
                    // 找引号
                    long long quote_start = attr_str.find_first_of("\"'", eq + 1);
                    if (quote_start == string::npos) break;
                    char quote_char = attr_str[quote_start];
                    long long quote_end = attr_str.find(quote_char, quote_start + 1);
                    if (quote_end == string::npos) break;
                    string val = attr_str.substr(quote_start + 1, quote_end - quote_start - 1);
                    // 存储属性
                    newNode->attributions[key] = val;
                    if (key == "id") newNode->id = val;
                    if (key == "class") newNode->classlist = split_classes(val);
                    
                    attr_pos = quote_end + 1;
                }
            }
            if (!newNode->tagname.empty() && newNode->tagname.back() == '/') {
                newNode->tagname.pop_back();
            }

            cur->children.push_back(newNode);

            if(!((tag_content.back() == '/') || is_selfclose_element(newNode->tagname))){
                cur=newNode;
            }
        }
        pos=next_pos+1;
    }
    
    return html->children[0];
}

Node* html_dom(string path){
    if(path.substr(0,4)=="http"){
        cout << "检测到 URL，正在下载..." << endl;
        string localFileName = "temp_download.html";
        string command = "curl -s -o " + localFileName + " \"" + path + "\"";
        int result = system(command.c_str());
        if(result == 0){
            cout<<"下载成功！已保存为 "<<localFileName<<endl;
        }else{
            cout<<"错误：下载失败."<<endl;
            return NULL;
        }
        Node* html=creat_dom(localFileName);
        remove(localFileName.c_str());
        cout<<"临时文件 "<<localFileName<<" 已清理."<<endl;
        return html;
    }else{
        Node* html=creat_dom(path);
        return html;
    }
}

// vector<string> class_qurries(string selector){
//     vector<string> qurries;
//     int pos=0;
//     int length=selector.length();
//     while(pos<length){
//         int cur=selector.find(".",pos);
//         int next=selector.find(".",cur+1);
//         if(next==string::npos){
//             string a_querry=selector.substr(cur+1,next-cur-1);
//             string one_querry;
//             if(a_querry[a_querry.length()-1]==' '){
//                 one_querry=a_querry.substr(0,a_querry.length()-1);
//             }else{
//                 one_querry=a_querry;
//             }
//             qurries.push_back(one_querry);
//             break;
//         }else{
//             string a_querry=selector.substr(cur+1,next-cur-1);
//             string one_querry;
//             if(a_querry[a_querry.length()-1]==' '){
//                 one_querry=a_querry.substr(0,a_querry.length()-1);
//             }else{
//                 one_querry=a_querry;
//             }
//             qurries.push_back(one_querry);
//         }
//         pos=next;
//     }
//     return qurries;
// }

// void find_node_class_1(vector<string> qurries,Node* node,vector<Node*>& out){
//     if(!node||node->type==TEXT_NODE)return;
//     if(node->attributions.count("class")){
//         string class_content=node->attributions["class"];
//         int flag=1;
//         for(string s:qurries){
//             int pos=class_content.find(s);
//             if(pos==string::npos){
//                 flag=0;
//                 break;
//             }
//         }
//         if(flag)out.push_back(node);
//     }
//     for(Node* child:node->children){
//         find_node_class_1(qurries,child,out);
//     }
// }

// void find_node_class_2(vector<string> qurries,Node* node,vector<Node*>& out){

// }

// vector<Node*> qurry(string selector,Node* html){
//     vector<Node*> out;
//     if(selector[0]=='.'){
//         vector<string> qurries=class_qurries(selector);
//         int mode=selector.find(' ');
//         if(mode==string::npos){
//             find_node_class_1(qurries,html,out);
//         }else{
//             find_node_class_2(qurries,html,out);
//         }
//         return out;
//     }
//     return out;
// }

typedef struct{
    string tag;
    string id;
    vector <string> classes;
    map<string, string> attributes;
}one_query;

one_query make_query(string s){
    one_query query;
    query.tag = "";
    query.id = "";
    
    int i = 0;
    int n = s.length();

    string tag_buf = "";
    while(i < n && s[i] != '#' && s[i] != '.' && s[i] != '[') {
        tag_buf += s[i];
        i++;
    }
    if(tag_buf != "*") query.tag = tag_buf;

    while (i < n) {
        char cur = s[i];

        if (cur == '#') {
            i++;
            string id_buf = "";
            while(i < n && s[i] != '#' && s[i] != '.' && s[i] != '[') {
                id_buf += s[i];
                i++;
            }
            query.id = id_buf;
        }
        
        else if (cur == '.') {
            i++;
            string class_buf = "";
            while(i < n && s[i] != '#' && s[i] != '.' && s[i] != '[') {
                class_buf += s[i];
                i++;
            }
            if(!class_buf.empty()) query.classes.push_back(class_buf);
        }
        
        else if (cur == '[') {
            i++;
            string next = "";
            while(i < n && s[i] != ']') {
                next += s[i];
                i++;
            }
            if (i < n) i++;
          
            size_t eq_pos = next.find('=');
            if (eq_pos != string::npos) {
                string key = delet_space(next.substr(0, eq_pos));
                string val_raw = next.substr(eq_pos + 1);
                
                size_t q1 = val_raw.find_first_of("\"'");
                size_t q2 = val_raw.find_last_of("\"'");
                string val;
                if(q1 != string::npos && q2 != string::npos && q2 > q1) {
                    val = val_raw.substr(q1 + 1, q2 - q1 - 1);
                } else {
                    val = delet_space(val_raw);
                }
                query.attributes[key] = val;
            } else {
                query.attributes[delet_space(next)] = "__ANY__"; 
            }
        } 
        else {
            //容错处理
            i++;
        }
    }
    return query;
}

bool check_node(Node* node,one_query query){
    if(!node||node->type!=ELEMENT_NODE)return false;
    if(!query.tag.empty()&&node->tagname!=query.tag)return false;
    if(!query.id.empty()&&node->id!=query.id)return false;
    for(string qclass:query.classes){
        int flag=0;
        for(string node_class:node->classlist){
            if(node_class==qclass)flag=1;
        }
        if(!flag)return false;
    }
    for(auto const& [q_key, q_val] : query.attributes){
        if(node->attributions.find(q_key) == node->attributions.end()) return false;
        if(q_val != "__ANY__") {
            if(node->attributions[q_key] != q_val) return false;
        }
    }
    return true;
}

void check_descendants(Node* cur,one_query query,vector<Node*>& res){
    if(!cur||cur->children.empty())return;
    for(Node* child:cur->children){
        if(check_node(child,query))res.push_back(child);
        check_descendants(child,query,res);
    }
}

vector<Node*> query(string selector,Node*html){
    string buf="";
    for (char c : selector) {
        if (c=='>'||c=='+'||c=='~'){
            buf += " "; 
            buf += c; 
            buf += " ";
        } else {
            buf += c;
        }
    }
    
    stringstream ss(buf);
    string item;
    vector<string> tokens;
    while (ss >> item) tokens.push_back(item);

    vector<Node*> sources = {html}; 
    string relation = " "; 

    for(string qtoken : tokens) {
        if(qtoken == ">" || qtoken == "+" || qtoken == "~") {
            relation = qtoken;
            continue;
        }
        one_query token=make_query(qtoken);
        vector<Node*> targets;
        
        for(Node* parent : sources) {
            if(relation == " ") { 
                check_descendants(parent, token, targets);
            } 
            else if(relation == ">") {
                for(Node* child : parent->children) {
                    if(check_node(child, token)) targets.push_back(child);
                }
            } 
            else if(relation == "+" || relation == "~") {
                if(!parent->parent) continue;
                
                vector<Node*>& siblings = parent->parent->children;
                int self_idx = -1;
                for(int i=0; i<siblings.size(); ++i) {
                    if(siblings[i] == parent) { self_idx = i; break; }
                }

                if(self_idx != -1) {
                    if(relation == "+") {
                        for (int k = self_idx + 1; k < siblings.size(); ++k) {
                            if (siblings[k]->type == TEXT_NODE) continue;
                            if (check_node(siblings[k], token)) targets.push_back(siblings[k]);
                            break; 
                        }
                    } 
                    else if (relation == "~") {
                        for (int k = self_idx + 1; k < siblings.size(); ++k) {
                            if (check_node(siblings[k], token)) targets.push_back(siblings[k]);
                        }
                    }
                }
            }
        }
        sort(targets.begin(), targets.end());
        targets.erase(unique(targets.begin(), targets.end()), targets.end());

        if (targets.empty()) return {}; 
        sources = targets;
        relation = " ";
    }

    return sources;
}

vector<Node*> query_all(string selector, Node* root) {
    vector<Node*> outer;
    //逗号相当于或
    string seg;
    size_t start = 0;
    size_t end = selector.find(',');
    
    while (end != string::npos) {
        string part=selector.substr(start, end - start);
        vector<Node*> res=query(part, root);
        outer.insert(outer.end(), res.begin(), res.end());
        start = end + 1;
        end = selector.find(',', start);
    }
    string part = selector.substr(start);
    vector<Node*> res = query(part, root);
    outer.insert(outer.end(), res.begin(), res.end());

    sort(outer.begin(), outer.end());
    outer.erase(unique(outer.begin(), outer.end()), outer.end());
    
    return outer;
}

void print_dom(Node* node, int depth = 0) {
    if (!node) return;
    string indent(depth * 2, ' ');
    if (node->type == ELEMENT_NODE) {
        cout << indent << "<" << node->tagname;
        if (!node->id.empty()) cout << " id=\"" << node->id << "\"";
        if(!node->attributions.empty()){
            for(auto const& [key,val]:node->attributions){
                cout<<" "<<key<<"=\""<<val<<"\"";
            }
        }
        cout << ">" << endl;
    } else if (node->type == TEXT_NODE) {
        cout << indent << node->content << endl;
    }
    for (Node* child : node->children) {
        print_dom(child, depth + 1);
    }
}
void print_out(vector<Node*> out){
    cout << "[";
    for(int i=0;i<out.size();i++){
        Node* node=out[i];
        string s = node->tagname;
        if (!node->id.empty()) s += "#" + node->id;
        for (const string& cls : node->classlist) s += "." + cls;
        cout<<s;
        if(i<out.size()-1)cout<<", ";
    }
    cout << "]" << endl;
}
void print_text(Node* node){
    if(!node){
        cout<<endl;
        return;
    }else if(node->type==TEXT_NODE){
        cout<<node->content<<endl;
        return;
    }else{
        for(Node* child : node->children){
            print_text(child);
        }
    }
}
void print_outerhtml(Node* node){
    if (!node) return;
    if (node->type == ELEMENT_NODE) {
        cout<<"<"<<node->tagname;
        if (!node->id.empty())cout<<" id=\""<<node->id<<"\"";
        if(!node->attributions.empty()){
            for(auto const& [key,val]:node->attributions){
                cout<<" "<<key<<"=\""<<val<<"\"";
            }
        }
        cout<<">"<<endl;
    } else if (node->type == TEXT_NODE) {
        cout<<node->content<<endl;
    }
}

string extract_arg(string cmd) {
    size_t start = cmd.find('(');
    size_t end = cmd.find_last_of(')');
    if (start == string::npos || end == string::npos || start >= end) return "";
    string content = cmd.substr(start + 1, end - start - 1);
    // 去除首尾引号
    size_t first = content.find_first_not_of(" \t\"'");
    if (first == string::npos) return "";
    size_t last = content.find_last_not_of(" \t\"'");
    return content.substr(first, (last - first + 1));
}

string xpath_to_css(string xpath) {
    string css = "";
    int n = xpath.length();
    
    for (int i=0;i<n; i++) {
        if (xpath[i] == '/') {
            if (i + 1 < n && xpath[i+1] == '/') {
                css += " ";
                i++;
            } else {
                css += " > "; 
            }
        } 
        else if (xpath[i] == '[') {
            size_t end_bracket = xpath.find(']', i);
            if (end_bracket == string::npos) break;
            
            string content = xpath.substr(i + 1, end_bracket - i - 1);
            string clean_content = delet_space(content);
            
            string val = "";
            size_t q1 = clean_content.find_first_of("'\"");
            size_t q2 = clean_content.find_last_of("'\"");
            
            if (q1 != string::npos && q2 != string::npos && q2 > q1) {
                val = clean_content.substr(q1 + 1, q2 - q1 - 1);
            }

            if (clean_content.find("@id") == 0) {
                css += "#" + val;
            } else if (clean_content.find("@class") == 0) {
                css += "." + val;
            } else {
                string key = clean_content;
                if(key[0] == '@') key = key.substr(1); //去掉@
                css += "[" + key + "='" + val + "']";
            }
            i = end_bracket;
        }
        else {
            css += xpath[i];
        }
    }
    return css;
}

int main(){
    Node* html = NULL;           
    vector<Node*> outer; 

    string input;
    cout<<"--- HTML DOM 交互式控制台 ---"<<endl;
    cout<<"支持命令: read(url), query(selectors), xpath(path), Out[k].innerText, Out[k].outerHTML, Out[k].href, Out[k].query(selectors), Out[k].xpath(path)"<<endl;

    while (true) {
        cout<<"> ";
        if(!getline(cin, input)) break;
        if(input=="exit"||input=="quit"||input=="q") break;
        if(input.empty()) continue;

        //1.read(html)
        if (input.substr(0, 4) == "read") {
            string path = extract_arg(input);
            if (path.empty()) {
                cout << "错误: 请提供文件名, 例如 read('index.html')" << endl;
                continue;
            }
            html = html_dom(path);
            if (html) cout << "文档加载成功。" << endl;
        }
        
        //2.query(selector)
        else if (input.substr(0, 5) == "query") {
            if (!html) {
                cout << "错误: 请先使用 read() 加载文档。" << endl;
                continue;
            }
            string selector = extract_arg(input);
            outer = query_all(selector, html);
            print_out(outer);
        }

        //3.Out[k]
        else if (input.substr(0, 4) == "Out[") {
            if (outer.empty()) {
                cout<<"错误: Out为空，请先执行query。"<<endl;
                continue;
            }

            int end_bracket = input.find(']');
            if (end_bracket == string::npos) {
                cout<<"语法错误: 缺少']'" <<endl;
                continue;
            }
            string num_str=input.substr(4, end_bracket - 4);
            int k=stoi(num_str);

            if (k<0||k>=outer.size()) {
                cout << "错误: 索引越界 (当前大小: " << outer.size() << ")" << endl;
                continue;
            }

            Node* target = outer[k];
            string suffix = input.substr(end_bracket + 1);

            //Out[k].innerText
            if(suffix == ".innerText") {
                print_text(target);
            }
            //Out[k].outerHTML
            else if(suffix == ".outerHTML") {
                print_outerhtml(target);
            }
            //Out[k].href
            else if (suffix == ".href") {
                if (target->tagname == "a" && target->attributions.count("href")) {
                    cout << target->attributions["href"] << endl;
                } else {
                    cout << "无href属性" << endl;
                }
            }
            //Out[k].query(selector)
            else if (suffix.substr(0, 7) == ".query(") {
                string selector = extract_arg(suffix);
                outer = query_all(selector, target);
                print_out(outer);
            }
            //Out[k].xapth(path)
            else if (suffix.substr(0,7)==".xpath("){
                string xpath=extract_arg(suffix);
                if(xpath=="."){
                    outer={target};
                }else if(xpath==".."){
                    outer={target->parent};
                }else{
                    string path=xpath_to_css(xpath);
                    outer=query_all(path,target);
                }
                print_out(outer);
            }
            else {
                cout << "错误:输入错误"<<endl;
            }
        }
        //4.xpath(path)
        else if(input.substr(0,5)=="xpath"){
            if(!html){
                cout << "错误: 请先使用 read() 加载文档。" << endl;
                continue;
            }
            string xpath=extract_arg(input);
            string path=xpath_to_css(xpath);
            outer=query_all(path,html);
            print_out(outer);
        }
        else {
            cout << "错误: 未知命令" << endl;
        }
    }
    return 0;
}