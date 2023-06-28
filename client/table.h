#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <fstream>

using namespace std;

class Table {

public:
  Table (string name, string tableName);
  string GetName() {return _tableName;}
  void BuildJoinIndex(string key_file,string eneq_file,string enkd_file,string ibf_file,string EqColName,string RaColName1,string RaColName2);
    
private:
  Table();
  string _tableName;
  string _fileName;
  fstream _file;
  vector<string> _headers;
  vector< vector<string>> _table;
  vector<pair<string, int>> ExtractStringColumn(int colId, const Table& table);
  int FindColumn(string colName) const;
  void Parse();
  void PrintHeaders(const vector<string>& headers);
};

class Pos{

public:  
  int count;
  int start;
  int end;
  int flag;
  int *arr;

public:
  Pos(){}
  Pos(int len)
  {
    count = 1;
    start = 0;
    end = 0;
    flag = 0;
    arr = new int[len];
  }
  void set(int s,int e)
  {
    start = s;
    end = e;
  }

};

#endif
