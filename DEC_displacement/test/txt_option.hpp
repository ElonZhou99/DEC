//
//  txt_option.hpp
//  test
//
//  Created by 周伟杰 on 2021/12/20.
//

#ifndef txt_option_hpp
#define txt_option_hpp

#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace::std;

class TxtOption {
public:
    char* ReadLineData(int lineNum, char* data);
    string CharToStr(char * contentChar);
    void DelLineData(int lineNum);
    void ModifyLineData(int lineNum, char* lineData);
    
public:
    char lineDara[1024] = {};
    string fileName = "info.txt";
};

#endif /* txt_option_hpp */
