//
//  txt_option.cpp
//  test
//
//  Created by 周伟杰 on 2021/12/20.
//

#include "txt_option.hpp"


char* TxtOption::ReadLineData(int lineNum, char* data)
{
    ifstream in;
    in.open(fileName);
 
    int line = 1;
    while (in.getline(data, 1024))
    {
        if (lineNum == line)
        {
            break;
        }
        line ++ ;
    }
 
    in.close();
    return data;
}
 

string TxtOption::CharToStr(char * contentChar)
{
    string tempStr;
    for (int i=0;contentChar[i]!='\0';i++)
    {
        tempStr+=contentChar[i];
    }
    return tempStr;
}
 
 

void TxtOption::DelLineData(int lineNum)
{
    ifstream in;
    in.open(fileName);
    
    string strFileData = "";
    int line = 1;
    char lineData[1024] = {0};
    while(in.getline(lineData, sizeof(lineData)))
    {
        if (line == lineNum)
        {
            strFileData += "\n";
        }
        else
        {
            strFileData += CharToStr(lineData);
            strFileData += "\n";
        }
        line++;
    }
    in.close();
 
    //写入文件
    ofstream out;
    out.open(fileName);
    out.flush();
    out<<strFileData;
    out.close();
}
 

void TxtOption::ModifyLineData(int lineNum, char* lineData)
{
    ifstream in;
    in.open(fileName);
 
    string strFileData = "";
    int line = 1;
    char tmpLineData[1024] = {0};
    while(in.getline(tmpLineData, sizeof(tmpLineData)))
    {
        if (line == lineNum)
        {
            strFileData += CharToStr(lineData);
            strFileData += "\n";
        }
        else
        {
            strFileData += CharToStr(tmpLineData);
            strFileData += "\n";
        }
        line++;
    }
    in.close();
 
    //写入文件
    ofstream out;
    out.open(fileName);
    out.flush();
    out<<strFileData;
    out.close();
}
 
