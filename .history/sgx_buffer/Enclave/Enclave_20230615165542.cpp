/*
 * Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "Enclave_t.h" /* print_string */
#include "baddtree.h"
#include "data_ebuffer.h"
#include "encode.h"
#include "kdtree.h"
#include "BloomFilter.h"
#include "Ocall_wrappers.h"

#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <vector>
#include <time.h>

#include <functional>
#include <memory>
#include <unordered_map>

using namespace BAT;

string cbc_key1 = "12345678901234561234567890123456";
string cbc_key2 = "98765432101234569876543210123456";
string iv1 = "1000000000000000";
string iv2 = "1111111111111111";
long long longiv1 = atoll(iv1.c_str());
long long longiv2 = atoll(iv2.c_str());
long long liv1 = atoll(iv1.c_str());
long long liv2 = atoll(iv1.c_str());
long long liv3 = atoll(iv1.c_str());
long long liv4 = atoll(iv1.c_str());

BAddTree<int,k_r>* tree;

void ecall_init(int order)
{
    printf("ecall_init\n");
    for(int i=1;i<=EBUFFER_SIZE;i++)
    {
        MBuf_id mbuf_id;
        mbuf_id.page_id = (i-1)/4 + 1;
        mbuf_id.offset = (i-1)%4;
        node2page[i] = mbuf_id;
    }
    for(int i = 0; i < 16; i++)
    {
        key[i] = 32 + i;
    }

    tree = new BAddTree<int,k_r>(order);

}

int ecall_search(int key,void* mbdes,char** mbpool)
{
    printf("ecall_search\n");
    return tree->find(key,(MBuf_des*)mbdes,mbpool)->rid;
    
}

void ecall_insert(void* key_rid,void* mbdes,char** mbpool)
{
    printf("ecall_insert\n");
    k_r* kr = (k_r*)key_rid;
    tree->insert(*kr,(MBuf_des*)mbdes,mbpool);
    
}

auto itf = [&](deque <k_r*>&e) {
	int _s = e.size();
	for (int i = 0; i < _s; ++i) {
		printf("%d ", e[i]->k);
	}
};

void ecall_traversal()
{
    printf("ebuffer size: %d\n",tree->ebuffer->size);
    printf("tree size: %d\n",tree->size());
    printf("start traversal:\n");
    tree->list_traversal(itf);
}

Data_EBuffer *Def;
Data_EBuffer *Def1;
Data_EBuffer *Def2;

void ecall_data_init()
{
    printf("ecall_data_init\n");

    for(int i = 0; i < 16; i++)
    {
        key2[i] = 32 + i;
    }

    Def = new Data_EBuffer(-1);
}
char* ecall_data_search(int rid,void* mbdes,char** mbpool)
{
    printf("ecall_data_search\n");
    return Def->SearchData(rid,(MBuf_des*)mbdes,mbpool);
    
}

void ecall_data_insert(char* newdata,void* mbdes,char** mbpool)
{
    printf("ecall_data_insert\n");
    Def->InsertData(newdata,(MBuf_des*)mbdes,mbpool);
    
}

void ecall_kd_init()
{
    printf("ecall_kd_init\n");
    for(int i = 0; i < 16; i++)
    {
        key2[i] = 32 + i;
    }
    Def1 = new Data_EBuffer(-1);
    Def2 = new Data_EBuffer(49999);
}

vector<int> result1;//存的下时
vector<int> result2;
vector< pair<int,int> > storage2;

