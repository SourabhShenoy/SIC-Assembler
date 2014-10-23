//#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
fstream f1,f2,f3,f4,f5;
int t=0;

class TWOPASS
{
    public:
    char symb[10];
    unsigned int loc;
    void pass1();
    void pass2();
}twopass[50];


//1st pass of the two pass assembler
//At the end of this pass, an intermediate file is generated which will be fed as input to the next pass
//This is to solve the problem of forward referencing
void TWOPASS::pass1()
{
    char opcode[10],label[10],operand[20],code[10],maccode[5];
    unsigned int loc,start;
    int j=0,n,len,f,found,i;
    f1>>label>>opcode>>operand;
    cout<<"\nContents of Intermediate file are:"<<endl;
    if(strcmp(opcode,"START")==0)
    {
        start=strtol(operand,NULL,16);
        loc=start;
        f2<<"-"<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
        cout<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
         //writing into intermediate file
       f1>>label>>opcode>>operand;
    }
    else if(strcmp(opcode,"")==0)
    {
        loc=0000;
    }
    //start=0000;
     //Else set location to 0
    f2<<hex<<loc;
    cout<<hex<<loc;  //writing starting address of the next record to location
    while(strcmp(opcode,"END")!=0)
    {
        //Checking for existence of label
        if(strcmp(label,"-")!=0)
        {
            //Which means label exists
            f=0;
            if(strcmp(twopass[0].symb,"\0")!=0)    //if there exist atleast one symbol in twopass.symb
            {
                for(i=0;i<50;i++)
                {
                    if(strcmp(label,twopass[i].symb)==0)  //search for label in twopass.symb
                    {
                        cout<<"\n duplicate symbol"<<endl; //If found, set error flag to one and print duplicate symbol
                        f=1;
                        break;
                    }
                }
                if(f==0)  //Else insert symbol, address to twopass
                {
                    strcpy(twopass[j].symb,label);
                    twopass[j].loc=loc;
                    j++;
                }
            }
            else
             {
                 //If empty, Directly insert symbol, address to twopass
                    strcpy(twopass[j].symb,label);
                    twopass[j].loc=loc;
                    j++;
             }
        }
        f3.open("optab.txt",ios::in|ios::out);
        //Get machine code from optab
        f3>>code>>maccode;
        found=0;
        //Set every address= address+3 if same as opcode
        while(f3)
        {
            if(strcmp(opcode,code)==0)
            {
                found=1;
                loc=loc+0x3;
                break;
            }
            f3>>code>>maccode;

        }

        f3.close();

        if(found==0)
        {
            //Add 3 for a WORD, 3*HEX for RESW, HEX for RESB
            if(strcmp(opcode,"WORD")==0)
               loc=loc+0x3;
            else if(strcmp(opcode,"RESW")==0)
                loc=loc+(0x3*atol(operand));
             else if(strcmp(opcode,"RESB")==0)
                loc=loc+atoi(operand);
             else if(strcmp(opcode,"BYTE")==0)
              {
                  //For BYTE
                  //If it is Hexadecimal
                  if(operand[0]=='X')
                  {
                        n=strlen(operand);
                        // -3 to exclude the 1st character and two quotes
                        n-=3;
                        // 2 digits take up 1 byte, hence divide by 2
                        n/=2;
                        loc+=n;
                  }
                  else
                  {
                      //If it is character
                      n=strlen(operand);
                      // -3 to exclude the 1st character and two quotes
                      n-=3;
                      loc+=n;
                  }
              }
              else
                //If opcode doesn't match any of the above
                 cout<<"Invalid opcode"<<endl;
        }
        f2<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
        cout<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
        f2<<loc;
        cout<<loc;
        //Fetch the next record
        f1>>label>>opcode>>operand;

    }

     f2<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
     cout<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
     //Store the program length
     len=loc-start;
     cout<<"\nLength of the program is: "<<len<<" bytes"<<endl;
     //Store into symbol.txt
     for(i=0;i<j;i++)
     {
         f4<<twopass[i].symb<<"\t"<<hex<<twopass[i].loc<<endl;

     }
     f4<<"END";
     f2.close();
     f4.close();
}


