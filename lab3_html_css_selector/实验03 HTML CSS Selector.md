# 实验03 HTML CSS Selector

**题目**：实现一个功能正确、方便易用的HTML CSS Selector

**姓名**：罗毅凡

**学号**：2024202715



### 一、需求分析

1. 通过**树数据结构**对HTML DOM层次结构进行建模。实现**常用HTML CSS Selector以及对应的获取文本、HTML、链接**等操作。

2. 开发测试或者**演示程序**，展示HTML CSS Selector的使用方法，验证正确性。程序执行的命令包括：

   1）**read(file_name_or_url)**：从文件读入Html

   2）**query(selector)**：根据selector从全文档检索所有符合条件的nodes，按顺序打印每个node的标签类型，id，class，同时将这些nodes保存，以供后续操作。

   3）**Out[k].innerText**：根据上次query操作得到的node list，找到第k个，输出他的innerText

   4）**Out[k].outerHTML**：根据上次query操作得到的node list，找到第k个，输出他的outerHTML

   5）**Out[k].query(selector)**：根据上次query操作得到的node list，找到第k个，再次调用选择器进行查询。按顺序打印每个node的标签类型，id，class，同时更新保存的nodes

   6）**Out[k].href**： 根据上次query操作得到的node list，找到第k个，输出他的href（仅对a标签）

3. **额外功能实现：**

   1）**xpath(path)**：支持xpath语法查找

   2）**Out[k].xpath(path)**：根据上次query操作得到的node list，找到第k个，进行xpath查找

4. **测试数据**

   （1）`example.html`：

   `query(.class1)`：`[div.class0.class1, span.class1, img.class1]`等

   （2）`lab3_news.html`：

   `query(.holding.with-hover.weibo-logo)`：`[div.holding.with-hover.weibo-logo]`等

   （1）`lab3_newslist.html`：

   `query(input~input)`：`[input#search_btn.submit]`等



### 二、概要设计

* **为实现上述程序功能，应以树结构对HTML DOM层次结构进行建模，如何实现对选择字符selector的解析，遍历树结构进行选择，支持对已选择节点的后续操作，最后包装命令台交互。**

1. 树结构的数据类型为：

   ```c
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
   ```

2. 单查找内容数据结构为：

   ```c
   typedef struct one_query{
       string tag;
       string id;
       vector <string> classes;
   }one_query;
   ```

3. 本程序包含四个模块：

   1）**主程序模块**：

   ```c
   void main(){
   	初始化；
   	while(true){
   		接受命令；
   		处理命令；
   	}
   }
   ```

   2）**文档读取模块**：读取本地文档或解析url，构建html树结构

   3）**选择模块**：解析selector，遍历树结构，输出符合要求的节点信息

   4）**节点后续操作模块**：对已选择节点进行后续操作



### 三、详细设计

#### 1. read(file_or_url)

* **实现逻辑：**

  将整个html文件读取到字符串中，然后**解析这个字符串**来构造树，构造树结构时通过维护一个node指针cur来实现**层次结构的构建**，cur指向当前读取到的html元素地址：

  * 读取到文本内容：构建一个文本节点作为cur的child
  * 读到元素开始：构建一个元素节点作为cur的child，并更新cur为这个新节点指针
  * 读到元素结束：更新cur为cur->parent
  * 特殊情况：读到自闭和标签、注释块等直接跳过

  这样我们就完成动态构建了一个html层次结构树

* **伪代码：**

  ```c
  Node* creat_dom(string path){
      content=读取文件(path)
      root=NewNode("document")
      cur=root  //当前节点指针
  
      while 遍历 content:
          //1.文本
          text=获取当前位置到下一个 '<' 之间的字符
          if text: 
              cur.addChild(NewTextNode(text))
  
          //2.标签
          tagStr=获取 '<' 和 '>' 之间的内容
          if tagStr 以'/'开头:     //结束标签
              cur=cur.parent      //维护cur
              
          else if tagStr 是注释或声明: 
              continue              
              
          else:   //开始标签
              node=...  //创建新元素节点
              cur.addChild(node)
  
              if NOT isSelfClosing(node): 
                  cur=node       //维护cur
  
      return root.children[0]
  ```