void eqjoin(char** ein1,char** ein2,char* ibf1,char* ibf2)
{
    
    int len1 = 0;
    vector<string> index1;
    ocall_read_eneq1(&len1,ein1);
    ocall_read_ibf1(ibf1); 
    IBloomFilter<string> *ibf1 = new IBloomFilter<string>(bfnlen,ibf1);

    int len2 = 0;
    vector<string> index2;
    ocall_read_eneq2(&len2,ein2);
    ocall_read_ibf2(ibf2);   
    IBloomFilter<string> *ibf2 = new IBloomFilter<string>(bfnlen,ibf2);

    for(int p1=0;p1<len1;p1++)
    {

        string enindex1 = ein1[p1];
        string p1_decode_base64 = base64_decode(enindex1);
	    string p1_decode = aes_256_cbc_decode(cbc_key2,iv1, p1_decode_base64);
        index1.push_back(p1_decode.c_str());

        if(ibf2->IsInIBloomFilter(index1[p1]) == 1)
        {
            result1.push_back(p1);
        }

    }

    for(int p2=0;p2<len2;p2++)
    {

        string enindex2 = ein2[p2];
        string p2_decode_base64 = base64_decode(enindex2);
	    string p2_decode = aes_256_cbc_decode(cbc_key2,iv2, p2_decode_base64);
        index2.push_back(p2_decode.c_str());
	    
	    if(ibf1->IsInIBloomFilter(index2[p2]) == 1)
        {
            result2.push_back(p2);
        }

    }

    unordered_map<string,int> HTE;
    for(auto h1 : result1)
    {
        HTE[index1[h1]] = h1;
    }
    for(auto h2 : result2)
    {
        if(HTE.find(index2[h2]) != HTE.end())
        {
            storage2.push_back(pair<int,int>(HTE[index2[h2]],h2));
        }
    }

}

void BuildKD(char** ekd1,char** ekd2,void* mbdes,char** mbpool)
{
    int* cn1 = new int[10000];
    int cnlen1 = 0;
    cnlen1 = ocall_read_enkd1(cn1,ekd1);
    int sum1 = 0;
    for(int i=0;i<cnlen1;i++)
    {
        vector<double> xda1;
        vector<double> yda1;
        for(int j=sum1;j<sum1+cn1[i];j++)
        {
            string tt1 = ekd1[j];
            string kdiv1 = to_string(longiv1);
            longiv1++;
            string kd_decode_base64 = base64_decode(tt1);
	        string kd_decode = aes_256_cbc_decode(cbc_key1,kdiv1, kd_decode_base64);
            double da1 = atof(kd_decode.c_str());

            if((j%2)==0)
            {
                xda1.push_back(da1);
            }
            else
            {
                yda1.push_back(da1);               
            }
        }

       
        vector< vector<double> > data1;
        for(int p=0;p<(cn1[i]/2);p++)
        {
            vector<double> xy;
            xy.push_back(xda1[p]);
            xy.push_back(yda1[p]);
            data1.push_back(xy);
        }
        KDTree *kdtree1 = new KDTree(2,data1);
        Def1->InsertData((char*)kdtree1,(MBuf_des*)mbdes,mbpool);

        sum1 += cn1[i];
    }

    int* cn2 = new int[10000];
    int cnlen2 = 0;
    cnlen2 = ocall_read_enkd2(cn2,ekd2);
    int sum2 = 0;
    for(int i=0;i<cnlen2;i++)
    {
        
        vector<double> xda2;
        vector<double> yda2;
        for(int j=sum2;j<sum2+cn2[i];j++)
        {
            string tt2 = ekd2[j];

            string kdiv2 = to_string(longiv2);
            longiv2++;
            string kd_decode_base64 = base64_decode(tt2);
	        string kd_decode = aes_256_cbc_decode(cbc_key1,kdiv2, kd_decode_base64);
            double da2 = atof(kd_decode.c_str());
            
            if((j%2)==0)
            {
                xda2.push_back(da2);
                
            }
            else
            {
                yda2.push_back(da2);
                
            }
        }

        vector< vector<double> > data2;
        for(int p=0;p<(cn2[i]/2);p++)
        {
            vector<double> xy2;
            xy2.push_back(xda2[p]);
            xy2.push_back(yda2[p]);
            data2.push_back(xy2);
        }
        KDTree *kdtree2 = new KDTree(2,data2);        
        Def2->InsertData((char*)kdtree2,(MBuf_des*)mbdes,mbpool);
        sum2 += cn2[i];
    }
    delete[] cn1;
    delete[] cn2;
}

