#include<stdio.h>
#include<string.h>
#include<stdlib.h>
 
#define SIXTEEN 16
#define EIGHT 8
#define X1 129
#define X2 257
 
typedef struct systemRegister{
    int proCount;
    int instruRegister;
    int signRegister;
}   SystemReg;
typedef SystemReg * SystemReg_PTR;
 
typedef struct genrealRegister{
    int dataReg[4];
    int ptrReg[4];
}   GPR;
typedef GPR * GPR_PTR;
 
/*函数功能：获取操作码，并转换成十进制；*/
int get_operationCode(char *ch,int i,int operCode);
/*函数功能：获取立即数，并转换成成十进制；*/
int get_immediateValue(char *ch,int i,int immeValue);
/*函数功能：获取每一条指令对应的代码段；*/
int get_codeMemory(char *ch,int i,int codeMemory);
/*函数功能：获取ir的值；*/
int get_ir(char *ch,int i,int ir);
/*函数功能：分析指令，并调用相应函数完成后续操作*/
void analyzeInstruction(int operCode,int immeValue,GPR_PTR gprPtr,
    SystemReg_PTR sysReg_ptr,int codeMemory[X1],int dataMemory[X2],char *ch);
/*函数功能：结束程序并打印代码段和数据段；*/
void exitPrint(int codeMemory[X1],int dataMemory[X2]);
/*函数功能：打印各个寄存器的状态；*/
void print(SystemReg_PTR sysReg_ptr,GPR_PTR gprPtr);
/*函数功能：数据传送*/
void dataPass(GPR_PTR gprPtr,int immeValue,int dataMemory[X2],int data,int ptr);
/*函数功能：实现算数和逻辑运算*/
void OPERATION(int operCode,GPR_PTR gprPtr,int immeValue,int dataMemory[X2],int data,int ptr);
/*函数功能：比较指令：（1）数据寄存器与立即数比较（2）数据寄存器与地址指向的
    数据比较；进而修改标志寄存器的值；*/
void COMP(GPR_PTR gprPtr,int immeValue,int dataMemory[X2],SystemReg_PTR sysReg_ptr
            ,int data,int ptr);
/*函数功能：实现输入输出指令：从输入端口输入一个整数存到数据寄存器；*/
void input_or_output(int operCode,GPR_PTR gprPtr,int data);
/*函数功能：实现跳转指令：（1）无条件跳转指令（2）根据标志寄存器内容，判断是否跳转，并执行；*/
void jumpInstruction(int immeValue,char *ch,SystemReg_PTR sysReg_ptr); 
 
int main(void)
{
    char filename[]={"dict.dic"};//文件名命名； 
    FILE *fPtr;
    SystemReg sysReg={0,0,0};//定义系统寄存器，并初始化为0； 
    GPR gpr={{0,0,0,0},{0,0,0,0}};//数据寄存器和地址寄存器初值为0； 
    char ch[33],inMemory[16384/4][33];//定义内存，并申请空间； 
    int operCode=0,immeValue=0,count=0,n=0;
    int codeMemory[X1]={0},dataMemory[X2]={0}; //代码段和数据段初值为0； 
    if((fPtr=fopen(filename,"r"))!=NULL){
        fgets(ch,sizeof(ch),fPtr);//读取一条指令； 
        while((n=fgetc(fPtr))!=-1){
            codeMemory[count]=get_codeMemory(ch,0,0);//存储代码段；
            strcpy(inMemory[count],ch);count++;//将指令存入内存；
            fgets(ch,sizeof(ch),fPtr);//读取下一条指令；  
        }fclose(fPtr);//关闭文件； 
    }else printf("The file can't be opened.\n");
    while(count){
        strcpy(ch,inMemory[sysReg.proCount/4]);//读取程序计数器指向的指令；  
        sysReg.proCount+=4;//系统寄存器内的程序计数器指向下一条指令； 
        sysReg.instruRegister=get_ir(ch,0,0);//指令寄存器赋值； 
        operCode=get_operationCode(ch,0,0);//读取操作码； 
        immeValue=get_immediateValue(ch,16,0);//读取立即数； 
        analyzeInstruction(operCode,immeValue,&gpr,&sysReg,
                codeMemory,dataMemory,ch);//分析指令，并调用后续函数操作； 
        print(&sysReg,&gpr);//打印寄存器状态； 
    } 
}
 
int get_ir(char *ch,int i,int ir)
{
    for(;i<15;i++)
        ir=2*(ir+ch[i]-'0');
    ir=ir+ch[i]-'0';
    return ir;
}
 