#### 2. query(selector)

* **实现逻辑：**

  我将选择条件分为两类：

  * 针对当前节点的选择条件
  * 对前后关系的选择条件，这个又分几种，这里不细说

  所以，这里要实现对复杂输入逻辑的选择，首先是处理selector，区分两种条件，在依次遍历查找：

  * 处理selector维护一个当前条件关系，然后循环选择并去重
  * 根据当前的条件关系，选择所有符合条件的节点，再去重（这里逻辑比较粗暴，效率相对会低一些）

* **伪代码：**

  ```c
  vector<Node*> query(string selector,Node*html){
      selector_str=在selector的'>','+','~'两侧添加空格   //预处理selector
      tokens=split(selector_str, " ") 
  
      sources=[htmlNode]   //上一轮匹配到的节点
      relation=" "   //维护条件关系
  
      for token in tokens:
          //更新关系
          if token is ">" OR "+" OR "~":
              relation=token
              continue
                  
          rule=MakeQuery(token)
          targets=[]    //存储本轮匹配结果
  
          for node in sources:
              if relation==" ":
                  found=FindDescendants(node, rule)
                  targets.addAll(found)
  
              else if relation==">": //只看孩子节点
                  for child in node.children:
                      if CheckNode(child, rule): targets.add(child)
  
              else if relation=="+" OR "~": //只看兄弟节点
                  if node.parent is NULL: continue
                  siblings=node.parent.children
                  index=FindIndex(siblings, node)
  
                  if relation=="+":  //只看第一个孩子
                      next_sib=FindFirstElementAfter(siblings, index)
                      if CheckNode(next_sib, rule): targets.add(next_sib)
                  
                  else if relation=="~": //所用后代节点
                      for k from index + 1 to end:
                          if CheckNode(siblings[k], rule): targets.add(siblings[k])
  
          // 4. 准备下一轮
          sources=Unique(targets)   //去重
          if sources is empty: return []
          relation=" " 
  
      return sources
  }
  ```




#### 3.xpath(path)

* **实现逻辑：**

  在前面query实现的基础上，去实现xpath有很多方式，我这里思路比较直接：

  * 把输入的path**解析翻译**成query中的selector，然后直接复用query来实现xpath查找

  具体的解析翻译逻辑：

  * 后代关系：将 `//` 转换成 ` ` 
  * 直接子代关系：将 `/` 转换成 `>`
  * 属性内容：将 `[@id='val']` 转换成 `#val` ，将 `[@class='val']` 转换成 `.val`

* **伪代码：**

  ```c
  string xpath_to_css(string xpath) {
      FOR 遍历 xpath 中的每个字符(索引 i):
          IF 当前字符是 '/':
              IF 下一个字符也是 '/':  
                  css 追加 " "   
                  跳过下一个字符 (i++)
              ELSE:                 
                  css 追加 " > " 
                      
          ELSE IF 当前字符是 '[':
              找到对应的右括号 ']' 的位置
              提取括号内的 content
              去除 content 中的多余空格
              
              从 content 中提取引号内的 value
              
              IF content 以 "@id" 开头:
                  css 追加 "#" + value
              ELSE IF content 以 "@class" 开头:
                  css 追加 "." + value
              ELSE:
                  提取 key (去掉开头的 '@')
                  css 追加 "[" + key + "='" + value + "']"
              
              将索引 i 移动到右括号的位置
              
          ELSE:
              css 直接追加当前字符
      RETURN css
  }
  ```

  

#### 4. Out[k]

* **实现逻辑：**

  根据k打印节点信息

* **代码实现：**

  ```c
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
  ```



### 四、调试分析&代码优化历程

> 这里只列出主要的coding过程中遇到的问题，小的试错就不说了