void ecall_rangesearch(char** ein1,char** ein2,char** ekd1,char** ekd2,char* ibf1,char* ibf2,void* mbdes,char** mbpool)
{
    eqjoin(ein1,ein2,ibf1,ibf2);
    BuildKD(ekd1,ekd2,mbdes,mbpool);
   
    ocall_open_result1();
    ocall_open_result2();
    ocall_open_enquery();

    char *etype = new char[25];
    char *en1 = new char[25];
    char *en2 = new char[25];
    char *en3 = new char[25];
    char *en4 = new char[25];
    char *en5 = new char[25];
    char *en6 = new char[25];
    char *en7 = new char[25];
    char *en8 = new char[25];
    char *es1 = new char[25];
    char *es2 = new char[25];

    ocall_read_s(etype,25);
    string setype = etype;
    string type_decode_base64 = base64_decode(setype);
	string st = aes_256_cbc_decode(cbc_key1,iv1, type_decode_base64);

    liv1++;
    liv2++;
    liv3++;
    liv4++;

    if(strcmp(st.c_str(),"1") == 0)
    {
        printf("type1:\n");

        ocall_read_s(en1,25);
        ocall_read_s(en2,25);
        ocall_read_s(en3,25);
        ocall_read_s(en4,25);

        string sen1 = en1;
        string sen2 = en2;
        string sen3 = en3;
        string sen4 = en4;

        string niv1 = to_string(liv1);
        liv1++;
        string n1_decode_base64 = base64_decode(sen1);
	    string n1 = aes_256_cbc_decode(cbc_key1,niv1, n1_decode_base64);

        string niv2 = to_string(liv1);
        liv1++;
        string n2_decode_base64 = base64_decode(sen2);
	    string n2 = aes_256_cbc_decode(cbc_key1,niv2, n2_decode_base64);

        string niv3 = to_string(liv1);
        liv1++;
        string n3_decode_base64 = base64_decode(sen3);
	    string n3 = aes_256_cbc_decode(cbc_key1,niv3, n3_decode_base64);

        string niv4 = to_string(liv1);
        liv1++;
        string n4_decode_base64 = base64_decode(sen4);
	    string n4 = aes_256_cbc_decode(cbc_key1,niv4, n4_decode_base64);

        double sx=atof(n1.c_str());
        double tx=atof(n2.c_str());
        double sy=atof(n3.c_str());
        double ty=atof(n4.c_str());
        vector<int> de;//范围查询没找到结果的resultid

        for(auto i : result1)
        {

            int cc=0;
            for(auto h : storage2)
            {
                
                if(h.first == i)
                {
                    cc=1;
                    break;
                }  
            }
            if (cc == 1)
            {
                KDTree *tree;
                tree = (KDTree*)(Def1->SearchData(i,(MBuf_des*)mbdes,mbpool));

                vector<vector<double> > searchdata;
                vector<double> nnsdata;
                vector<double> from;
                vector<double> to;
                from.push_back(sx);
                from.push_back(sy);
                to.push_back(tx);
                to.push_back(ty);

                searchdata = tree->SearchByRegion(from,to);        
                if(searchdata.size() == 0)
                {
                    de.push_back(i);
                }
                else
                {
                    ocall_write_result1(i);
                    for(auto p : searchdata)
                    {
                        ocall_write_result1(p[0]);
                        ocall_write_result1(p[1]);
                    }
                    ocall_writeendl_result1();
                }
                
            }
        }

        for(auto s : storage2)
        {
            int flag = 0;
            for(auto d : de)
            {
                if(s.first == d)
                {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0)
            {
                ocall_write_result2(s.second);
                ocall_writeendl_result2();
            }
        }


    }
    else if(strcmp(st.c_str(),"2") == 0)
    {
        printf("type2:\n");

        ocall_read_s(en1,25);
        ocall_read_s(en2,25);
        ocall_read_s(en3,25);
        ocall_read_s(en4,25);

        string sen1 = en1;
        string sen2 = en2;
        string sen3 = en3;
        string sen4 = en4;

        string niv1 = to_string(liv2);
        liv2++;
        string n1_decode_base64 = base64_decode(sen1);
	    string n1 = aes_256_cbc_decode(cbc_key1,niv1, n1_decode_base64);

        string niv2 = to_string(liv2);
        liv2++;
        string n2_decode_base64 = base64_decode(sen2);
	    string n2 = aes_256_cbc_decode(cbc_key1,niv2, n2_decode_base64);

        string niv3= to_string(liv2);
        liv2++;
        string n3_decode_base64 = base64_decode(sen3);
	    string n3 = aes_256_cbc_decode(cbc_key1,niv3, n3_decode_base64);

        string niv4 = to_string(liv2);
        liv2++;
        string n4_decode_base64 = base64_decode(sen4);
	    string n4 = aes_256_cbc_decode(cbc_key1,niv4, n4_decode_base64);

        double sx=atof(n1.c_str());
        double tx=atof(n2.c_str());
        double sy=atof(n3.c_str());
        double ty=atof(n4.c_str());
        vector<int> de;

        for(auto i : result2)
        {
 
            int cc=0;
            for(auto h : storage2)
            {
                
                if(h.second == i)
                {
                    cc=1;
                    break;
                }  
            }
            if (cc == 1)
            {
               
                KDTree *tree;
                tree = (KDTree*)(Def2->SearchData(i+50000,(MBuf_des*)mbdes,mbpool));

                vector<vector<double> > searchdata;
                vector<double> from;
                vector<double> to;
                from.push_back(sx);
                from.push_back(sy);
                to.push_back(tx);
                to.push_back(ty);
                searchdata = tree->SearchByRegion(from,to);
                delete tree;

                if(searchdata.size() == 0)
                {
                    de.push_back(i);
                }
                else
                {
                    ocall_write_result2(i);
                    for(auto p : searchdata)
                    {
                        ocall_write_result2(p[0]);
                        ocall_write_result2(p[1]);
                    }
                    ocall_writeendl_result2();
                }
            }
        }
        for(auto s : storage2)
        {
            int flag = 0;
            for(auto d : de)
            {
                if(s.second == d)
                {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0)
            {
                ocall_write_result1(s.first);
                ocall_writeendl_result1();
            }
        }

    }
    else if(strcmp(st.c_str(),"3") == 0)
    {
        printf("type3:\n");

        ocall_read_s(en1,25);
        ocall_read_s(en2,25);
        ocall_read_s(en3,25);
        ocall_read_s(en4,25);

        string sen1 = en1;
        string sen2 = en2;
        string sen3 = en3;
        string sen4 = en4;

        string niv1 = to_string(liv3);
        liv3++;
        string n1_decode_base64 = base64_decode(sen1);
	    string n1 = aes_256_cbc_decode(cbc_key1,niv1, n1_decode_base64);

        string niv2 = to_string(liv3);
        liv3++;
        string n2_decode_base64 = base64_decode(sen2);
	    string n2 = aes_256_cbc_decode(cbc_key1,niv2, n2_decode_base64);

        string niv3 = to_string(liv3);
        liv3++;
        string n3_decode_base64 = base64_decode(sen3);
	    string n3 = aes_256_cbc_decode(cbc_key1,niv3, n3_decode_base64);

        string niv4 = to_string(liv3);
        liv3++;
        string n4_decode_base64 = base64_decode(sen4);
	    string n4 = aes_256_cbc_decode(cbc_key1,niv4, n4_decode_base64);

        ocall_read_s(en5,25);
        ocall_read_s(en6,25);
        ocall_read_s(en7,25);
        ocall_read_s(en8,25);

        string sen5 = en5;
        string sen6 = en6;
        string sen7 = en7;
        string sen8 = en8;

        string niv5 = to_string(liv3);
        liv3++;
        string n5_decode_base64 = base64_decode(sen5);
	    string n5 = aes_256_cbc_decode(cbc_key1,niv5, n5_decode_base64);

        string niv6 = to_string(liv3);
        liv3++;
        string n6_decode_base64 = base64_decode(sen6);
	    string n6 = aes_256_cbc_decode(cbc_key1,niv6, n6_decode_base64);

        string niv7 = to_string(liv3);
        liv3++;
        string n7_decode_base64 = base64_decode(sen7);
	    string n7 = aes_256_cbc_decode(cbc_key1,niv7, n7_decode_base64);

        string niv8 = to_string(liv3);
        liv3++;
        string n8_decode_base64 = base64_decode(sen8);
	    string n8 = aes_256_cbc_decode(cbc_key1,niv8, n8_decode_base64);

        double sx1=atof(n1.c_str());
        double tx1=atof(n2.c_str());
        double sy1=atof(n3.c_str());
        double ty1=atof(n4.c_str());

        double sx2=atof(n5.c_str());
        double tx2=atof(n6.c_str());
        double sy2=atof(n7.c_str());
        double ty2=atof(n8.c_str());

        vector<int> de1;
        map<int,vector< vector<double> > > sdata1;


        for(auto i : result1)
        {
            
            int cc=0;
            for(auto h : storage2)
            {
                
                if(h.first == i)
                {
                    cc=1;
                    break;
                }  
            }
            if (cc == 1)
            {
                KDTree *tree1;
                tree1 = (KDTree*)(Def1->SearchData(i,(MBuf_des*)mbdes,mbpool));

                vector<vector<double> > searchdata1;
                vector<double> from1;
                vector<double> to1;
                from1.push_back(sx1);
                from1.push_back(sy1);
                to1.push_back(tx1);
                to1.push_back(ty1);
                searchdata1 = tree1->SearchByRegion(from1,to1);

                if(searchdata1.size() == 0)
                {
                    de1.push_back(i);
                }
                else
                {
                    sdata1[i] = searchdata1;
                }
                
                                
            }
        }

        vector<int> de2;
        map<int,vector< vector<double> > > sdata2;
        for(auto i : result2)
        {
           
            int cc=0;
            for(auto h : storage2)
            {
                
                if(h.second == i)
                {
                    cc=1;
                    break;
                }  
            }
            if (cc == 1)
            {
                KDTree *tree2;
                tree2 = (KDTree*)(Def2->SearchData(i+50000,(MBuf_des*)mbdes,mbpool));

                vector<vector<double> > searchdata2;
                vector<double> from2;
                vector<double> to2;
                from2.push_back(sx2);
                from2.push_back(sy2);
                to2.push_back(tx2);
                to2.push_back(ty2);
                searchdata2 = tree2->SearchByRegion(from2,to2);
                delete tree2;

                if(searchdata2.size() == 0)
                {
                    de2.push_back(i);
                }
                else
                {
                    sdata2[i] = searchdata2;
                }
            }
        }

        for(auto s : storage2)
        {
            int flag1 = 0;
            int flag2 = 0;
            for(auto d : de2)
            {
                if(s.second == d)
                {
                    flag1 = 1;
                    break;
                }
            }
            for(auto d : de1)
            {
                if(s.first == d)
                {
                    flag2 = 1;
                    break;
                }
            }
            if((flag1||flag2) == 0)
            {
                ocall_write_result1(s.first);
                for(auto p : sdata1[s.first])
                {
                    ocall_write_result1(p[0]);
                    ocall_write_result1(p[1]);
                }
                ocall_writeendl_result1();
            }


            if((flag1||flag2) == 0)
            {
                ocall_write_result2(s.second);
                for(auto p : sdata2[s.second])
                {
                    ocall_write_result2(p[0]);
                    ocall_write_result2(p[1]);
                }
                ocall_writeendl_result2();
            }
        }

    } 
    else
    {
        printf("type4:\n");

        ocall_read_s(es1,25);
        ocall_read_s(en1,25);
        ocall_read_s(en2,25);
        ocall_read_s(es2,25);
        ocall_read_s(en3,25);
        ocall_read_s(en4,25);

        string ses1 = es1;
        string sen1 = en1;
        string sen2 = en2;
        string ses2 = es2;
        string sen3 = en3;
        string sen4 = en4;
        
        string niv1 = to_string(liv4);
        liv4++;
        string s1_decode_base64 = base64_decode(ses1);
	    string s1 = aes_256_cbc_decode(cbc_key1,niv1, s1_decode_base64);

        string niv2 = to_string(liv4);
        liv4++;
        string n1_decode_base64 = base64_decode(sen1);
	    string n1 = aes_256_cbc_decode(cbc_key1,niv2, n1_decode_base64);

        string niv3 = to_string(liv4);
        liv4++;
        string n2_decode_base64 = base64_decode(sen2);
	    string n2 = aes_256_cbc_decode(cbc_key1,niv3, n2_decode_base64);

        string niv4 = to_string(liv4);
        liv4++;
        string s2_decode_base64 = base64_decode(ses2);
	    string s2 = aes_256_cbc_decode(cbc_key1,niv4, s2_decode_base64);

        string niv5 = to_string(liv4);
        liv4++;
        string n3_decode_base64 = base64_decode(sen3);
	    string n3 = aes_256_cbc_decode(cbc_key1,niv5, n3_decode_base64);

        string niv6 = to_string(liv4);
        liv4++;
        string n4_decode_base64 = base64_decode(sen4);
	    string n4 = aes_256_cbc_decode(cbc_key1,niv6, n4_decode_base64);

        double d1=atof(n1.c_str());
        double d2=atof(n2.c_str());
        double d3=atof(n3.c_str());
        double d4=atof(n4.c_str());


        for(auto i : result1)
        {
            
            KDTree *tree1;
            tree1 = (KDTree*)(Def1->SearchData(i,(MBuf_des*)mbdes,mbpool));

            int ff = 0;//判断是否为第一次而输出i

            for(auto j : result2)
            {
                
                vector<pair<int,int>>::iterator it;
                it = find(storage2.begin(), storage2.end(), pair<int,int>(i,j));
                if (it != storage2.end())
                {
                    KDTree *tree2;
                    tree2 = (KDTree*)(Def2->SearchData(i+50000,(MBuf_des*)mbdes,mbpool));

                    vector<vector<double> > searchdata2;
                
                    for (int h = 0; h < tree2->m_points.size(); ++h)
                    {

                        double x2 = tree2->m_points[h][0];
                        double y2 = tree2->m_points[h][1];

                        double sx=0;
                        double tx=0;
                        double sy=0;
                        double ty=0;
                        if(s1 == "+")
                        {
                            if(s2 == "+")
                            {
                                sx = d1 - x2;
                                tx = d2 - x2;
                                sy = d3 - y2;
                                ty = d4 - y2;
                            }
                            else
                            {
                                sx = d1 - x2;
                                tx = d2 - x2;
                                sy = y2 - d4;
                                ty = y2 - d3;
                            } 
                        }
                        else
                        {
                            if(s2 == "+")
                            {
                                sx = x2 - d2;
                                tx = x2 - d1;
                                sy = d3 - y2;
                                ty = d4 - y2;
                            }
                            else
                            {
                                sx = x2 - d2;
                                tx = x2 - d1;
                                sy = y2 - d4;
                                ty = y2 - d3;
                            }

                        }
                
                        vector<vector<double> > searchdata1;
                        vector<double> from1;
                        vector<double> to1;
                        from1.push_back(sx);
                        from1.push_back(sy);
                        to1.push_back(tx);
                        to1.push_back(ty);
                        searchdata1 = tree1->SearchByRegion(from1,to1);
                      
                        if(searchdata1.size() == 0)
                        {
                            break;
                        }
                        else
                        {
                            searchdata2.push_back(tree2->m_points[h]);
                            if(ff == 0)
                            {
                                ocall_write_result1(i);
				                ff = 1;
                            }
                            else{
                                for(auto p : searchdata1)
                                {
                                    ocall_write_result1(p[0]);
                                    ocall_write_result1(p[1]);
                                }
                                
                            }
                        }
                    }

                    if(searchdata2.size() != 0)
                    {
                        ocall_write_result2(j);
                        for(auto p : searchdata2)
                        {
                            ocall_write_result2(p[0]);
                            ocall_write_result2(p[1]);
                        }
                        ocall_writeendl_result2();
                        break;
                    }

                }
            }
            if(ff == 1)
            	ocall_writeendl_result1();
        }


    }

    ocall_close_enquery();
    ocall_close_result1();
    ocall_close_result2();
}
