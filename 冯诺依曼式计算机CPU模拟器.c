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
 
/*�������ܣ���ȡ�����룬��ת����ʮ���ƣ�*/
int get_operationCode(char *ch,int i,int operCode);
/*�������ܣ���ȡ����������ת���ɳ�ʮ���ƣ�*/
int get_immediateValue(char *ch,int i,int immeValue);
/*�������ܣ���ȡÿһ��ָ���Ӧ�Ĵ���Σ�*/
int get_codeMemory(char *ch,int i,int codeMemory);
/*�������ܣ���ȡir��ֵ��*/
int get_ir(char *ch,int i,int ir);
/*�������ܣ�����ָ���������Ӧ������ɺ�������*/
void analyzeInstruction(int operCode,int immeValue,GPR_PTR gprPtr,
    SystemReg_PTR sysReg_ptr,int codeMemory[X1],int dataMemory[X2],char *ch);
/*�������ܣ��������򲢴�ӡ����κ����ݶΣ�*/
void exitPrint(int codeMemory[X1],int dataMemory[X2]);
/*�������ܣ���ӡ�����Ĵ�����״̬��*/
void print(SystemReg_PTR sysReg_ptr,GPR_PTR gprPtr);
/*�������ܣ����ݴ���*/
void dataPass(GPR_PTR gprPtr,int immeValue,int dataMemory[X2],int data,int ptr);
/*�������ܣ�ʵ���������߼�����*/
void OPERATION(int operCode,GPR_PTR gprPtr,int immeValue,int dataMemory[X2],int data,int ptr);
/*�������ܣ��Ƚ�ָ���1�����ݼĴ������������Ƚϣ�2�����ݼĴ������ַָ���
    ���ݱȽϣ������޸ı�־�Ĵ�����ֵ��*/
void COMP(GPR_PTR gprPtr,int immeValue,int dataMemory[X2],SystemReg_PTR sysReg_ptr
            ,int data,int ptr);
/*�������ܣ�ʵ���������ָ�������˿�����һ�������浽���ݼĴ�����*/
void input_or_output(int operCode,GPR_PTR gprPtr,int data);
/*�������ܣ�ʵ����תָ���1����������תָ�2�����ݱ�־�Ĵ������ݣ��ж��Ƿ���ת����ִ�У�*/
void jumpInstruction(int immeValue,char *ch,SystemReg_PTR sysReg_ptr); 
 
int main(void)
{
    char filename[]={"dict.dic"};//�ļ��������� 
    FILE *fPtr;
    SystemReg sysReg={0,0,0};//����ϵͳ�Ĵ���������ʼ��Ϊ0�� 
    GPR gpr={{0,0,0,0},{0,0,0,0}};//���ݼĴ����͵�ַ�Ĵ�����ֵΪ0�� 
    char ch[33],inMemory[16384/4][33];//�����ڴ棬������ռ䣻 
    int operCode=0,immeValue=0,count=0,n=0;
    int codeMemory[X1]={0},dataMemory[X2]={0}; //����κ����ݶγ�ֵΪ0�� 
    if((fPtr=fopen(filename,"r"))!=NULL){
        fgets(ch,sizeof(ch),fPtr);//��ȡһ��ָ� 
        while((n=fgetc(fPtr))!=-1){
            codeMemory[count]=get_codeMemory(ch,0,0);//�洢����Σ�
            strcpy(inMemory[count],ch);count++;//��ָ������ڴ棻
            fgets(ch,sizeof(ch),fPtr);//��ȡ��һ��ָ�  
        }fclose(fPtr);//�ر��ļ��� 
    }else printf("The file can't be opened.\n");
    while(count){
        strcpy(ch,inMemory[sysReg.proCount/4]);//��ȡ���������ָ���ָ�  
        sysReg.proCount+=4;//ϵͳ�Ĵ����ڵĳ��������ָ����һ��ָ� 
        sysReg.instruRegister=get_ir(ch,0,0);//ָ��Ĵ�����ֵ�� 
        operCode=get_operationCode(ch,0,0);//��ȡ�����룻 
        immeValue=get_immediateValue(ch,16,0);//��ȡ�������� 
        analyzeInstruction(operCode,immeValue,&gpr,&sysReg,
                codeMemory,dataMemory,ch);//����ָ������ú������������� 
        print(&sysReg,&gpr);//��ӡ�Ĵ���״̬�� 
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
            gprPtr->dataReg[data-1]=immeValue;//���������������ݼĴ����� 
        else if(data>=5) 
            gprPtr->ptrReg[data-5]=immeValue;//�������������ַ�Ĵ����� 
    }else if(data<ptr)//����ַ�Ĵ�����ָ�ڴ�������ݼĴ����� 
        gprPtr->dataReg[data-1]=dataMemory[((gprPtr->ptrReg[ptr-5])-16384)/2];
    else if(data>ptr)//�����ݼĴ���ֵ�����ַ�Ĵ�����ָ�ڴ棻 
        dataMemory[(gprPtr->ptrReg[data-5]-16384)/2]=gprPtr->dataReg[ptr-1];
}
 
