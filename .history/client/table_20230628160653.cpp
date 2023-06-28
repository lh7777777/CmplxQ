#include "table.h"
#include "ibf.h"
#include <sstream>
#include <functional>
#include <unordered_map>
#include "time.h"

Table::Table(string fileName, string tableName) : _fileName(fileName), 
  _tableName(tableName) {
  _file.open(_fileName.c_str(), fstream::in);
  if (!_file.good()) {
    cerr << "Table reading error for file : " << _fileName << endl; 
  }
  Parse();
}

vector<pair<string,int>> Table::ExtractStringColumn(int colId, const Table& table) {
  vector<pair<string,int>> column;
  for (int i = 0; i < table._table.size(); i++) {
    column.push_back(pair<string,int>(table._table[i][colId], i));
  }
  return column;
}

int Table::FindColumn(string colName) const {
  int columnId = -1;
  for (int i = 0; i < _headers.size(); i++) {
    if(_headers[i] == colName) {
      columnId = i;
    }
  }
  if (columnId == -1) {
    cerr << "Column " << colName << " not found." << endl;
  }
  return columnId;
}

void Table::Parse() {
  string line;
  string temp;

  getline(_file, line);
  istringstream iss(line);
  while (getline(iss, temp, ',')) {
    _headers.push_back (temp);
  }
  
  while (getline(_file, line)) {
    istringstream iss(line);
    vector<string> values;
    while (getline(iss, temp, ',')) {
      values.push_back(temp);
    }
    _table.push_back (values);
  }
}

void Table::PrintHeaders(const vector<string>& headers) {
  string sheaders (""), dashes;
  for (auto colName : headers) {
    dashes += "--------";
    sheaders += colName + "\t";
  }
  cout << dashes << endl;
  cout << sheaders << endl;
  cout << dashes << endl;
}

void Table::BuildJoinIndex(string key_file,string eneq_file,string enkd_file,string ibf_file,string EqColName,string RaColName1,string RaColName2)
{

  int EqColId = FindColumn(EqColName);
  if (EqColId == -1) {
    return;
  }

  vector<pair<string, int>> Col = ExtractStringColumn(EqColId, *this);

  unordered_map<string,Pos> HT;
  int len = Col.size();
  Pos pos(len);

  for (auto m : Col) {
    if(HT.find(m.first) != HT.end())
    {
      HT[m.first].count += 1;
    }
    else
    {
      HT[m.first] = pos;
    }
  }

  int ptr = 0;
  for (auto m : Col) {

    if(HT[m.first].count != 1)
    {
      if(HT[m.first].flag == 0)
      {
        HT[m.first].set(ptr,ptr);
        ptr += HT[m.first].count;
        HT[m.first].flag = 1;
      }      
    }
    else
    {
      HT[m.first].set(ptr,ptr);
      ptr += HT[m.first].count;
    }
  }


  for (auto m : Col) {
    int temp_end = HT[m.first].end;
    HT[m.first].arr[temp_end] = m.second;
    HT[m.first].end++;
  }

  ofstream eneq(eneq_file);
  ofstream enkd(enkd_file);
  ofstream ibffile(ibf_file);

  ifstream keyfile(key_file);
  string key1;
  string key2;
  string iv;
  keyfile >> key1 >> key2 >> iv;
  long long longiv = atoll(iv.c_str());

  size_t ll=0;
  int RaColId1 = FindColumn(RaColName1);
  int RaColId2 = FindColumn(RaColName2);
  IBloomFilter<string> *ibf = new IBloomFilter<string>(bfnlen);
  
  for(auto p : HT) {
    
    ll++;
    vector<int> columnid;
    string x_encode;
    string x_encode_base64;
    string y_encode;
    string y_encode_base64;
    string index_encode;
    string index_encode_base64;

    int j = p.second.start;
      while(j<p.second.end)
      {
        string kdxiv = to_string(longiv);
        longiv++;
        x_encode = aes_256_cbc_encode(key1,kdxiv, _table[p.second.arr[j]][RaColId1]);
        x_encode_base64 = base64_encode(x_encode.c_str(), x_encode.length());
        string kdyiv = to_string(longiv);
        longiv++;
        y_encode = aes_256_cbc_encode(key1,kdyiv, _table[p.second.arr[j]][RaColId2]);
        y_encode_base64 = base64_encode(y_encode.c_str(), y_encode.length());
        enkd << x_encode_base64 << " " << y_encode_base64 << " ";

        columnid.push_back(p.second.arr[j]);
        j++;
      }
      if(ll < HT.size())
        enkd << endl;
      
      index_encode = aes_256_cbc_encode(key2,iv, p.first);
      index_encode_base64 = base64_encode(index_encode.c_str(), index_encode.length());
      eneq << index_encode_base64;
      if(ll < HT.size())
        eneq << endl;
      
      ibf->iset(p.first);
  }

  for(int y=0;y<bfmlen;y++)
	{
		ibffile << ibf->_ibs._ibitset[0][y];
	}
  for(int y=0;y<bfmlen;y++)
	{
		ibffile << ibf->_ibs._ibitset[1][y];
	}
  ibffile << endl;

  eneq.close();
  enkd.close();
  ibffile.close();

}