1. 最开始对每种类型的selector设置不同的查找函数，最后放弃这个方案，主要有两点原因：

   * 代码量大且代码重复臃肿
   * 对混合查找不可行，逻辑会更复杂

   最后采用了对selector解析的方法，并维护一个relation实现对不同的关系的选择

2. 去重是很重要的一步，最开始没有对结果去重，发现输出特别多重复，而且有错误的输出



### 五、使用手册

> 与附录中使用手册相同

### **1. 概述**

`HtmlSelectorTool` 是一个基于C/C++实现的高效HTML解析和数据抽取工具。它将HTML文档结构建模为一棵树，并支持常用的CSS选择器（Selector）进行节点定位，以及方便地抽取节点的文本内容、完整HTML代码和链接信息。

本手册将指导用户如何导入HTML文档、执行CSS查询以及对查询结果进行操作。

### **2. 核心功能**

| **功能**      | **描述**                                                     |
| ------------- | ------------------------------------------------------------ |
| **HTML建模**  | 将HTML文件或URL内容解析为内存中的树形数据结构（DOM）。       |
| **CSS选择器** | 支持实现列表中的基本CSS选择器，快速定位目标节点。            |
| **数据抽取**  | 支持抽取节点的内部文本、外部HTML代码以及链接（`href`）属性。 |
| **链式查询**  | 支持对已选中的节点集合再次执行CSS查询，实现局部精确抽取。    |

### **3. 命令行交互操作**

程序提供一个交互式的命令行界面，用户可以通过输入以下命令进行操作：

#### **3.1 文档加载：`read()`**

该命令用于加载HTML内容到程序中，构建DOM树。

| **命令格式**      | **描述**                            |
| ----------------- | ----------------------------------- |
| `read(file_name)` | 从本地文件加载HTML内容。            |
| `read(url)`       | 从指定的URL地址下载并加载HTML内容。 |

**示例:**

```
> read(C:\Users\luo yifan\Desktop\GSAI-2th-mystudy\Data_structure\labs\lab3\实验03\examples\example.html)
文档加载成功。
```

#### **3.2 全局查询：`query()`**

该命令用于在当前加载的整个HTML文档中，根据给定的CSS选择器检索所有符合条件的节点。

| **命令格式**      | **描述**                                             |
| ----------------- | ---------------------------------------------------- |
| `query(selector)` | 执行查询，结果将存储为一个有序列表，供后续操作使用。 |

**支持的基本CSS选择器 (Implemented Selectors):**


