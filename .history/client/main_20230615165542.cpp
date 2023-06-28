#include "table.h"
#include "encode.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <memory>
#include <map>

using namespace std;

shared_ptr<Table> FindTable(string table, vector<shared_ptr<Table>>& tables) {
  shared_ptr<Table> ptr;
  for (auto t : tables) {
    if (t->GetName() == table) {
      ptr = t;
      break;
    }
  }
  return ptr;
}

int main(int argc, char *argv[]) {
  string line;
  vector<shared_ptr<Table>> tables;

  cout << "Please load csv files, sample command: "
       << "load a test1.csv" << endl;
  while (getline(cin, line)) {
    istringstream iss(line);
    string operation;
    iss >> operation;

    if (operation == "load") {
      string table, file;
      iss >> table >> file;
      shared_ptr<Table> tablePtr(new Table(file, table));
      tables.push_back(tablePtr);
    }else if (operation == "buildindex") {
      string table;
      string file1, file2, file3, file4;
      string eqcol, racol1, racol2;
      string on;
      iss >> table >> file1 >> file2 >> file3 >> file4 >> on >> eqcol >> racol1 >> racol2;
   
      shared_ptr<Table> tablePtr = FindTable(table, tables);
      if (!tablePtr) {
        cerr << "Table: " << table << " not found!" << endl;
      } else {
        tablePtr->BuildJoinIndex(file1,file2,file3,file4,eqcol,racol1,racol2);
      }
    } else if (operation == "rangejoin") {
      string type;
      string x1,x2,y1,y2;
      string x3,x4,y3,y4;
      string sym1,sym2;
      
      string table1,table2;
      string eq1,eq2;
      string And1,And2;
      string On1,On2;
      iss >> table1 >> eq1 >> On1 >> table2 >> eq2 >> And1 >> type >> On2;
   
      shared_ptr<Table> tablePtr1 = FindTable(table1, tables);
      shared_ptr<Table> tablePtr2 = FindTable(table2, tables);
      if (!tablePtr1) {
        cerr << "Table: " << table1 << " not found!" << endl;
      } else if (!tablePtr2) {
        cerr << "Table: " << table2 << " not found!" << endl;
      } else {
        ofstream enquery("/var/lib/mysql/file/enquery.txt");
        // AES key
        string key = "12345678901234561234567890123456";
        string iv = "1000000000000000";
        long long longiv1 = atoll(iv.c_str());
        long long longiv2 = atoll(iv.c_str());
        long long longiv3 = atoll(iv.c_str());
        longiv1++;
        longiv2++;
        longiv3++;

        string type_encode = aes_256_cbc_encode(key,iv, type);
        string type_encode_base64 = base64_encode(type_encode.c_str(), type_encode.length());
        // 加密后的结果，以base64编码写入文件enquery
        enquery << type_encode_base64 << " ";
        

        if(type == "3")
        {
          iss >> x1 >> x2 >> y1 >> y2 >> x3 >> x4 >> y3 >> y4;
          string newiv1 = to_string(longiv1);
          longiv1++;
          string x1_encode = aes_256_cbc_encode(key,newiv1, x1);
          string x1_encode_base64 = base64_encode(x1_encode.c_str(), x1_encode.length());
          enquery << x1_encode_base64 << " ";

          string newiv2 = to_string(longiv1);
          longiv1++;
          string x2_encode = aes_256_cbc_encode(key,newiv2, x2);
          string x2_encode_base64 = base64_encode(x2_encode.c_str(), x2_encode.length());
          enquery << x2_encode_base64 << " ";

          string newiv3 = to_string(longiv1);
          longiv1++;
          string y1_encode = aes_256_cbc_encode(key,newiv3, y1);
          string y1_encode_base64 = base64_encode(y1_encode.c_str(), y1_encode.length());
          enquery << y1_encode_base64 << " ";

          string newiv4 = to_string(longiv1);
          longiv1++;
          string y2_encode = aes_256_cbc_encode(key,newiv4, y2);
          string y2_encode_base64 = base64_encode(y2_encode.c_str(), y2_encode.length());
          enquery << y2_encode_base64 << " ";

          string newiv5 = to_string(longiv1);
          longiv1++;
          string x3_encode = aes_256_cbc_encode(key,newiv5, x3);
          string x3_encode_base64 = base64_encode(x3_encode.c_str(), x3_encode.length());
          enquery << x3_encode_base64 << " ";

          string newiv6 = to_string(longiv1);
          longiv1++;
          string x4_encode = aes_256_cbc_encode(key,newiv6, x4);
          string x4_encode_base64 = base64_encode(x4_encode.c_str(), x4_encode.length());
          enquery << x4_encode_base64 << " ";

          string newiv7 = to_string(longiv1);
          longiv1++;
          string y3_encode = aes_256_cbc_encode(key,newiv7, y3);
          string y3_encode_base64 = base64_encode(y3_encode.c_str(), y3_encode.length());
          enquery << y3_encode_base64 << " ";

          string newiv8 = to_string(longiv1);
          longiv1++;
          string y4_encode = aes_256_cbc_encode(key,newiv8, y4);
          string y4_encode_base64 = base64_encode(y4_encode.c_str(), y4_encode.length());
          enquery << y4_encode_base64;
        }
        else if(type == "4")
        {
          iss >> sym1 >> x1 >> x2 >> And2 >> sym2 >> y1 >> y2;

          string newiv1 = to_string(longiv2);
          longiv2++;
          string sym1_encode = aes_256_cbc_encode(key,newiv1, sym1);
          string sym1_encode_base64 = base64_encode(sym1_encode.c_str(), sym1_encode.length());
          enquery << sym1_encode_base64 << " ";
          
          string newiv2 = to_string(longiv2);
          longiv2++;
          string x1_encode = aes_256_cbc_encode(key,newiv2, x1);
          string x1_encode_base64 = base64_encode(x1_encode.c_str(), x1_encode.length());
          enquery << x1_encode_base64 << " ";

          string newiv3 = to_string(longiv2);
          longiv2++;
          string x2_encode = aes_256_cbc_encode(key,newiv3, x2);
          string x2_encode_base64 = base64_encode(x2_encode.c_str(), x2_encode.length());
          enquery << x2_encode_base64 << " ";

          string newiv4 = to_string(longiv2);
          longiv2++;
          string sym2_encode = aes_256_cbc_encode(key,newiv4, sym2);
          string sym2_encode_base64 = base64_encode(sym2_encode.c_str(), sym2_encode.length());
          enquery << sym2_encode_base64 << " ";

          string newiv5 = to_string(longiv2);
          longiv2++;
          string y1_encode = aes_256_cbc_encode(key,newiv5, y1);
          string y1_encode_base64 = base64_encode(y1_encode.c_str(), y1_encode.length());
          enquery << y1_encode_base64 << " ";

          string newiv6 = to_string(longiv2);
          longiv2++;
          string y2_encode = aes_256_cbc_encode(key,newiv6, y2);
          string y2_encode_base64 = base64_encode(y2_encode.c_str(), y2_encode.length());
          enquery << y2_encode_base64;
        }
        else
        {
          iss >> x1 >> x2 >> y1 >> y2;

          string newiv1 = to_string(longiv3);
          longiv3++;
          string x1_encode = aes_256_cbc_encode(key,newiv1, x1);
          string x1_encode_base64 = base64_encode(x1_encode.c_str(), x1_encode.length());
          enquery << x1_encode_base64 << " ";

          string newiv2 = to_string(longiv3);
          longiv3++;
          string x2_encode = aes_256_cbc_encode(key,newiv2, x2);
          string x2_encode_base64 = base64_encode(x2_encode.c_str(), x2_encode.length());
          enquery << x2_encode_base64 << " ";

          string newiv3 = to_string(longiv3);
          longiv3++;
          string y1_encode = aes_256_cbc_encode(key,newiv3, y1);
          string y1_encode_base64 = base64_encode(y1_encode.c_str(), y1_encode.length());
          enquery << y1_encode_base64 << " ";

          string newiv4 = to_string(longiv3);
          longiv3++;
          string y2_encode = aes_256_cbc_encode(key,newiv4, y2);
          string y2_encode_base64 = base64_encode(y2_encode.c_str(), y2_encode.length());
          enquery << y2_encode_base64;
        }
        enquery.close();
      }  
      
    } else if (operation == "exit") {
      return 0;
    } else {
      cerr << "Unknown operation!" << endl;
      cerr << "Supported operations are: load, select, join, compute, aggregation" 
        << endl;
    }
  }
  return 0;
}