int get_codeMemory(char *ch,int i,int codeMemory)
{
    for(;i<31;i++)
        codeMemory=2*(codeMemory+(ch[i]-'0'));
    codeMemory=codeMemory+ch[i]-'0';
    return codeMemory;
}
 
int get_operationCode(char *ch,int i,int operCode)
{
    for(;i<7;i++)
        operCode=2*(operCode+(ch[i]-'0'));
    operCode=operCode+ch[i]-'0';
    return operCode;
}
 
int get_immediateValue(char *ch,int i,int immeValue)
{
    for(;i<31;i++)
        immeValue=2*(immeValue+ch[i]-'0');
    immeValue=immeValue+ch[i]-'0';
    return (short) immeValue;
}
 
void dataPass(GPR_PTR gprPtr,int immeValue,int dataMemory[X2],int data,int ptr)
{
    if(ptr==0){
        if(data<=4) 
            gprPtr->dataReg[data-1]=immeValue;//将立即数存入数据寄存器； 
        else if(data>=5) 
            gprPtr->ptrReg[data-5]=immeValue;//将立即数存入地址寄存器； 
    }else if(data<ptr)//将地址寄存器所指内存存入数据寄存器； 
        gprPtr->dataReg[data-1]=dataMemory[((gprPtr->ptrReg[ptr-5])-16384)/2];
    else if(data>ptr)//将数据寄存器值存入地址寄存器所指内存； 
        dataMemory[(gprPtr->ptrReg[data-5]-16384)/2]=gprPtr->dataReg[ptr-1];
}
 
void OPERATION(int operCode,GPR_PTR gprPtr,int immeValue,int dataMemory[X2],int data,int ptr)
{
    if(operCode==2&&ptr==0)//数据寄存器+立即数存入原数据寄存器； 
        gprPtr->dataReg[data-1]+=immeValue;
    else if(operCode==2&&data<ptr)//数据寄存器+地址寄存器所指内存，存入原数据寄存器； 
        gprPtr->dataReg[data-1]+=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==3&&ptr==0)//数据寄存器-立即数存入原数据寄存器； 
        gprPtr->dataReg[data-1]-=immeValue;
    else if(operCode==3&&data<ptr)//数据寄存器-地址寄存器所指内存，存入原数据寄存器； 
        gprPtr->dataReg[data-1]-=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==4&&ptr==0)//数据寄存器*立即数存入原数据寄存器； 
        gprPtr->dataReg[data-1]*=immeValue;
    else if(operCode==4&&data<ptr)//数据寄存器*地址寄存器所指内存，存入原数据寄存器； 
        gprPtr->dataReg[data-1]*=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==5&&ptr==0)//数据寄存器/立即数存入原数据寄存器； 
        gprPtr->dataReg[data-1]/=immeValue;
    else if(operCode==5&&data<ptr)//数据寄存器/地址寄存器所指内存，存入原数据寄存器； 
        gprPtr->dataReg[data-1]/=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==6&&ptr==0)//数据寄存器、立即数与运算存入原数据寄存器； 
        gprPtr->dataReg[data-1]=gprPtr->dataReg[data-1]&&immeValue;
    else if(operCode==6&&data<ptr)//数据寄存器、地址寄存器所指内存与运算，存入原数据寄存器； 
        (gprPtr->dataReg[data-1])=(gprPtr->dataReg[data-1])&&(dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]);
    if(operCode==7&&ptr==0)//数据寄存器、立即数或运算存入原数据寄存器；
        gprPtr->dataReg[data-1]=gprPtr->dataReg[data-1]||immeValue;
    else if(operCode==7&&data<ptr)//数据寄存器、地址寄存器所指内存或运算，存入原数据寄存器；
        gprPtr->dataReg[data-1]=(gprPtr->dataReg[data-1])||(dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]);
    if(operCode==8&&data!=0)//数据寄存器非运算，存入原数据寄存器； 
        gprPtr->dataReg[data-1]=!(gprPtr->dataReg[data-1]);
    else if(operCode==8&&data==0)//指令寄存器所指内存非运算，存入该内存； 
        dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]=!dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
}
 
