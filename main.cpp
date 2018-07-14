/*
 * PL/0 complier program implemented in C
 *
 * The program has been tested on Visual Studio 2010
 *
 * 使用方法：
 * 运行后输入PL/0源程序文件名
 * 回答是否输出虚拟机代码
 * 回答是否输出符号表
 * fcode.txt输出虚拟机代码
 * foutput.txt输出源文件、出错示意（如有错）和各行对应的生成代码首地址（如无错）
 * fresult.txt输出运行结果
 * ftable.txt输出符号表
 */

//TODO 类型检查，主要包括赋值时的类型检查和return时的赋值检查

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <iostream>
#include <vector>
#include <string>

#define bool int
#define true 1
#define false 0

#define norw 20       /* 保留字个数 */
#define txmax 100     /* 符号表容量 */
#define nmax 14       /* 数字的最大位数 */
#define al 10         /* 标识符的最大长度 */
#define maxerr 30     /* 允许的最多错误数 */
#define amax 2048     /* 地址上界*/
#define levmax 3      /* 最大允许过程嵌套声明层数*/
#define cxmax 200     /* 最多的虚拟机代码数 */
#define stacksize 500 /* 运行时数据栈元素最多为500个 */

/* 符号 */
enum symbol {
    nul,         ident,     number,     plus,      minus,
    times,       slash,     oddsym,     eql,       neq,
    lss,         leq,       gtr,        geq,       lparen,
    rparen,      comma,     semicolon,  period,    becomes,
    beginsym,    endsym,    ifsym,      thensym,   whilesym,
    writesym,    readsym,   dosym,      callsym,   constsym,
    varsym,      procsym,   elsesym,    forsym,    stepsym,
    untilsym,    returnsym, timeseql,   slasheql,  numberand,
    numberor,    logicor,   logicnot,   selfplus,  selfminus,
    charval,     charsym,   realsym,    lsquare,   rsquare
};

#define symnum 50

/* 符号表中的类型 */
enum object {
    constant,
    variable,
    procedure,
    charvar,
    realvar,
    variable_array
};

/* 虚拟机代码指令 */
enum fct {
    lit,       opr,     lod,
    real_sto,  cal,     ini,
    jmp,       jpc,     int_sto,
    array_lod, array_sto
};

#define fctnum 11

/* 虚拟机代码结构 */
struct instruction
{
    enum fct f; /* 虚拟机代码指令 */
    int l;      /* 引用层与声明层的层次差 */
    float a;      /* 根据f的不同而不同 */
};


bool listswitch ;   /* 显示虚拟机代码与否 */
bool tableswitch ;  /* 显示符号表与否 */
char ch;            /* 存放当前读取的字符，getch 使用 */
enum symbol sym;    /* 当前的符号 */
char id[al+1];      /* 当前ident，多出的一个字节用于存放0 */
float num;            /* 当前number */
int cc, ll;         /* getch使用的计数器，cc表示当前字符(ch)的位置 */
int cx;             /* 虚拟机代码指针, 取值范围[0, cxmax-1]*/
char line[81];      /* 读取行缓冲区 */
char a[al+1];       /* 临时符号，多出的一个字节用于存放0 */
struct instruction code[cxmax]; /* 存放虚拟机代码的数组 */
char word[norw][al];        /* 保留字 */
enum symbol wsym[norw];     /* 保留字对应的符号值 */
enum symbol ssym[256];      /* 单字符的符号值 */
char mnemonic[fctnum][10];   /* 虚拟机代码指令名称 */
bool declbegsys[symnum];    /* 表示声明开始的符号集合 */
bool statbegsys[symnum];    /* 表示语句开始的符号集合 */
bool facbegsys[symnum];     /* 表示因子开始的符号集合 */
char ch_value;              /* 存放当前的char类型数据 */
object var_type;            /* 上一个处理的变量的类型，用于write语句识别类型 */
int param_adr;   /* 记录传入参数的地址，处于调用栈栈顶　*/

/* 符号表结构 */
struct tablestruct
{
    char name[al];      /* 名字 */
    enum object kind;   /* 类型：const，var或procedure */
    int val;            /* 数值，仅const使用 */
    int level;          /* 所处层，仅const不使用 */
    int adr;            /* 地址，仅const不使用 */
    int size;           /* 需要分配的数据区空间 */
    int params_num;     /* procedure的参数个数 */
};

struct tablestruct table[txmax]; /* 符号表 */

struct parameter {     /* 用于记录过程声明时的参数名和参数类型 */
    std::string name;
    object var_type;
};

std::vector<parameter> procedure_params; /* 用于记录过程声明时的参数名和参数类型,使用后清空 */

FILE* fin;      /* 输入源文件 */
FILE* ftable;   /* 输出符号表 */
FILE* fcode;    /* 输出虚拟机代码 */
FILE* foutput;  /* 输出文件及出错示意（如有错）、各行对应的生成代码首地址（如无错） */
FILE* fresult;  /* 输出执行结果 */
char fname[al];
int err;        /* 错误计数器 */

void error(int n);
void getsym();
void getch();
void init();
void gen(enum fct x, int y, float z);
void test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
void block(int lev, int tx, bool* fsys);
void interpret();
void factor(bool* fsys, int* ptx, int lev);
void term(bool* fsys, int* ptx, int lev);
void condition(bool* fsys, int* ptx, int lev);
void expression(bool* fsys, int* ptx, int lev);
void statement(bool* fsys, int* ptx, int lev);
void listcode(int cx0);
void listall();
void vardeclaration(int* ptx, int lev, int* pdx);
void constdeclaration(int* ptx, int lev, int* pdx);
int position(char* idt, int tx);
void enter(enum object k, int* ptx, int lev, int* pdx, int elem_num, bool is_param, const char* name);
int base(int l, float* s, int b);
void chardeclaration(int* ptx, int lev, int* pdx);
void realdeclaration(int* ptx, int lev, int* pdx);
void var_arry_declaration(int* ptx,int lev,int* pdx, int elem_num);
void procedure_var_declaration(int *ptx, int lev, int* pdx);

/* 主程序开始 */
int main()
{
    bool nxtlev[symnum];

    printf("Input pl/0 file?   ");
    //scanf("%s", fname);     /* 输入文件名 */
    strcpy(fname, "pl0.txt");

    std::cout << fname << std::endl;
    if ((fin = fopen(fname, "r")) == NULL)
    {
        printf("Can't open the input file!\n");
        exit(1);
    }

    ch = fgetc(fin);
    if (ch == EOF)
    {
        printf("The input file is empty!\n");
        fclose(fin);
        exit(1);
    }
    rewind(fin);

    if ((foutput = fopen("foutput.txt", "w")) == NULL)
    {
        printf("Can't open the output file!\n");
        exit(1);
    }

    if ((ftable = fopen("ftable.txt", "w")) == NULL)
    {
        printf("Can't open ftable.txt file!\n");
        exit(1);
    }

    printf("List object codes?(Y/N)");  /* 是否输出虚拟机代码 */
    scanf("%s", fname);
    listswitch = (fname[0]=='y' || fname[0]=='Y');

    printf("List symbol table?(Y/N)");  /* 是否输出符号表 */
    scanf("%s", fname);
    tableswitch = (fname[0]=='y' || fname[0]=='Y');

    printf("3115005077 杨哲\n");

    init();     /* 初始化 */
    err = 0;
    cc = ll = cx = 0;
    ch = ' ';

    getsym();

    addset(nxtlev, declbegsys, statbegsys, symnum);
    nxtlev[period] = true;
    block(0, 0, nxtlev);    /* 处理分程序 */

    if (sym != period)
    {
        error(9);
    }

    if (err == 0)
    {
        printf("\n===Parsing success!===\n");
        fprintf(foutput,"\n===Parsing success!===\n");

        if ((fcode = fopen("fcode.txt", "w")) == NULL)
        {
            printf("Can't open fcode.txt file!\n");
            exit(1);
        }

        if ((fresult = fopen("fresult.txt", "w")) == NULL)
        {
            printf("Can't open fresult.txt file!\n");
            exit(1);
        }

        listall();   /* 输出所有代码 */
        fclose(fcode);

        interpret();    /* 调用解释执行程序 */
        fclose(fresult);
    }
    else
    {
        printf("\n%d errors in pl/0 program!\n",err);
        fprintf(foutput,"\n%d errors in pl/0 program!\n",err);
    }

    fclose(ftable);
    fclose(foutput);
    fclose(fin);

    return 0;
}