//Now the intermediate file generated in the first pass is fed as input to the second pass
void TWOPASS::pass2()
{

    char c[4],add[10],opcode[10],label[10],object[50],operand[50],code[10],maccode[10],symbol[10],addr[10],buff[10],String[10][10];
    long start,fin,len1,st;
    int found,k=0,j,str1[10],c1,l,d,f,b[4],i,r,len;
    f2.open("intermediate.txt",ios::in|ios::out);
    f2>>add>>label>>opcode>>operand;
    //length of the program
    //If opcode is START
    if(strcmp(opcode,"START")==0)
          start=strtol(operand,NULL,16);
    //Till End of File
    while(strcmp(opcode,"END")!=0)
          f2>>add>>label>>opcode>>operand;
    //Store last address
    fin=strtol(add,NULL,16);

	//Length of Object Program
    len1=fin-start;
    f2.close();
    f2.open("intermediate.txt",ios::in|ios::out);
    f5.open("output.txt",ios::in|ios::out);
    f2>>add>>label>>opcode>>operand;
    if(strcmp(opcode,"START")==0)
    {
        f5<<add<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<"\t"<<"-"<<endl;
        f2>>add>>label>>opcode>>operand;
    }
    //Till the end
    while(strcmp(opcode,"END")!=0)
     {
        f5<<add<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<"\0";
		//Open in I/O mode
		f3.open("optab.txt",ios::in|ios::out);
        f3>>code>>maccode;
        while(f3)
        {
            found=0;
            if(strcmp(opcode,code)==0)
            {
                found=1;
			//If Operand Exists
                if(strcmp(operand,"-")!=0)
                {
                    f4.open("symbol.txt",ios::in|ios::out);
                    f4>>symbol>>addr;
                    while(f4)
                    {
                        f=0;
                        if(strcmp(operand,symbol)==0)
                        {
                            f=1;
                            f5<<"\t"<<maccode<<addr<<endl;
                            break;
                        }
                        f4>>symbol>>addr;
                    }
                    f3.close();
                    f4.close();
                    if(f==0)
                    {
                        l=strlen(operand);
                        if((operand[l-1]=='X') && (operand[l-2]==','))
                        {
                             for(k=0;k<l-2;k++)
                              {
                                  buff[k]=operand[k];
                               }
                             buff[k]='\0';
                            f4.open("symbol.txt",ios::in|ios::out);
                             f4>>symbol>>addr;
                             while(f4)
                             {
                                if(strcmp(buff,symbol)==0)
                                  {
                                     c[0]=addr[0];
                                     d=strtol(c,NULL,16);
                                      for(i=0;i<4;i++)
                                       {
                                          b[i]=0;
                                       }
                                     i=3;
                                     while(i!=0)
                                     {
                                       if(d!= 0)
                                        {
                                           r = d % 2;
                                           b[i] =r;
                                           d=d / 2;
                                        }
                                        i--;
                                     }
                                    b[0]=b[0]+1;
                                    j=0;
                                    len=0;
                                    for(i=3;i>=0;i--)
                                    {
                                       len+=b[i]*pow(2,j);
                                       j++;
                                     }
                                    f5<<"\t"<<maccode<<hex<<len;
                                     for(i=1;i<4;i++)
                                     {
                                         f5<<addr[i];
                                     }
                                     f5<<endl;
                                     break;
                                  }
                               f4>>symbol>>addr;
                             }
                             f4.close();
                          }
                    }
                 }
                else
                {
                   f5<<maccode<<"0000"<<endl;
                }
                break;
            }
            f3>>code>>maccode;
        }
        f3.close();
        if(found==0)
        {
           if(strcmp(opcode,"BYTE")==0)
           {
               f5<<"\t";
               if(operand[0]=='X')
               {
                   len=strlen(operand);
                   for(k=2;k<len-1;k++)
                   {
                       f5<<operand[k];
                   }
               }
               else if(operand[0]=='C')
               {

                   j=0;

                   len=strlen(operand);

                   for(k=2;k<len-1;k++)
                   {
                       str1[j]=operand[k];
                       f5<<hex<<str1[j];
                       j++;
                   }
               }

            }
           else if(strcmp(opcode,"WORD")==0)
           {
               c1=atol(operand);
               f5<<"00000";
               f5<<hex<<"\t";
               f5<<"00000"<<c1<<endl;
           }
           else
           {
               f5<<"\t"<<"-"<<endl;
           }
        }
        f2>>add>>label>>opcode>>operand;

    }
    f5<<"\n"<<add<<"\t"<<label<<"\t"<<opcode<<"\t"<<operand<<endl;
    f5.close();
   f5.open("output.txt",ios::in|ios::out);
    cout<<"\nOBJECT CODE\n";
    f5>>add>>label>>opcode>>operand>>object;
    //HEADER Record
    cout<<"\n"<<"H^"<<label<<"^00"<<start<<"^"<<len1;
   f5>>add>>label>>opcode>>operand>>object;
    l=0;
    j=0;
    f=0;
    while(strcmp(opcode,"END")!=0)
    {
       if(strcmp(object,"-")!=0)
        {
            if(j<=60)
            {
                if(l==0)
                {
                     st=strtol(add,NULL,16);
                }
                strcpy(String[l],object);
                l++;
                j+=strlen(object);
            }
            if(j>60)
            {
                 cout<<"\n"<<"T^00"<<st;
                 len=strtol(add,NULL,16)-st;
                 cout<<"^"<<len;
                 for(i=0;i<l-1;i++)
                 {
                      cout<<"^"<<String[i];
                 }
                 j=0;
                 l=0;
                 continue;
            }
         }
        else if(strcmp(object,"-")==0)
        {
            if(j!=0)
            {
                cout<<"\n"<<"T^00"<<st;
                len=strtol(add,NULL,16)-st;
                cout<<"^"<<len;
                for(i=0;i<l;i++)
                {
                    cout<<"^"<<String[i];
                }
                j=0;
                l=0;
                continue;
            }
         }
         f5>>add>>label>>opcode>>operand>>object;
     }
	//Entering last text record
    if(j!=0)
    {
        cout<<"\n"<<"T^00"<<st;
        len=strtol(add,NULL,16)-st;
        cout<<"^"<<len;
        for(i=0;i<l;i++)
        {
             cout<<"^"<<String[i];
        }
    }

    //Write the END Record
    cout<<"\n"<<"E^00"<<start;
    f5.close();
}


int main()
{
    TWOPASS p1;
    cout<<"Two Pass Assembler"<<endl;
    f1.open("input.txt",ios::in|ios::out);
    f2.open("intermediate.txt",ios::in|ios::out);
    f4.open("symbol.txt",ios::in|ios::out);
    if(f1==NULL)
    {
        cout<<"file doesn't exist"<<endl;
        exit(0);
    }
    else
    {
        p1.pass1();
        p1.pass2();
    }
    f1.close();
    f2.close();
    f3.close();
    f4.close();

    return 0;
}
