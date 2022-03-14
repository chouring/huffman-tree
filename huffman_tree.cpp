#include<iostream>
#include<unordered_map>

using namespace std;

//常量
const int INF = 0x3f3f3f3f; //大整数
const int BUFSIZE = 3;//buf的size,暂留空间
const int MAXN = 1e3; //暂定的最大输入字节数
const int NAME_SIZE = 1e3;
const int N = 1e4;
//数据结构
typedef struct Node{
    int weight;
    int parent, lchild, rchild;
    Node() {
        parent = lchild = rchild = weight = -1;
    }
}*HuffmanTree; //哈夫曼树节点类型
char name[NAME_SIZE];//输入文件名
char input[MAXN]; //从A中读入的数据，8bit一个单元
char *datab; //编码后B中数据,string方便处理，后续转换为char*
char datac[MAXN];//从文件B中读的数据
char dataout[MAXN];//写入C的数组
int doc_cnt = 0;//dataout的尾指针
int cnt; //B中字符总个数
int byte_size = 0;//从A中读入数据的Byte数
int v[256],w[256], k = 0;//v[i]:第i个待编码的字节;w[i]:第i个待编码的字节出现的次数，k为尾下标
char buf[BUFSIZE];//读入的每个byte
unordered_map<int,int> h;//哈希表，记录每个字节是否出现，以及在v和w数组中对应的下标
unordered_map<string, int> btoc;//每个字符串映射到原字节
string code_table[256]; //Huffman编码表
int son[N][2], cntt[N], idx; //复原哈夫曼树的存储结构

//函数
void read_from_a(); //从文件A读入数据
void init_vw();//初始化v和w数组
void Select(HuffmanTree HT, int end, int* s1, int* s2);//从HT的[0,end]中选出weight最小的两个Node赋给s1和s2指针
void create_huffman_tree(HuffmanTree& HT, int n, int* w);//创建哈夫曼树
void dfs(HuffmanTree t, int u, string path, string code_table[]);//搜索出每个叶节点的编码
void create_codetable(HuffmanTree t, string code_table[]);//生成Huffman编码表
void code();//编码函数
void decode();//解码函数
void recovery(string code_table[]);//恢复哈夫曼树；
void insert(string str);//插入字符串
int query(string str);//删除字符串


int main() {
    cout << "输入待编码的文件名，请输入：";
    cin >> name;
    read_from_a();
    init_vw();
    HuffmanTree t;
    create_huffman_tree(t, k, w);
    create_codetable(t, code_table);
    cout << "编码输入1，解码输入2，请输入：";
    int cho;
    cin >> cho;
    if (cho == 1)
    {
        code();
        cout << "编码完成，编码结果已输出到文件B.txt\n";
        system("pause");
    }
    else {
        cout << "即将开始解码文件B.txt,";
        system("pause");
        decode();
        recovery(code_table);
        cout << "解码完成，解码结果已输出到文件C.txt\n";
        system("pause");
    }
	return 0;
}

void read_from_a() { 
    FILE* fpa;
    fopen_s(&fpa, name, "rb");
    while (!feof(fpa)) {
        fread(buf, 1, 1, fpa);
        if(!feof(fpa))input[byte_size++] = *buf - 0;
    }
    fclose(fpa);
}
void init_vw() {
    for (int i = 0; i < byte_size; i++) {
        if (h.count(input[i])) {
            w[h[input[i]]]++;
        }
        else {
            h[input[i]] = k;
            v[k] = input[i], w[k] = 1, k++;
        }
    }
}
void Select(HuffmanTree HT, int end, int* s1, int* s2) 
{
    int i, first = INF, second = INF;
    *s1 = *s2 = 0;
    for (i = 1; i <= end; ++i) {
        if (HT[i].parent != -1) continue; // 不是叶子节点就跳过
        if (HT[i].weight < first) {
            second = first;
            *s2 = *s1;
            first = HT[i].weight;
            *s1 = i;
        }
        else if (HT[i].weight < second) {
            second = HT[i].weight;
            *s2 = i;
        }
    }
}
void create_huffman_tree(HuffmanTree& HT, int n, int* w) {
    if (n <= 1) return;
    int m = 2 * n - 1;
    HuffmanTree ht = (Node*)calloc(m + 1, sizeof(Node));
    for (int i = 0; i < 2 * n; i++) {
        ht[i].lchild = ht[i].rchild = ht[i].parent = ht[i].weight = -1;
    }
    int i;
    for (i = 0; i < n; ++i) {
        ht[i].weight = w[i];
        //cout << ht[i].weight << endl;
    }

    int s1, s2;
    for (i = n; i < m; ++i) {
        Select(ht, i - 1, &s1, &s2);
        ht[s1].parent = i, ht[s2].parent = i;
        ht[i].lchild = s1, ht[i].rchild = s2;
        ht[i].weight = ht[s1].weight + ht[s2].weight;
    }
    HT = ht;
}
void dfs(HuffmanTree t, int u, string path, string code_table[]) { 
    if (t[u].lchild == -1 && t[u].rchild == -1) {
        code_table[u] = path;
        //cout << "编号"<<u << "的字节为"<<v[u]<<"出现次数为"<<w[u]<<"编码为" << path << endl;
        return;
    }
    dfs(t, t[u].lchild, path + "0", code_table);
    dfs(t, t[u].rchild, path + "1", code_table);
}
void create_codetable(HuffmanTree t, string code_table[]) {
    dfs(t, 2 * k - 2, "", code_table);
}
void code() {
    string dex;
    for (int i = 0; i < byte_size; i++) {
        dex += code_table[h[input[i]]];
    }
    cnt = dex.size();
    datab = const_cast<char*>(dex.c_str());
    FILE* fpb;
    fopen_s(&fpb, "B.txt", "wb");
    fwrite(datab, 1, cnt, fpb);
    fclose(fpb);
}
void decode() {
    for (int i = 0; i < k; i++) btoc[code_table[i]] = i;
    FILE* fpc , *fpout;
    fopen_s(&fpc, "B.txt", "rb");
    string dex3;
    for (int i = 0; i < byte_size; i++) {
        dex3 += code_table[h[input[i]]];
    }
    cnt = dex3.size();
    fread(datac, 1, cnt, fpc);
    string dex;
    for (int i = 0; i < cnt; i++) {
        dex += datac[i];
        if (btoc.count(dex)){
            dataout[doc_cnt++] = (char)v[btoc[dex]];
            dex = "";
        }
    }
    fopen_s(&fpout, "C.txt", "wb");
    fwrite(dataout, 1, doc_cnt, fpout);
}
void insert(string str)
{
    int p = 0;
    for (int i = 0; i < str.size(); i++)
    {
        int u = str[i] - 'a';
        if (!son[p][u]) son[p][u] = ++idx;
        p = son[p][u];
    }
    cntt[p] ++;
}
int query(string str)
{
    int p = 0;
    for (int i = 0; i < str.size(); i++)
    {
        int u = str[i] - 'a';
        if (!son[p][u]) return 0;
        p = son[p][u];
    }
    return cntt[p];
}
void recovery(string code_table[]) {
    for (int i = 0; i < k; i++) {
        string cur = code_table[i];
        insert(cur);
    }
}