/*
 * 初始化
 */
void init()
{
    int i;

    /* 设置单字符符号 */
    for (i=0; i<=255; i++)
    {
        ssym[i] = nul;
    }
    ssym['+'] = plus;
    ssym['-'] = minus;
    ssym['*'] = times;
    ssym['/'] = slash;
    ssym['('] = lparen;
    ssym[')'] = rparen;
    ssym['='] = eql;
    ssym[','] = comma;
    ssym['.'] = period;
//    ssym['#'] = neq;
    ssym[';'] = semicolon;
    ssym['['] = lsquare;
    ssym[']'] = rsquare;
    ssym['!'] = logicnot;

    /* 设置保留字名字,按照字母顺序，便于二分查找 */
    strcpy(&(word[0][0]), "begin");
    strcpy(&(word[1][0]), "call");
    strcpy(&(word[2][0]), "char");
    strcpy(&(word[3][0]), "const");
    strcpy(&(word[4][0]), "do");
    strcpy(&(word[5][0]), "else");
    strcpy(&(word[6][0]), "end");
    strcpy(&(word[7][0]), "for");
    strcpy(&(word[8][0]), "if");
    strcpy(&(word[9][0]), "odd");
    strcpy(&(word[10][0]), "procedure");
    strcpy(&(word[11][0]), "read");
    strcpy(&(word[12][0]), "real");
    strcpy(&(word[13][0]), "return");
    strcpy(&(word[14][0]), "step");
    strcpy(&(word[15][0]), "then");
    strcpy(&(word[16][0]), "until");
    strcpy(&(word[17][0]), "var");
    strcpy(&(word[18][0]), "while");
    strcpy(&(word[19][0]), "write");

    /* 设置保留字符号 */
    wsym[0] = beginsym;
    wsym[1] = callsym;
    wsym[2] = charsym;
    wsym[3] = constsym;
    wsym[4] = dosym;
    wsym[5] = elsesym;
    wsym[6] = endsym;
    wsym[7] = forsym;
    wsym[8] = ifsym;
    wsym[9] = oddsym;
    wsym[10] = procsym;
    wsym[11] = readsym;
    wsym[12] = realsym;
    wsym[13] = returnsym;
    wsym[14] = stepsym;
    wsym[15] = thensym;
    wsym[16] = untilsym;
    wsym[17] = varsym;
    wsym[18] = whilesym;
    wsym[19] = writesym;

    /* 设置指令名称 */
    strcpy(&(mnemonic[lit][0]), "lit");
    strcpy(&(mnemonic[opr][0]), "opr");
    strcpy(&(mnemonic[lod][0]), "lod");
    strcpy(&(mnemonic[real_sto][0]), "real_sto");
    strcpy(&(mnemonic[cal][0]), "cal");
    strcpy(&(mnemonic[ini][0]), "int");
    strcpy(&(mnemonic[jmp][0]), "jmp");
    strcpy(&(mnemonic[jpc][0]), "jpc");
    strcpy(&(mnemonic[int_sto][0]), "int_sto");
    strcpy(&(mnemonic[array_lod][0]), "array_lod");
    strcpy(&(mnemonic[array_sto][0]), "array_sto");

    /* 设置符号集 */
    for (i=0; i<symnum; i++)
    {
        declbegsys[i] = false;
        statbegsys[i] = false;
        facbegsys[i] = false;
    }

    /* 设置声明开始符号集 */
    declbegsys[constsym] = true;
    declbegsys[varsym] = true;
    declbegsys[procsym] = true;
    declbegsys[charsym] = true;
    declbegsys[realsym] = true;

    /* 设置语句开始符号集 */
    statbegsys[beginsym] = true;
    statbegsys[callsym] = true;
    statbegsys[ifsym] = true;
    statbegsys[whilesym] = true;
//    statbegsys[selfplus] = true;
//    statbegsys[selfminus] = true;

    /* 设置因子开始符号集 */
    facbegsys[ident] = true;
    facbegsys[number] = true;
    facbegsys[lparen] = true;
    facbegsys[selfplus] = true;
    facbegsys[selfminus] = true;
    facbegsys[charval] = true;
    facbegsys[callsym] = true;
}

/*
 * 用数组实现集合的集合运算
 */
int inset(int e, bool* s)
{
    return s[e];
}

int addset(bool* sr, bool* s1, bool* s2, int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        sr[i] = s1[i]||s2[i];
    }
    return 0;
}

int subset(bool* sr, bool* s1, bool* s2, int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        sr[i] = s1[i]&&(!s2[i]);
    }
    return 0;
}

int mulset(bool* sr, bool* s1, bool* s2, int n)
{
    int i;
    for (i=0; i<n; i++)
    {
        sr[i] = s1[i]&&s2[i];
    }
    return 0;
}

/*
 *  出错处理，打印出错位置和错误编码
 */
void error(int n)
{
    char space[81];
    memset(space,32,81);

    space[cc-1]=0; /* 出错时当前符号已经读完，所以cc-1 */

    printf("**%s^%d\n", space, n);
    fprintf(foutput,"**%s^%d\n", space, n);

    err = err + 1;
    if (err > maxerr)
    {
        exit(1);
    }
}

/*
 * 过滤空格，读取一个字符
 * 每次读一行，存入line缓冲区，line被getsym取空后再读一行
 * 被函数getsym调用
 */
void getch()
{
    if (cc == ll) /* 判断缓冲区中是否有字符，若无字符，则读入下一行字符到缓冲区中 */
    {
        if (feof(fin))
        {
            printf("Program is incomplete!\n");
            exit(1);
        }
        ll = 0;
        cc = 0;
        printf("%d ", cx);
        fprintf(foutput,"%d ", cx);
        ch = ' ';
        while (ch != 10)
        {
            if (EOF == fscanf(fin,"%c", &ch))
            {
                line[ll] = 0;
                break;
            }

            printf("%c", ch);
            fprintf(foutput, "%c", ch);
            line[ll] = ch;
            ll++;
        }
    }
    ch = line[cc];
    cc++;
}

/*
 * 词法分析，获取一个符号
 */