void COMP(GPR_PTR gprPtr,int immeValue,int dataMemory[X2],SystemReg_PTR sysReg_ptr
            ,int data,int ptr)
{
    if(ptr==0){//数据寄存器与立即数比较； 
        if(gprPtr->dataReg[data-1]==immeValue)
            sysReg_ptr->signRegister=0;
        else if(gprPtr->dataReg[data-1]>immeValue)
            sysReg_ptr->signRegister=1;
        else if(gprPtr->dataReg[data-1]<immeValue)
            sysReg_ptr->signRegister=-1;
    }else{//地址寄存器所指内存与数据寄存器的比较； 
        if(dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]==gprPtr->dataReg[data-1])
            sysReg_ptr->signRegister=0;
        else if(dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]<gprPtr->dataReg[data-1])
            sysReg_ptr->signRegister=1;
        else if(dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]>gprPtr->dataReg[data-1])
            sysReg_ptr->signRegister=-1;
    }
}
 
void jumpInstruction(int immeValue,char *ch,SystemReg_PTR sysReg_ptr)
{
    int sign=0;
    int i;
    for(i=EIGHT;i<SIXTEEN-1;i++)
        sign=2*(sign+ch[i]-'0');
    sign=sign+ch[i]-'0';
    if(sign==0||(sign==1&&sysReg_ptr->signRegister==0)||(sign==2&&sysReg_ptr->signRegister==1)|| 
    (sign==3&&sysReg_ptr->signRegister==-1))//无条件跳转;标志寄存器为0时跳转;标志寄存器为1时跳转;标志寄存器为-1时跳转；  
        sysReg_ptr->proCount+=(immeValue-4);
}
 
void input_or_output(int operCode,GPR_PTR gprPtr,int data)
{
    if(operCode==12)
        printf("out: %d\n",gprPtr->dataReg[data-1]);//输出数据寄存器内的值；
    else{printf("in:\n");
    scanf("%d",&gprPtr->dataReg[data-1]);//输入的值存入数据寄存器； 
    }
}
 
void analyzeInstruction(int operCode,int immeValue,GPR_PTR gprPtr,
    SystemReg_PTR sysReg_ptr,int codeMemory[X1],int dataMemory[X2],char *ch)
{
    int data=0,ptr=0,i=EIGHT,j=12;
    for(i=EIGHT;i<11;i++)
        data=2*(data+ch[i]-'0');
    data=data+ch[i]-'0';
    for(j=12;j<SIXTEEN-1;j++)
        ptr=2*(ptr+ch[j]-'0');
    ptr=ptr+ch[j]-'0';
    if(operCode==0){//停机指令； 
        print(sysReg_ptr,gprPtr);
        exitPrint(codeMemory,dataMemory);
    }
    if(operCode==1)//数据传送指令； 
        dataPass(gprPtr,immeValue,dataMemory,data,ptr);
    if(operCode>=2&&operCode<=8)//算数和逻辑运算； 
        OPERATION(operCode,gprPtr,immeValue,dataMemory,data,ptr);
    if(operCode==9)//比较指令； 
        COMP(gprPtr,immeValue,dataMemory,sysReg_ptr,data,ptr);
    if(operCode==10)//跳转指令； 
        jumpInstruction(immeValue,ch,sysReg_ptr); 
    if(operCode==11||operCode==12)//输入输出指令； 
        input_or_output(operCode,gprPtr,data);
}
 
void exitPrint(int codeMemory[X1],int dataMemory[X2])
{
    int i;
    printf("\ncodeSegment :\n");
    for(i=1;i<X1;i++){ //输出代码段； 
        if(i%EIGHT!=0) printf("%d ",codeMemory[i-1]);
        else printf("%d\n",codeMemory[i-1]);
    }printf("\ndataSegment :\n");
    for(i=1;i<X2;i++){ //输出数据段； 
        if(i%SIXTEEN!=0) printf("%d ",dataMemory[i-1]);
        else printf("%d\n",dataMemory[i-1]);
    }exit(0);
}
 
void print(SystemReg_PTR sysReg_ptr,GPR_PTR gprPtr)
{//输出11个寄存器的状态； 
    printf("ip = %d\n",sysReg_ptr->proCount);
    printf("flag = %d\n",sysReg_ptr->signRegister);
    printf("ir = %d\n",sysReg_ptr->instruRegister);
    printf("ax1 = %d ax2 = %d ax3 = %d ax4 = %d\n",gprPtr->dataReg[0],gprPtr->dataReg[1],gprPtr->dataReg[2],gprPtr->dataReg[3]);
    printf("ax5 = %d ax6 = %d ax7 = %d ax8 = %d\n",gprPtr->ptrReg[0],gprPtr->ptrReg[1],gprPtr->ptrReg[2],gprPtr->ptrReg[3]);
}