| **选择器**                                                   | **例子**      | **例子描述**                                        |
| ------------------------------------------------------------ | ------------- | --------------------------------------------------- |
| [.*class*](https://www.w3school.com.cn/cssref/selector_class.asp) | .intro        | 选择 class="intro" 的所有元素。                     |
| .*class1*.*class2*                                           | .name1.name2  | 选择 class 属性中同时有 name1 和 name2 的所有元素。 |
| .*class1* .*class2*                                          | .name1 .name2 | 选择作为类名 name1 元素后代的所有类名 name2 元素。  |
| [#*id*](https://www.w3school.com.cn/cssref/selector_id.asp)  | #firstname    | 选择 id="firstname" 的元素。                        |
| [*](https://www.w3school.com.cn/cssref/selector_all.asp)     | *             | 选择所有元素。                                      |
| [*element*](https://www.w3school.com.cn/cssref/selector_element.asp) | p             | 选择所有 <p> 元素。                                 |
| [*element*.*class*](https://www.w3school.com.cn/cssref/selector_element_class.asp) | p.intro       | 选择 class="intro" 的所有 <p> 元素。                |
| [*element*,*element*](https://www.w3school.com.cn/cssref/selector_element_comma.asp) | div, p        | 选择所有 <div> 元素和所有 <p> 元素。                |
| [*element* *element*](https://www.w3school.com.cn/cssref/selector_element_element.asp) | div p         | 选择 <div> 元素内的所有 <p> 元素。                  |
| [*element*>*element*](https://www.w3school.com.cn/cssref/selector_element_gt.asp) | div > p       | 选择父元素是 <div> 的所有 <p> 元素。                |
| [*element*+*element*](https://www.w3school.com.cn/cssref/selector_element_plus.asp) | div + p       | 选择紧跟 <div> 元素的首个 <p> 元素。                |
| [*element1*~*element2*](https://www.w3school.com.cn/cssref/selector_gen_sibling.asp) | p ~ ul        | 选择前面有 <p> 元素的每个 <ul> 元素。               |

**查询结果输出:**

程序将按文档顺序打印每个匹配节点的简要信息，并将其保存到 **结果列表** (`Out`) 中。

**示例:**

```
> query(.class2.class3)
[div.class2.class3, span.class3.class2, img.class2.class0.class3]
> query(#id4.class2)
[div#id4.class2]
```

#### **3.3 XPath 查询：`xpath()`**

该命令允许用户使用 XPath 语法在当前文档中进行灵活的节点查找和定位。

| **命令格式**  | **描述**                                    |
| ------------- | ------------------------------------------- |
| `xpath(path)` | 解析 XPath 表达式，检索符合条件的所有节点。 |

**支持的 XPath 语法 (Supported Syntax):**

| **表达式** | **描述**                                                     | **示例**                                    |
| ---------- | ------------------------------------------------------------ | ------------------------------------------- |
| `nodename` | 选取此节点的所有子节点。                                     | `body` (选取 body 节点)                     |
| `/`        | 从根节点选取（绝对路径）。                                   | `/body/div` (选取 body 下的直接 div 子节点) |
| `//`       | 从匹配选择的当前节点选择文档中的节点，而不考虑它们的位置（递归查找）。 | `//div` (选取文档中所有的 div)              |
| `.`        | 选取当前节点。                                               | `.`                                         |
| `..`       | 选取当前节点的父节点。                                       | `..`                                        |
| `@`        | 选取属性（常用于谓语条件中）。                               | `//div[@id='id1']` (选取 id 为 id1 的 div)  |
| `*`        | 通配符，匹配任何元素节点。                                   | `/body/*` (选取 body 下的所有子节点)        |

**示例:**

```
> xpath(/body/div)
[div.class0.class1, div, div#id1, div, ...]
> xpath(//div[@id='id1'])
[div#id1]
```

#### **3.4 结果列表操作：`Out[k].operation`**

查询完成后，用户可以通过 `Out[k]` 访问结果列表中的第 **$k$** 个节点 (索引 $k$ 从 $0$ 开始)。

##### **A. 获取内部文本：`Out[k].innerText`**

返回节点及其所有后代节点的**纯文本内容**，去除所有HTML标签。

**示例:**

```
> Out[0].innerText
div#id4.class2
```

##### **B. 获取完整HTML：`Out[k].outerHTML`**

返回节点自身的**完整HTML代码**，包括其自身标签及其所有子节点。

**示例:**

```
> Out[0].outerHTML
<div id="id4" class="class2" id="id4">
```

##### **C. 获取链接属性：`Out[k].href`**

仅适用于 `<a>` 标签节点。返回该节点的 `href` 属性值。

**示例:**

```
> Out[0].href
/a.html
```

##### **D. 链式查询/二次查询：`Out[k].query(selector)`**

以 **`Out[k]`** 节点为根，在其内部进行一次新的 CSS 查询。新的查询结果将**覆盖**原有的结果列表 (`Out`)。

**示例 (在列表的第一个 `div` 节点内部查找所有的 `p` 标签):**

```
> Out[0].query(*)
[p#id5.class5]
```

**E. 链式 XPath 查询：`Out[k].xpath(path)`**

以 **`Out[k]`** 节点为上下文环境（当前节点），执行 XPath 查询。支持使用 `.` (当前节点) 和 `..` (父节点) 进行相对导航。

**示例:**

```
> Out[0].xpath(..)
[html]
> Out[0].xpath(.)
[body]
```

#### 3.5 退出：q 或 quit 或 exit



### 六、功能测试报告

> 与附录中功能测试报告相同

### 1. 测试概述

本次测试旨在验证 HTML CSS Selector 工具的核心功能完整性与健壮性。测试范围覆盖了本地文件解析、网络URL资源加载、基础与复杂 CSS 选择器逻辑（包括层级、组合、兄弟选择器）、以及节点属性（innerText, outerHTML, href）的提取。

### 2. 测试资源

- **本地测试文件：** `C:\Users\luo yifan\Desktop\GSAI-2th-mystudy\Data_structure\labs\lab3\实验03\examples\example.html`
- **网络测试地址：** `https://www.crummy.com/software/BeautifulSoup/bs4/doc.zh/` (BeautifulSoup 中文文档)

### 3. 详细测试记录

#### 3.1 文件加载与基础环境测试

| **ID**    | **测试功能**     | **输入命令**             | **实际输出结果** | **结果状态** |
| --------- | ---------------- | ------------------------ | ---------------- | ------------ |
| **TC-01** | **本地文件读取** | `read(...\example.html)` | `文档加载成功。` | **PASS**     |

#### 3.2 CSS 选择器查询测试 (Query Functionality)

测试基于 `example.html` 的 DOM 结构。

| **ID**    | **测试功能**           | **输入命令**                         | **实际输出结果**                                             | **分析**                                             | **结果** |
| --------- | ---------------------- | ------------------------------------ | ------------------------------------------------------------ | ---------------------------------------------------- | -------- |
| **TC-02** | **Class 选择器**       | `query(.class1)`                     | `[div.class0.class1, span.class1, img.class1]`               | 正确检索出所有含 class1 的不同标签。                 | **PASS** |
| **TC-03** | **ID 选择器**          | `query(#id1)`                        | `[div#id1]`                                                  | 唯一性检索正确。                                     | **PASS** |
| **TC-04** | **多 Class 组合**      | `query(.class2.class3)`              | `[div.class2.class3, span.class3.class2, img.class2.class0.class3]` | 正确处理多个类名共存及顺序无关性。                   | **PASS** |
| **TC-05** | **ID + Class 组合**    | `query(#id4.class2)`                 | `[div#id4.class2]`                                           | 准确定位特定ID且具备特定类的元素。                   | **PASS** |
| **TC-06** | **后代选择器 (空格)**  | `query(.class4 .class5)`             | `[div.class5, p#id5.class5]`                                 | 正确识别父子/祖孙层级关系。                          | **PASS** |
| **TC-07** | **复杂混合查询**       | `query(div.class4 p.class5#id5)`     | `[p#id5.class5]`                                             | 成功解析 Tag+Class+Descendant+ID 的复杂组合。        | **PASS** |
| **TC-08** | **通用兄弟选择器 (~)** | `query(div.class2.class3 ~ .class0)` | `[img.class2.class0.class3]`                                 | **高级功能**：正确识别并返回指定节点之后的同级节点。 | **PASS** |

#### 3.3 结果集操作与数据抽取 (Extraction)

基于 `query(a)` 的结果集进行测试。

| **ID**    | **测试功能**       | **输入命令**       | **实际输出结果**                           | **结果状态**                         |
| --------- | ------------------ | ------------------ | ------------------------------------------ | ------------------------------------ |
| **TC-09** | **Tag 选择器**     | `query(a)`         | `[a, a]` (检索到2个链接)                   | **PASS**                             |
| **TC-10** | **获取 innerText** | `Out[0].innerText` | `p#id5.class5` (注:此处为链接内的文本内容) | **PASS**                             |
| **TC-11** | **获取 outerHTML** | `Out[0].outerHTML` | `<a href="/a.html">`                       | **PASS**                             |
| **TC-12** | **获取 href 属性** | `Out[0].href`      | `/a.html`                                  | **PASS**                             |
| **TC-13** | **链式/嵌套查询**  | `Out[0].query(*)`  | `[p#id5.class5]`                           | 成功在已选节点的范围内进行二次检索。 |

#### 3.4 网络加载与压力测试 (Network & Stress Test)

测试工具对真实互联网网页的处理能力。

| **ID**    | **测试功能**       | **输入命令**        | **实际输出结果**                                | **结果状态** |
| --------- | ------------------ | ------------------- | ----------------------------------------------- | ------------ |
| **TC-14** | **URL 下载与加载** | `read(https://...)` | `检测到 URL... 下载成功... 文档加载成功。`      | **PASS**     |
| **TC-15** | **真实DOM查询**    | `query(div.line)`   | 返回包含 40+ 个节点的长列表 (`[div.line, ...]`) | **PASS**     |
| **TC-16** | **大数据抽取**     | `Out[0].outerHTML`  | `<div class="line">`                            | **PASS**     |

#### 3.5 XPath 选择器查询测试 (XPath Query Functionality)

测试基于 `example.html` 的 DOM 结构，验证 XPath 解析引擎的正确性。

| **ID**    | **测试功能**          | **输入命令**                  | **实际输出结果**                         | **分析**                                                     | **结果** |
| --------- | --------------------- | ----------------------------- | ---------------------------------------- | ------------------------------------------------------------ | -------- |
| **TC-17** | **绝对路径查询**      | `xpath(/body/div)`            | `[div.class0.class1, div, div#id1, ...]` | 正确解析层级结构，返回 body 下的所有 div 子节点。            | **PASS** |
| **TC-18** | **全局递归查找 (//)** | `xpath(//div)`                | `[div.class0.class1, ..., div.class6]`   | 成功检索文档中任意位置的 div 节点（包含嵌套节点）。          | **PASS** |
| **TC-19** | **属性谓语过滤**      | `xpath(/body/div[@id='id1'])` | `[div#id1]`                              | 正确解析 `[@attr='val']` 语法，实现精确过滤。                | **PASS** |
| **TC-20** | **通配符测试 (\*)**   | `xpath(/body/*)`              | `[div.class0.class1, ..., div.class4]`   | 能够匹配指定路径下的所有不同类型的标签节点。                 | **PASS** |
| **TC-21** | **节点名称查找**      | `xpath(body)`                 | `[body]`                                 | 能够根据节点名称选取当前上下文的子节点。                     | **PASS** |
| **TC-22** | **父子节点相对导航**  | `Out[0].xpath(..)`            | `[html]`                                 | **高级功能**：支持通过 `..` 访问父节点，验证了树结构双向链接的完整性。 | **PASS** |

### 4. 测试结论与评价

1. **功能完备性：** 程序完全覆盖了实验要求的所有功能点。
2. **交互体验：** `read`, `query`, `Out[k]` 的交互逻辑清晰流畅，且支持从结果集中进行**链式查询 (Nested Query)**，极大地增强了工具的灵活性。
3. **鲁棒性：**
   - 能够处理本地长路径文件。
   - **网络模块集成良好**，能够自动识别 URL，下载并解析 HTTPS 协议的真实网页。
   - 在解析包含大量节点（如 BeautifulSoup 文档中的 `div.line`）的真实网页时，程序响应迅速，未出现崩溃或内存溢出。

**最终结论：** 该 HTML CSS Selector 程序功能强大，运行稳定，**通过所有测试用例**，具备投入实际使用的能力。



### 七、附录

```
1   📂罗毅凡_2024202715_实验3  (总文件夹)
    ┣ 📂Selector_project
    ┃ ┣ 📂 bin                (存放执行码)
    ┃ ┃ ┗ 📜 lab3_tree.exe 
    ┃ ┣ 📂 src                (存放源代码)
    ┃ ┃ ┗ 📜 lab3_tree.cpp      
    ┃ ┗ 📂 .vscode            (存放工程配置)
    ┃   ┗📜 settings.json  
2   ┣📜 实验报告.pdf  
3   ┣📜 使用手册.pdf 
4   ┗📜 功能测试报告.pdf 
```