void getsym()
{
    int i,j,k;

    while (ch == ' ' || ch == 10 || ch == 9)    /* 过滤空格、换行和制表符 */
    {
        getch();
    }


    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) /* 当前的单词是标识符或是保留字 */
    {
        k = 0;
        do {
            if(k < al)
            {
                a[k] = ch;
                k++;
            }
            getch();
        } while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'));
        a[k] = 0;
        strcpy(id, a);
        i = 0;
        j = norw - 1;
        do {    /* 搜索当前单词是否为保留字，使用二分法查找 */
            k = (i + j) / 2;
            if (strcmp(id,word[k]) <= 0)
            {
                j = k - 1;
            }
            if (strcmp(id,word[k]) >= 0)
            {
                i = k + 1;
            }
        } while (i <= j);

        if (i-1 > j) /* 当前的单词是保留字 */
        {
            sym = wsym[k];
        }
        else /* 当前的单词是标识符 */
        {
            sym = ident;
        }
    }
    else
    {
        if (ch >= '0' && ch <= '9') /* 当前的单词是数字 */
        {
            k = 0;
            num = 0;
            float count = 0.1;
            sym = number;
            do {
                if(ch == '.') {
                    getch();
                    //将sym改为realsym
                    while(ch >= '0' && ch <= '9') {
                        num = num + (ch-'0') * count;
                        k++;
                        count *= 0.1;
                        getch();
                    }
                }
                else {
                    num = 10 * num + ch - '0';
                    k++;
                    getch();
                }

            } while ((ch >= '0' && ch <= '9') || ch == '.'); /* 获取数字的值 */
            k--;
            if (k > nmax) /* 数字位数太多 */
            {
                error(30);
            }
        }
        else
        {
            if (ch == ':')      /* 检测赋值符号 */
            {
                getch();
                if (ch == '=')
                {
                    sym = becomes;
                    getch();
                }
                else
                {
                    sym = nul;  /* 不能识别的符号 */
                }
            }
            else if(ch == '*') {  // *= and *
                getch();
                if(ch == '=') {
                    sym = timeseql;
                    getch();
                    printf("---sym: *=\n");
                }
                else {
                    sym = times;
                }
            }
            else if(ch == '/') {   // /= and / and /*
                getch();
                if(ch == '=') {
                    sym = slasheql;
                    getch();
                    printf("---sym: /=\n");
                }
                else if(ch == '*') {   // 代码注释,跳过注释再getsym()
                    while(true) {
                        getch();
                        if(ch == '*') {    //TODO 增加处理没有'*/'的部分
                            getch();
                            if(ch == '/') {

                                getch();
                                getsym();
                                return;
                            }
                        }
                    }
                }
                else {
                    sym = slash;
                }
            }
            else if(ch == '&') {
                sym = numberand;
                printf("---sym: &\n");
                getch();
            }
            else if(ch == '!') {
                getch();
                sym = logicnot;
                printf("---sym: !\n");
            }
            else if(ch == '|') {
                getch();
                if(ch == '|') {
                    sym = logicor;
                    getch();
                    printf("---sym: ||\n");
                }
            }
            else if(ch == '+') {
                getch();
                if(ch == '+') {   // 自增
                    sym = selfplus;
                    getch();
                } else {
                    sym = plus;
                }
            }
            else if(ch == '-') {
                getch();
                if(ch == '-') {
                    sym = selfminus;
                    getch();
                } else {
                    sym = minus;
                }
            }
            else if(ch == '\'') {
                getch();
                ch_value = ch;
                getch();
                if(ch == '\'')
                    sym = charval;
                else
                    error(35);   // 最后一个单引号不匹配
                getch();
            }
            else
            {
                if (ch == '<')       /* 检测小于或小于等于符号 */
                {
                    getch();
                    if (ch == '=')
                    {
                        sym = leq;
                        getch();
                    }else if(ch =='>'){//替代#
                        sym = neq;
                        getch();
                    }
                    else
                    {
                        sym = lss;
                    }
                }
                else
                {
                    if (ch == '>')       /* 检测大于或大于等于符号 */
                    {
                        getch();
                        if (ch == '=')
                        {
                            sym = geq;
                            getch();
                        }
                        else
                        {
                            sym = gtr;
                        }
                    }
                    else  //单字符符号处理
                    {
                        sym = ssym[ch];     /* 当符号不满足上述条件时，全部按照单字符符号处理 */
                        if (sym != period)
                        {
                            getch();
                        }

                    }
                }
            }
        }
    }
}

/*
 * 生成虚拟机代码
 *
 * x: instruction.f;
 * y: instruction.l;
 * z: instruction.a;
 */
void gen(enum fct x, int y, float z )
{
    if (cx >= cxmax)
    {
        printf("Program is too long!\n");   /* 生成的虚拟机代码程序过长 */
        exit(1);
    }
    if ( z >= amax)
    {
        printf("Displacement address is too big!\n");   /* 地址偏移越界 */
        exit(1);
    }
    code[cx].f = x;
    code[cx].l = y;
    code[cx].a = z;
    cx++;
}


/*
 * 测试当前符号是否合法
 *
 * 在语法分析程序的入口和出口处调用测试函数test，
 * 检查当前单词进入和退出该语法单位的合法性
 *
 * s1:  需要的单词集合
 * s2:  如果不是需要的单词，在某一出错状态时，
 *      可恢复语法分析继续正常工作的补充单词符号集合
 * n:   错误号
 */
void test(bool* s1, bool* s2, int n)
{
    if (!inset(sym, s1))
    {
        error(n);

        /* 当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合 */
        while ((!inset(sym,s1)) && (!inset(sym,s2)))
        {
            getsym();
        }
    }
}

/*
 * 编译程序主体
 *
 * lev:    当前分程序所在层
 * tx:     符号表当前尾指针
 * fsys:   当前模块后继符号集合
 */