void OPERATION(int operCode,GPR_PTR gprPtr,int immeValue,int dataMemory[X2],int data,int ptr)
{
    if(operCode==2&&ptr==0)//���ݼĴ���+����������ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]+=immeValue;
    else if(operCode==2&&data<ptr)//���ݼĴ���+��ַ�Ĵ�����ָ�ڴ棬����ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]+=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==3&&ptr==0)//���ݼĴ���-����������ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]-=immeValue;
    else if(operCode==3&&data<ptr)//���ݼĴ���-��ַ�Ĵ�����ָ�ڴ棬����ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]-=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==4&&ptr==0)//���ݼĴ���*����������ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]*=immeValue;
    else if(operCode==4&&data<ptr)//���ݼĴ���*��ַ�Ĵ�����ָ�ڴ棬����ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]*=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==5&&ptr==0)//���ݼĴ���/����������ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]/=immeValue;
    else if(operCode==5&&data<ptr)//���ݼĴ���/��ַ�Ĵ�����ָ�ڴ棬����ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]/=dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
    if(operCode==6&&ptr==0)//���ݼĴ��������������������ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]=gprPtr->dataReg[data-1]&&immeValue;
    else if(operCode==6&&data<ptr)//���ݼĴ�������ַ�Ĵ�����ָ�ڴ������㣬����ԭ���ݼĴ����� 
        (gprPtr->dataReg[data-1])=(gprPtr->dataReg[data-1])&&(dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]);
    if(operCode==7&&ptr==0)//���ݼĴ��������������������ԭ���ݼĴ�����
        gprPtr->dataReg[data-1]=gprPtr->dataReg[data-1]||immeValue;
    else if(operCode==7&&data<ptr)//���ݼĴ�������ַ�Ĵ�����ָ�ڴ�����㣬����ԭ���ݼĴ�����
        gprPtr->dataReg[data-1]=(gprPtr->dataReg[data-1])||(dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]);
    if(operCode==8&&data!=0)//���ݼĴ��������㣬����ԭ���ݼĴ����� 
        gprPtr->dataReg[data-1]=!(gprPtr->dataReg[data-1]);
    else if(operCode==8&&data==0)//ָ��Ĵ�����ָ�ڴ�����㣬������ڴ棻 
        dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2]=!dataMemory[(gprPtr->ptrReg[ptr-5]-16384)/2];
}
 
void COMP(GPR_PTR gprPtr,int immeValue,int dataMemory[X2],SystemReg_PTR sysReg_ptr
            ,int data,int ptr)
{
    if(ptr==0){//���ݼĴ������������Ƚϣ� 
        if(gprPtr->dataReg[data-1]==immeValue)
            sysReg_ptr->signRegister=0;
        else if(gprPtr->dataReg[data-1]>immeValue)
            sysReg_ptr->signRegister=1;
        else if(gprPtr->dataReg[data-1]<immeValue)
            sysReg_ptr->signRegister=-1;
    }else{//��ַ�Ĵ�����ָ�ڴ������ݼĴ����ıȽϣ� 
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
    (sign==3&&sysReg_ptr->signRegister==-1))//��������ת;��־�Ĵ���Ϊ0ʱ��ת;��־�Ĵ���Ϊ1ʱ��ת;��־�Ĵ���Ϊ-1ʱ��ת��  
        sysReg_ptr->proCount+=(immeValue-4);
}
 
void input_or_output(int operCode,GPR_PTR gprPtr,int data)
{
    if(operCode==12)
        printf("out: %d\n",gprPtr->dataReg[data-1]);//������ݼĴ����ڵ�ֵ��
    else{printf("in:\n");
    scanf("%d",&gprPtr->dataReg[data-1]);//�����ֵ�������ݼĴ����� 
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
    if(operCode==0){//ͣ��ָ� 
        print(sysReg_ptr,gprPtr);
        exitPrint(codeMemory,dataMemory);
    }
    if(operCode==1)//���ݴ���ָ� 
        dataPass(gprPtr,immeValue,dataMemory,data,ptr);
    if(operCode>=2&&operCode<=8)//�������߼����㣻 
        OPERATION(operCode,gprPtr,immeValue,dataMemory,data,ptr);
    if(operCode==9)//�Ƚ�ָ� 
        COMP(gprPtr,immeValue,dataMemory,sysReg_ptr,data,ptr);
    if(operCode==10)//��תָ� 
        jumpInstruction(immeValue,ch,sysReg_ptr); 
    if(operCode==11||operCode==12)//�������ָ� 
        input_or_output(operCode,gprPtr,data);
}
 
void exitPrint(int codeMemory[X1],int dataMemory[X2])
{
    int i;
    printf("\ncodeSegment :\n");
    for(i=1;i<X1;i++){ //�������Σ� 
        if(i%EIGHT!=0) printf("%d ",codeMemory[i-1]);
        else printf("%d\n",codeMemory[i-1]);
    }printf("\ndataSegment :\n");
    for(i=1;i<X2;i++){ //������ݶΣ� 
        if(i%SIXTEEN!=0) printf("%d ",dataMemory[i-1]);
        else printf("%d\n",dataMemory[i-1]);
    }exit(0);
}
 
void print(SystemReg_PTR sysReg_ptr,GPR_PTR gprPtr)
{//���11���Ĵ�����״̬�� 
    printf("ip = %d\n",sysReg_ptr->proCount);
    printf("flag = %d\n",sysReg_ptr->signRegister);
    printf("ir = %d\n",sysReg_ptr->instruRegister);
    printf("ax1 = %d ax2 = %d ax3 = %d ax4 = %d\n",gprPtr->dataReg[0],gprPtr->dataReg[1],gprPtr->dataReg[2],gprPtr->dataReg[3]);
    printf("ax5 = %d ax6 = %d ax7 = %d ax8 = %d\n",gprPtr->ptrReg[0],gprPtr->ptrReg[1],gprPtr->ptrReg[2],gprPtr->ptrReg[3]);
}