void block(int lev, int tx, bool* fsys)
{
    int i;

    int dx;                 /* 记录数据分配的相对地址 */
    int tx0;                /* 保留初始tx */
    int cx0;                /* 保留初始cx */
    bool nxtlev[symnum];    /* 在下级函数的参数中，符号集合均为值参，但由于使用数组实现，
                               传递进来的是指针，为防止下级函数改变上级函数的集合，开辟新的空间
                               传递给下级函数*/

    dx = 4;                 /* 三个空间用于存放静态链SL、动态链DL和返回地址RA、参数个数  */
    tx0 = tx;               /* 记录本层标识符的初始位置 */
    table[tx].adr = cx;     /* 记录当前层代码的开始位置 */
    gen(jmp, 0, 0);         /* 产生跳转指令，跳转位置未知暂时填0,等到生成变量的空间后回填　*/


    if (lev > levmax)        /* 嵌套层数过多 */
    {
        error(32);
    }

    std::vector<parameter> local_procedure_params = procedure_params;
    procedure_params.clear();   //清空形参列表
    for(const auto& param : local_procedure_params) {   //声明形参
        switch (param.var_type) {
            case variable:
                enter(variable, &tx, lev, &dx, 0, true, param.name.c_str());
                break;

            case realvar:
                enter(realvar, &tx, lev, &dx, 0, true, param.name.c_str());
                break;

            case charvar:
                enter(charvar, &tx, lev, &dx, 0, true, param.name.c_str());
                break;
        }
    }


    do {

        if (sym == constsym)    /* 遇到常量声明符号，开始处理常量声明 */
        {
            getsym();

            do {
                constdeclaration(&tx, lev, &dx);    /* dx的值会被constdeclaration改变，使用指针 */
                while (sym == comma)  /* 遇到逗号继续定义常量 */
                {
                    getsym();
                    constdeclaration(&tx, lev, &dx);
                }
                if (sym == semicolon) /* 遇到分号结束定义常量 */
                {
                    getsym();
                }
                else
                {
                    error(5);   /* 漏掉了分号 */
                }
            } while (sym == ident);
        }

        if(sym == charsym) {  /* char类型变量声明 */
            getsym();

            do {
                chardeclaration(&tx, lev, &dx);
                while (sym == comma)
                {
                    getsym();
                    chardeclaration(&tx, lev, &dx);
                }
                if (sym == semicolon)
                {
                    getsym();
                }
                else
                {
                    error(5); /* 漏掉了分号 */
                }
            } while (sym == ident);
        }

        if (sym == realsym)      /* real型变量声明 */
        {
            getsym();

            do {
                realdeclaration(&tx, lev, &dx);
                while (sym == comma)
                {
                    getsym();
                    realdeclaration(&tx, lev, &dx);
                }
                if (sym == semicolon)
                {
                    getsym();
                }
                else
                {
                    error(5); /* 漏掉了分号 */
                }
            } while (sym == ident);
        }

        if (sym == varsym)      /* 遇到变量声明符号，开始处理变量声明 */
        {
            getsym();
            if (sym == ident) {
                getsym();
                if (sym == lsquare) {  //　var数组声明
                    getsym();
                    if (sym == number) {
                        getsym();
                        if (sym == rsquare) {
                            var_arry_declaration(&tx, lev, &dx, num);
                            getsym();
                        } else {
                            error(37);   //　数组声明时缺少']'
                        }
                    } else {
                        error(36); // 数组声明时'['后没有number
                    }
                }
                else {   // var变量声明
                    do {
                        vardeclaration(&tx, lev, &dx);
                        while (sym == comma)
                        {
                            getsym();
                            if(sym == ident) {
                                vardeclaration(&tx, lev, &dx);
                                getsym();
                            }
                            else
                                error(41);   /* ','后没有跟标识符 */
                        }

                        if (sym == semicolon)
                        {
                            getsym();
                        }
                        else
                        {
                            error(5); /* 漏掉了分号 */
                        }
                    } while (sym == ident);
                }
            } else
                error(4);  /* var后面应是标识符 */
        }

        while (sym == procsym) /* 遇到过程声明符号，开始处理过程声明 */
        {
            getsym();

            if (sym == ident)
            {
                enter(procedure, &tx, lev, &dx, 0, false, nullptr);    /* 填写符号表 */
                getsym();
                if(sym == lparen) {
                    procedure_var_declaration(&tx, lev, &dx);
                    if(sym == rparen) {
                        getsym();
                    }
                    else
                        error(42);
                }
                else {
                    error(42); /* 过程声明缺少括号　*/
                }
            }
            else
            {
                error(4);   /* procedure后应为标识符 */
            }

            if (sym == semicolon)
            {
                getsym();
            }
            else
            {
                error(5);   /* 漏掉了分号 */
            }

            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
            nxtlev[semicolon] = true;
            block(lev + 1, tx, nxtlev); /* 递归调用 */

            if(sym == semicolon)
            {
                getsym();
                memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
                nxtlev[ident] = true;
                nxtlev[procsym] = true;
                test(nxtlev, fsys, 6);
            }
            else
            {
                error(5);   /* 漏掉了分号 */
            }
        }

        memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
        nxtlev[ident] = true;
        test(nxtlev, declbegsys, 7);
    } while (inset(sym, declbegsys));   /* 直到没有声明符号 */

    code[table[tx0].adr].a = cx;    /* 把前面生成的跳转语句的跳转位置改成当前位置 */
    table[tx0].adr = cx;            /* 记录当前过程代码地址 */
    table[tx0].size = dx;           /* 声明部分中每增加一条声明都会给dx增加1，声明部分已经结束，dx就是当前过程数据的size */
    table[tx0].params_num = local_procedure_params.size(); /* 形参个数，用于call语句时检查参数个数　*/
    cx0 = cx;
    gen(ini, 0, dx);                /* 生成指令，此指令执行时在数据栈中为被调用的过程开辟dx个单元的数据区 */

    gen(lit, 0, local_procedure_params.size());    /* 填充形参个数　*/
    gen(int_sto, lev, 3);

    for(int i = 0; i < local_procedure_params.size(); i++) {    /* 传递参数 */
        gen(lod, 0, -static_cast<int>(local_procedure_params.size()) + i);
        gen(int_sto, 0, 4 + i);
    }

    if (tableswitch)        /* 输出符号表 */
    {
        for (i = 1; i <= tx; i++)
        {
            switch (table[i].kind)
            {
                case constant:
                    printf("    %d const %s ", i, table[i].name);
                    printf("val=%d\n", table[i].val);
                    fprintf(ftable, "    %d const %s ", i, table[i].name);
                    fprintf(ftable, "val=%d\n", table[i].val);
                    break;

                case variable:
                    printf("    %d var   %s ", i, table[i].name);
                    printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
                    fprintf(ftable, "    %d var   %s ", i, table[i].name);
                    fprintf(ftable, "lev=%d addr=%d\n", table[i].level, table[i].adr);
                    break;

                case procedure:
                    printf("    %d proc  %s ", i, table[i].name);
                    printf("lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
                    fprintf(ftable,"    %d proc  %s ", i, table[i].name);
                    fprintf(ftable,"lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
                    break;
            }
        }
        printf("\n");
        fprintf(ftable,"\n");
    }

    /* 语句后继符号为分号或end */
    memcpy(nxtlev, fsys, sizeof(bool) * symnum);    /* 每个后继符号集合都包含上层后继符号集合，以便补救 */
    nxtlev[semicolon] = true;
    nxtlev[endsym] = true;
    statement(nxtlev, &tx, lev);
    gen(opr, 0, 0);                     /* 每个过程出口都要使用的释放数据段指令 */
    memset(nxtlev, 0, sizeof(bool) * symnum);   /* 分程序没有补救集合 */
    test(fsys, nxtlev, 8);              /* 检测后继符号正确性 */
    listcode(cx0);                      /* 输出本分程序生成的代码 */
}

/*
 * 在符号表中加入一项
 *
 * k:      标识符的种类为const，var或procedure
 * ptx:    符号表尾指针的指针，为了可以改变符号表尾指针的值
 * lev:    标识符所在的层次
 * pdx:    dx为当前应分配的变量的相对地址，分配后要增加1
 *
 */
void enter(enum object k, int* ptx, int lev, int* pdx, int elem_num, bool is_param, const char* name)
{
    (*ptx)++;
    if(is_param)  //　如果是过程的形参填入的名字使用传入的名字
        strcpy(table[(*ptx)].name, name);
    else
        strcpy(table[(*ptx)].name, id); /* 符号表的name域记录标识符的名字 */
    table[(*ptx)].kind = k;
    switch (k)
    {
        case constant:  /* 常量 */
            if (num > amax)
            {
                error(31);  /* 常数越界 */
                num = 0;
            }
            table[(*ptx)].val = num; /* 登记常数的值 */
            break;

        case variable:  /* 变量 */
            table[(*ptx)].level = lev;
            table[(*ptx)].adr = (*pdx);
            (*pdx)++;
            break;

        case procedure: /* 过程 */
            table[(*ptx)].level = lev;
            break;

        case charvar: /* char */
            table[(*ptx)].level = lev;
            table[(*ptx)].adr = (*pdx);
            (*pdx)++;
            break;

        case realvar:
            table[(*ptx)].level = lev;
            table[(*ptx)].adr = (*pdx);
            (*pdx)++;
            break;

        case variable_array:
            table[(*ptx)].level = lev;
            table[(*ptx)].adr = (*pdx);
            table[(*ptx)].size = elem_num;
            (*pdx) += elem_num;
            break;
    }
}

/*
 * 查找标识符在符号表中的位置，从tx开始倒序查找标识符
 * 找到则返回在符号表中的位置，否则返回0
 *
 * id:    要查找的名字
 * tx:    当前符号表尾指针
 */
int position(char* id, int tx)
{
    int i;
    strcpy(table[0].name, id);
    i = tx;
    while (strcmp(table[i].name, id) != 0)
    {
        i--;
    }
    return i;
}

/*
 * 常量声明处理
 */
void constdeclaration(int* ptx, int lev, int* pdx)
{
    if (sym == ident)
    {
        getsym();
        if (sym == eql || sym == becomes)
        {
            if (sym == becomes)
            {
                error(1);   /* 把=写成了:= */
            }
            getsym();
            if (sym == number)
            {
                enter(constant, ptx, lev, pdx, 0, false, nullptr);
                getsym();
            }
            else
            {
                error(2);   /* 常量声明中的=后应是数字 */
            }
        }
        else
        {
            error(3);   /* 常量声明中的标识符后应是= */
        }
    }
    else
    {
        error(4);   /* const后应是标识符 */
    }
}

/*
 * 变量声明处理
 */
void vardeclaration(int* ptx,int lev, int* pdx)
{
    enter(variable, ptx, lev, pdx, 0, false, nullptr); // 填写符号表
}


/*
 * 变量数组声明处理, elem_num为数组元素的个数
 */
void var_arry_declaration(int* ptx,int lev, int* pdx, int elem_num)
{

    enter(variable_array, ptx, lev, pdx, elem_num, false, nullptr); // 填写符号表
    getsym();
}

/*
 * 字符型变量声明处理
 */
void chardeclaration(int* ptx, int lev, int* pdx) {
    if(sym == ident) {
        enter(charvar, ptx, lev, pdx, 0, false, nullptr);
        getsym();
    }
    else
        error(4);  /* char 后面应该是标识符 */
}


/*
 * 实型变量声明处理
 */
void realdeclaration(int* ptx, int lev, int* pdx) {
    if(sym == ident) {
        enter(realvar, ptx, lev, pdx, 0, false, nullptr);
        getsym();
    }
    else
        error(4);
}

/* 过程声明时的形参声明　*/
void procedure_var_declaration(int *ptx, int lev, int* pdx) {
    do {
        getsym();
        if(sym == varsym) {
            getsym();

            if(sym == ident) {
                procedure_params.push_back(parameter{id, variable});
                getsym();
            }
            else
                error(43); /* 形参声明时类型后面没有标识符 */
        }
        else if (sym == realsym){
            getsym();

            if(sym == ident) {
                procedure_params.push_back(parameter{id, realvar});
                getsym();
            }
            else
                error(43); /* 形参声明时类型后面没有标识符 */
        }
        else if(sym == charsym) {
            getsym();

            if(sym == ident) {
                procedure_params.push_back(parameter{id, charvar});
                getsym();
            }
            else
                error(43); /* 形参声明时类型后面没有标识符 */
        }
    } while (sym == comma);
}

/*
 * 输出目标代码清单
 */
void listcode(int cx0)
{
    int i;
    if (listswitch)
    {
        printf("\n");
        for (i = cx0; i < cx; i++)
        {
            printf("%d %s %d %f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}

/*
 * 输出所有目标代码
 */
void listall()
{
    int i;
    if (listswitch)
    {
        for (i = 0; i < cx; i++)
        {
            printf("%d %s %d %f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
            fprintf(fcode,"%d %s %d %f\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
        }
    }
}

/*
 * 语句处理
 */
void statement(bool* fsys, int* ptx, int lev)
{
    int i, cx1, cx2;
    bool nxtlev[symnum];

    if (sym == ident)   /* 准备按照赋值语句处理 or *= or /= or ++ or -- */
    {
        i = position(id, *ptx);/* 查找标识符在符号表中的位置 */
        if (i == 0)
        {
            error(11);  /* 标识符未声明 */
        }
        else
        {
            if(table[i].kind != variable && table[i].kind != charvar && table[i].kind != realvar && table[i].kind != variable_array)
            {
                error(12);  /* 赋值语句中，赋值号左部标识符应该是变量 */
                i = 0;
            }
            else {
                getsym();

                if(sym == lsquare) {
                    getsym();
                    expression(nxtlev, ptx, lev);

                    if(sym == rsquare) {
                        getsym();
                    }
                    else {
                       error(40); /* 格式错误，应是']' */
                    }
                }

                symbol tmp_sym = sym;
                if (sym == becomes || sym == timeseql || sym == slasheql) {
                    if (sym == becomes) {
                        getsym();
                    } else if (sym == timeseql || sym == slasheql) {
                        gen(lod, lev - table[i].level, table[i].adr);
                        getsym();
                    } else
                        error(13);
                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                    expression(nxtlev, ptx, lev);   /* 处理赋值符号右侧表达式 */

                    if (tmp_sym == timeseql) {
                        gen(opr, 0, 4);
                    } else if (tmp_sym == slasheql) {
                        gen(opr, 0, 5);
                    }

                    if (i != 0) {
                        /* expression将执行一系列指令，但最终结果将会保存在栈顶，执行sto命令完成赋值 */
                        switch (table[i].kind) {
                            case variable:
                                gen(int_sto, lev - table[i].level, table[i].adr);
                                break;

                            case charvar:
                                gen(int_sto, lev - table[i].level, table[i].adr);
                                break;

                            case realvar:
                                gen(real_sto, lev - table[i].level, table[i].adr);
                                break;

                            case variable_array:
                                gen(array_sto, lev - table[i].level, table[i].adr);
                                break;
                        }

                    }
                }
                else if(sym == selfplus || sym == selfminus) {  //后置++，　--
                    gen(lod, lev - table[i].level, table[i].adr);

                    if(sym == selfplus) {
                        gen(lit, 0, 1);    //入栈1
                        gen(opr, 0, 2);    //相加
                    }
                    else if(sym == selfminus) {
                        gen(lit, 0, 1);  //入栈1
                        gen(opr, 0, 3);  //相减
                    }
                    getsym();
                    gen(int_sto, lev - table[i].level, table[i].adr);
                }
            }
        }
    }
    else
    {
        if (sym == readsym) /* 准备按照read语句处理 */
        {
            getsym();
            if (sym != lparen)
            {
                error(34);  /* 格式错误，应是左括号 */
            }
            else
            {
                do {
                    getsym();
                    if (sym == ident)
                    {
                        i = position(id, *ptx); /* 查找要读的变量 */
                    }
                    else
                    {
                        i = 0;
                    }

                    if (i == 0)
                    {
                        error(35);  /* read语句括号中的标识符应该是声明过的变量 */
                    }
                    else
                    {
                        gen(opr, 0, 16);    /* 生成输入指令，读取值到栈顶 */
                        switch (table[i].kind) {
                            case variable:
                                gen(int_sto, lev-table[i].level, table[i].adr); /* 将栈顶内容送入变量单元中 */
                                break;

                            case realvar:
                                gen(real_sto, lev-table[i].level, table[i].adr);
                                break;
                        }

                    }
                    getsym();

                } while (sym == comma); /* 一条read语句可读多个变量 */
            }
            if(sym != rparen)
            {
                error(33);  /* 格式错误，应是右括号 */
                while (!inset(sym, fsys))   /* 出错补救，直到遇到上层函数的后继符号 */
                {
                    getsym();
                }
            }
            else
            {
                getsym();
            }
        }
        else
        {
            if (sym == writesym)    /* 准备按照write语句处理 */
            {
                getsym();
                if (sym == lparen)
                {
                    do {
                        getsym();
                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                        nxtlev[rparen] = true;
                        nxtlev[comma] = true;
                        expression(nxtlev, ptx, lev);   /* 调用表达式处理 */
                        gen(opr, var_type, 14);    /* 生成输出指令，输出栈顶的值 */
                        gen(opr, 0, 15);    /* 生成换行指令 */

                    } while (sym == comma);  /* 一条write可输出多个变量的值 */
                    if (sym != rparen)
                    {
                        error(33);  /* 格式错误，应是右括号 */
                    }
                    else
                    {
                        getsym();
                    }
                }

            }
            else
            {
                if (sym == callsym) /* 准备按照call语句处理 */
                {
                    getsym();
                    if (sym != ident)
                    {
                        error(14);  /* call后应为标识符 */
                    }
                    else
                    {
                        i = position(id, *ptx);
                        if (i == 0)
                        {
                            error(11);  /* 过程名未找到 */
                        }
                        else
                        {
                            if (table[i].kind == procedure)
                            {
                                getsym();
                                int params_num = 0;
                                if(sym == lparen) {
                                    getsym();

                                    while(sym != rparen) {
                                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                        nxtlev[rparen] = true;
                                        nxtlev[comma] = true;
                                        expression(nxtlev, ptx, lev);
                                        params_num++;   /* 统计参数个数　*/
                                        if(sym == comma) {
                                            getsym();
                                            if(sym == rparen)
                                                error(46);  /* 调用过程时存在多余的逗号 */
                                            continue;
                                        }
                                    }
                                    if(table[i].params_num != params_num)
                                        error(45);  /* 调用过程时传入参数个数不正确 */
                                }
                                else{
                                    error(44); /*　调用过程时括号缺失 */
                                }
                                gen(cal, lev-table[i].level, table[i].adr); /* 生成call指令 */
                            }
                            else
                            {
                                error(15);  /* call后标识符类型应为过程 */
                            }
                        }
                        getsym();
                    }
                }
                else
                {
                    if (sym == ifsym)   /* 准备按照if语句处理 */
                    {
                        getsym();
                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                        nxtlev[thensym] = true;
                        nxtlev[dosym] = true;   /* 后继符号为then或do */
                        condition(nxtlev, ptx, lev); /* 调用条件处理 */
                        if (sym == thensym)
                        {
                            getsym();
                        }
                        else
                        {
                            error(16);  /* 缺少then */
                        }
                        cx1 = cx;   /* 保存当前指令地址 */
                        gen(jpc, 0, 0); /* 生成条件跳转指令，跳转地址未知，暂时写0 */
                        statement(fsys, ptx, lev);  /* 处理then后的语句 */

                        //处理else
                        getsym();
                        if(sym == elsesym) {
                            getsym();
                            cx2 = cx;
                            gen(jmp, 0, 0);
                            code[cx1].a = cx;   //地址回填
                            statement(fsys, ptx, lev);
                            code[cx2].a = cx;
                        }
                        else {
                            code[cx1].a = cx;   /* 经statement处理后，cx为then后语句执行完的位置，它正是前面未定的跳转地址，此时进行回填 */
                        }
                    }
                    else
                    {
                        if (sym == beginsym)    /* 准备按照复合语句处理 */
                        {
                            getsym();
                            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                            nxtlev[semicolon] = true;
                            nxtlev[endsym] = true;  /* 后继符号为分号或end */

                            statement(nxtlev, ptx, lev);  /* 对begin与end之间的语句进行分析处理 */
                            /* 如果分析完一句后遇到语句开始符或分号，则循环分析下一句语句 */
                            while (inset(sym, statbegsys) || sym == semicolon)
                            {
                                if (sym == semicolon)
                                {
                                    getsym();
                                }
                                else
                                {
                                    error(10);  /* 缺少分号 */
                                }
                                statement(nxtlev, ptx, lev);
                            }
                            if(sym == endsym)
                            {
                                getsym();
                            }
                            else
                            {
                                error(17);  /* 缺少end */
                            }
                        }
                        else
                        {
                            if (sym == whilesym)    /* 准备按照while语句处理 */
                            {
                                cx1 = cx;   /* 保存判断条件操作的位置 */
                                getsym();
                                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                nxtlev[dosym] = true;   /* 后继符号为do */
                                condition(nxtlev, ptx, lev);    /* 调用条件处理 */
                                cx2 = cx;   /* 保存循环体的结束的下一个位置 */
                                gen(jpc, 0, 0); /* 生成条件跳转，但跳出循环的地址未知，标记为0等待回填 */
                                if (sym == dosym)
                                {
                                    getsym();
                                }
                                else
                                {
                                    error(18);  /* 缺少do */
                                }
                                statement(fsys, ptx, lev);  /* 循环体 */
                                gen(jmp, 0, cx1);   /* 生成条件跳转指令，跳转到前面判断条件操作的位置 */
                                code[cx2].a = cx;   /* 回填跳出循环的地址 */
                            }
                            else if(sym == forsym) {  //for语句
                                getsym();
                                if(sym != ident)
                                    error(13);
                                else {
                                    i = position(id, *ptx); //id为标识符， ptx为表尾指针
                                    if(i == 0)  //标识符不存在
                                        error(11);

                                    else if(table[i].kind != variable) {  //该符号不为int型变量
                                        error(12);
                                        i = 0; //　
                                    }

                                    getsym();
                                    if(sym != becomes)
                                        error(13);
                                    else {
                                        getsym();
                                        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                        nxtlev[untilsym] = true;   /* 后继符号为do */
                                        nxtlev[stepsym] = true;
                                        nxtlev[dosym] = true;
                                        expression(nxtlev, ptx, lev);

                                        if(i != 0) {
                                            gen(int_sto, lev - table[i].level, table[i].adr);  //初始赋值
                                            cx1 = cx;
                                            gen(jmp, 0, 0); // cx1，第一次循环直接调到until
                                            cx2 = cx;  //　执行完do跳回此处
                                            if(sym != stepsym)   //step
                                                error(19);
                                            else {
                                                getsym();
                                                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                                nxtlev[untilsym] = true;
                                                nxtlev[stepsym] = true;
                                                nxtlev[dosym] = true;
                                                expression(nxtlev, ptx, lev);
                                                gen(lod, lev - table[i].level, table[i].adr);
                                                gen(opr, 0, 2);
                                                gen(int_sto, lev - table[i].level, table[i].adr);   //step增加值

                                                if(sym != untilsym)  // until
                                                    error(19);
                                                else {
                                                    code[cx1].a = cx;
                                                    getsym();

                                                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                                    nxtlev[untilsym] = true;
                                                    nxtlev[stepsym] = true;
                                                    nxtlev[dosym] = true;
                                                    expression(nxtlev, ptx, lev);

                                                    gen(lod, lev - table[i].level, table[i].adr);
                                                    gen(opr, 0, 11); // 变量大于等于until　expression
                                                    int cx3 = cx;  // until跳转代码地址
                                                    gen(jpc, 0, 0);

                                                    if(sym == dosym) {
                                                        getsym();
                                                        statement(fsys, ptx, lev);
                                                        gen(jmp, 0, cx2);   //　跳回step　
                                                        code[cx3].a = cx;   //回填地址,如果循环变量大于until表达式的值，跳出循环
                                                    }
                                                    else
                                                        error(19);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if(sym == selfplus || sym == selfminus) {   //前置自增自减
//                                facbegsys[selfplus] = true;
                                symbol tmp_sym = sym;  // 记录是自增或者自减
                                getsym();
                                if(sym == ident) {
                                    i = position(id, *ptx);
                                    if(i == 0) {
                                        error(11);
                                    }
                                    else {
                                        if(table[i].kind != variable) {
                                            error(12);
                                            i = 0;
                                        }
                                        else {
                                            if(tmp_sym == selfplus) {
                                                gen(lod, lev - table[i].level, table[i].adr);
                                                gen(lit, 0, 1);
                                                gen(opr, 0, 2);
                                                gen(int_sto, lev - table[i].level, table[i].adr);
                                                getsym();
                                            }
                                            else {
                                                gen(lod, lev - table[i].level, table[i].adr);
                                                gen(lit, 0, 1);
                                                gen(opr, 0, 3);
                                                gen(int_sto, lev - table[i].level, table[i].adr);
                                                getsym();
                                            }
                                        }
                                    }
                                }
                            }
                            else if(sym == returnsym) {
                                getsym();
                                memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                nxtlev[semicolon] = true;
                                expression(nxtlev, ptx, lev);
                                gen(real_sto, 0, 0);
                                gen(opr, 1, 0);
                            }
                        }
                    }
                }
            }
        }
    }
    memset(nxtlev, 0, sizeof(bool) * symnum);   /* 语句结束无补救集合 */
    test(fsys, nxtlev, 19); /* 检测语句结束的正确性 */
}

/*
 * 表达式处理
 */
void expression(bool* fsys, int* ptx, int lev)
{
    enum symbol addop;  /* 用于保存正负号 */
    bool nxtlev[symnum];

    if(sym == plus || sym == minus) /* 表达式开头有正负号，此时当前表达式被看作一个正的或负的项 */
    {
        addop = sym;    /* 保存开头的正负号 */
        getsym();
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[plus] = true;
        nxtlev[minus] = true;
        term(nxtlev, ptx, lev); /* 处理项 */
        if (addop == minus)
        {
            gen(opr,0,1);   /* 如果开头为负号生成取负指令 */
        }
    }
    else    /* 此时表达式被看作项的加减 */
    {
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[plus] = true;
        nxtlev[minus] = true;
        term(nxtlev, ptx, lev); /* 处理项 */
    }

    while (sym == plus || sym == minus)
    {
        addop = sym;
        getsym();
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[plus] = true;
        nxtlev[minus] = true;
        term(nxtlev, ptx, lev); /* 处理项 */
        if (addop == plus)
        {
            gen(opr, 0, 2); /* 生成加法指令 */
        }
        else
        {
            gen(opr, 0, 3); /* 生成减法指令 */
        }
    }
}

/*
 * 项处理
 */
void term(bool* fsys, int* ptx, int lev)
{
    enum symbol mulop;  /* 用于保存乘除法符号 */
    bool nxtlev[symnum];

    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
    nxtlev[times] = true;
    nxtlev[slash] = true;
    nxtlev[rsquare] = true;
    factor(nxtlev, ptx, lev);   /* 处理因子 */
    while(sym == times || sym == slash)
    {
        mulop = sym;
        getsym();
        factor(nxtlev, ptx, lev);
        if(mulop == times)
        {
            gen(opr, 0, 4); /* 生成乘法指令 */
        }
        else
        {
            gen(opr, 0, 5); /* 生成除法指令 */
        }
    }
}

/*
 * 因子处理
 */
void factor(bool* fsys, int* ptx, int lev)
{
    int i;
    bool nxtlev[symnum];
    test(facbegsys, fsys, 24);  /* 检测因子的开始符号 */
    while(inset(sym, facbegsys))    /* 循环处理因子 */
    {
        if(sym == ident)    /* 因子为常量或变量 */
        {
            i = position(id, *ptx); /* 查找标识符在符号表中的位置 */
            if (i == 0)
            {
                error(11);  /* 标识符未声明 */
            }
            else
            {
                switch (table[i].kind)
                {
                    case constant:  /* 标识符为常量 */
                        gen(lit, 0, table[i].val);  /* 直接把常量的值入栈 */
                        var_type = constant;
                        break;

                    case variable:  /* 标识符为变量 */
                        gen(lod, lev-table[i].level, table[i].adr); /* 找到变量地址并将其值入栈 */
                        var_type = variable;
                        getsym();

                        if(sym == selfplus) {
                            gen(lit, 0, 1);
                            gen(opr, 0, 2);
                            gen(int_sto, lev-table[i].level, table[i].adr);
                            gen(lod, lev-table[i].level, table[i].adr);
                            gen(lit, 0, 1);
                            gen(opr, 0, 3);
                            getsym();

                        }
                        else if(sym == selfminus) {
                            gen(lit, 0, 1);
                            gen(opr, 0, 3);
                            gen(int_sto, lev-table[i].level, table[i].adr);
                            gen(lod, lev-table[i].level, table[i].adr);
                            gen(lit, 0, 1);
                            gen(opr, 0, 2);
                            getsym();
                        }
                        break;

                    case procedure: /* 标识符为过程 */
                        error(21);  /* 不能为过程 */
                        break;

                    case charvar:
                        gen(lod, lev-table[i].level, table[i].adr); /* 找到变量地址并将其值入栈 */
                        var_type = charvar;
                        getsym();
                        break;

                    case realvar:
                        gen(lod, lev-table[i].level, table[i].adr);
                        var_type = realvar;
                        getsym();
                        break;

                    case variable_array:  //数组
                        getsym();

                        if(sym == lsquare) {
                            getsym();
                            bool nxtlev_array[symnum];
                            memcpy(nxtlev_array, fsys, sizeof(bool) * symnum);
                            nxtlev_array[']'] = true;
                            expression(nxtlev_array, ptx, lev);

                            if(sym == rsquare) {
                                var_type = variable;
                                gen(array_lod, lev-table[i].level, table[i].adr);
                                getsym();
                            }
                            else
                                error(38);
                        }
                        else {
                            error(38);  //　数组访问不正确
                        }

                        break;
                }
            }

        }
        else if(sym == selfplus || sym == selfminus) {   //前置++, --
            symbol tmp_sym = sym;
            getsym();

            if(sym == ident) {
                i = position(id, *ptx);

                if (i == 0)
                {
                    error(11);  /* 标识符未声明 */
                }
                else {
                    printf("%s\n", table[i].name);
                    if(table[i].kind != variable) {
                        error(33);  //++后面应为变量
                    }
                    else {
                        if(tmp_sym == selfplus) {
                            gen(lod, lev-table[i].level, table[i].adr);
                            gen(lit, 0, 1);
                            gen(opr, 0, 2);
                            gen(int_sto, lev-table[i].level, table[i].adr);
                            gen(lod, lev-table[i].level, table[i].adr);
                            getsym();
                        }
                        else if(tmp_sym == selfminus) {
                            gen(lod, lev-table[i].level, table[i].adr);
                            gen(lit, 0, 1);
                            gen(opr, 0, 3);
                            gen(int_sto, lev-table[i].level, table[i].adr);
                            gen(lod, lev-table[i].level, table[i].adr);
                            getsym();
                        }
                    }
                }
            }
            else
                error(32);   //++后面应为标识符
        }
        else
        {
            if(sym == number)   /* 因子为数 */
            {
                if (num > amax)
                {
                    error(31); /* 数越界 */
                    num = 0;
                }
                gen(lit, 0, num);
                getsym();
            }
            else if(sym == charval) { /* 因子为char类型 */
                gen(lit, 0, ch_value);
                getsym();
            }
            else
            {
                if (sym == lparen)  /* 因子为表达式 */
                {
                    getsym();
                    memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                    nxtlev[rparen] = true;
                    expression(nxtlev, ptx, lev);
                    if (sym == rparen)
                    {
                        getsym();
                    }
                    else
                    {
                        error(22);  /* 缺少右括号 */
                    }
                }
                else {    /* 当因子为函数调用　*/
                    if (sym == callsym) /* 准备按照call语句处理 */
                    {
                        getsym();
                        if (sym != ident) {
                            error(14);  /* call后应为标识符 */
                        } else {
                            i = position(id, *ptx);
                            if (i == 0) {
                                error(11);  /* 过程名未找到 */
                            } else {
                                if (table[i].kind == procedure) {
                                    getsym();
                                    int params_nums = 0;
                                    if (sym == lparen) {
                                        getsym();
                                        while (sym != rparen) {
                                            memcpy(nxtlev, fsys, sizeof(bool) * symnum);
                                            nxtlev[rparen] = true;
                                            nxtlev[comma] = true;
                                            expression(nxtlev, ptx, lev);
                                            params_nums++;
                                            if (sym == comma) {
                                                getsym();
                                                if(sym == rparen)
                                                    error(46);  /* 调用过程时存在多余的逗号 */
                                                continue;
                                            }
                                        }
                                        if(table[i].params_num != params_nums)
                                            error(45); /*　参数个数不正确 */
                                    } else {
                                        error(44); /*　调用过程时括号缺失 */
                                    }
                                    gen(cal, lev - table[i].level, table[i].adr); /* 生成call指令 */
                                } else {
                                    error(15);  /* call后标识符类型应为过程 */
                                }
                            }
                            getsym();
                        }
                    }
                }
            }
        }
        memset(nxtlev, 0, sizeof(bool) * symnum);
        nxtlev[lparen] = true;
        test(fsys, nxtlev, 23); /* 一个因子处理完毕，遇到的单词应在fsys集合中 */
        /* 如果不是，报错并找到下一个因子的开始，使语法分析可以继续运行下去 */
    }
}

/*
 * 条件处理
 */
void condition(bool* fsys, int* ptx, int lev)
{
    enum symbol relop;
    bool nxtlev[symnum];

    if(sym == oddsym)   /* 准备按照odd运算处理 */
    {
        getsym();
        expression(fsys, ptx, lev);
        gen(opr, 0, 6); /* 生成odd指令 */
    }
    else
    {
        /* 逻辑表达式处理 */
        memcpy(nxtlev, fsys, sizeof(bool) * symnum);
        nxtlev[eql] = true;
        nxtlev[neq] = true;
        nxtlev[lss] = true;
        nxtlev[leq] = true;
        nxtlev[gtr] = true;
        nxtlev[geq] = true;
        expression(nxtlev, ptx, lev);
        if (sym != eql && sym != neq && sym != lss && sym != leq && sym != gtr && sym != geq)
        {
            error(20); /* 应该为关系运算符 */
        }
        else
        {
            relop = sym;
            getsym();
            expression(fsys, ptx, lev);
            switch (relop)
            {
                case eql:
                    gen(opr, 0, 8);
                    break;
                case neq:
                    gen(opr, 0, 9);
                    break;
                case lss:
                    gen(opr, 0, 10);
                    break;
                case geq:
                    gen(opr, 0, 11);
                    break;
                case gtr:
                    gen(opr, 0, 12);
                    break;
                case leq:
                    gen(opr, 0, 13);
                    break;
            }
        }
    }
}

/*
 * 解释程序
 */
void interpret()
{
    int p = 0; /* 指令指针 */
    int b = 1; /* 指令基址 */
    int t = 0; /* 栈顶指针 */
    struct instruction i;   /* 存放当前指令 */
    float s[stacksize];   /* 栈 */
    memset(s, 0, sizeof(s));

    printf("Start pl0\n");
    fprintf(fresult,"Start pl0\n");
    s[0] = 0; /* s[0]不用 */
    s[1] = 0; /* 主程序的三个联系单元均置为0 */
    s[2] = 0;
    s[3] = 0;

    do {
        i = code[p];    /* 读当前指令 */
        p = p + 1;
        switch (i.f)
        {
            case lit:   /* 将常量a的值取到栈顶 */
                t = t + 1;
                s[t] = i.a;
                break;

            case opr:   /* 数学、逻辑运算 */
                switch (static_cast<int>(i.a))
                {
                    case 0:  /* 函数调用结束后返回 */
                        if(static_cast<int>(i.l) == 0) {   // 无返回值
                            t = b - 1;
                            p = s[t + 3];
                            b = s[t + 2];
                        }
                        else {      // 有返回值
                            t = b;
                            p = s[t + 2];
                            b = s[t + 1];

                        }
                        break;

                    case 1: /* 栈顶元素取反 */
                        s[t] = - s[t];
                        break;

                    case 2: /* 次栈顶项加上栈顶项，退两个栈元素，相加值进栈 */
                        t = t - 1;
                        s[t] = s[t] + s[t + 1];
                        break;

                    case 3:/* 次栈顶项减去栈顶项 */
                        t = t - 1;
                        s[t] = s[t] - s[t + 1];
                        break;

                    case 4:/* 次栈顶项乘以栈顶项 */
                        t = t - 1;
                        s[t] = s[t] * s[t + 1];
                        break;

                    case 5:/* 次栈顶项除以栈顶项 */
                        t = t - 1;
                        s[t] = s[t] / s[t + 1];
                        break;

                    case 6:/* 栈顶元素的奇偶判断 */
                        s[t] = static_cast<int>(s[t]) % 2;
                        break;

                    case 8:/* 次栈顶项与栈顶项是否相等 */
                        t = t - 1;
                        s[t] = (s[t] == s[t + 1]);
                        break;

                    case 9:/* 次栈顶项与栈顶项是否不等 */
                        t = t - 1;
                        s[t] = (s[t] != s[t + 1]);
                        break;

                    case 10:/* 次栈顶项是否小于栈顶项 */
                        t = t - 1;
                        s[t] = (s[t] < s[t + 1]);
                        break;

                    case 11:/* 次栈顶项是否大于等于栈顶项 */
                        t = t - 1;
                        s[t] = (s[t] >= s[t + 1]);
                        break;

                    case 12:/* 次栈顶项是否大于栈顶项 */
                        t = t - 1;
                        s[t] = (s[t] > s[t + 1]);
                        break;

                    case 13: /* 次栈顶项是否小于等于栈顶项 */
                        t = t - 1;
                        s[t] = (s[t] <= s[t + 1]);
                        break;

                    case 14:/* 栈顶值输出 */
                        switch (i.l) {
                            case variable:
                                printf("%d", static_cast<int>(s[t]));
                                fprintf(fresult, "%d", static_cast<int>(s[t]));
                                break;

                            case charvar:
                                printf("%c", static_cast<char>(s[t]));
                                fprintf(fresult, "%c", static_cast<char>(s[t]));
                                break;

                            case realvar:
                                printf("%f", s[t]);
                                fprintf(fresult, "%f", s[t]);
                                break;
                        }

                        t = t - 1;
                        break;

                    case 15:/* 输出换行符 */
                        printf("\n");
                        fprintf(fresult,"\n");
                        break;

                    case 16:/* 读入一个输入置于栈顶 */
                        t = t + 1;
                        printf("?");
                        fprintf(fresult, "?");
                        scanf("%f", &(s[t]));
                        fprintf(fresult, "%f\n", s[t]);
                        break;
                }
                break;

            case lod:   /* 取相对当前过程的数据基地址为a的内存的值到栈顶 */
                t = t + 1;
                s[t] = s[base(i.l,s,b) + static_cast<int>(i.a)];
                break;

            case real_sto:   /* 栈顶的值存到相对当前过程的数据基地址为a的内存 */
                s[base(i.l, s, b) + static_cast<int>(i.a)] = s[t];
                t = t - 1;
                break;

            case cal:   /* 调用子过程 */
                s[t + 1] = base(i.l, s, b); /* 将父过程基地址入栈，即建立静态链 */
                s[t + 2] = b;   /* 将本过程基地址入栈，即建立动态链 */
                s[t + 3] = p;   /* 将当前指令指针入栈，即保存返回地址 */
                b = t + 1;  /* 改变基地址指针值为新过程的基地址 */
                p = i.a;    /* 跳转 */
                break;

            case ini:   /* 在数据栈中为被调用的过程开辟a个单元的数据区 */
                t = t + i.a;
                break;

            case jmp:   /* 直接跳转 */
                p = i.a;
                break;

            case jpc:   /* 条件跳转 */
                if (s[t] == 0)
                    p = i.a;
                t = t - 1;
                break;

            case int_sto:   /* 栈顶的值存到相对当前过程的数据基地址为a的内存,
                            　* 较real_sto多了强制转换成int的过程 */
                s[base(i.l, s, b) + static_cast<int>(i.a)] = static_cast<int>(s[t]);
                t = t - 1;
                break;

            case array_lod:  /* 将数组元素入栈，索引为t-1中的变量　*/
                s[t] = s[base(i.l,s,b) + static_cast<int>(i.a) + static_cast<int>(s[t])];
                break;

            case array_sto:     /* a[b] = c, 栈顶的值c存到相对当前过程的数据基地址为a+b的内存,其中b为次栈顶中的值　*/
                s[base(i.l, s, b) + static_cast<int>(i.a) + static_cast<int>(s[t-1])] = static_cast<int>(s[t]);
                t = t - 2;
                break;
        }
    } while (p != 0);
    printf("End pl0\n");
    fprintf(fresult,"End pl0\n");
}

/* 通过过程基址求上l层过程的基址 */
int base(int l, float* s, int b)
{
    int b1;
    b1 = b;
    while (l > 0)
    {
        b1 = static_cast<int>(s[b1]);
        l--;
    }
    return b1;
}