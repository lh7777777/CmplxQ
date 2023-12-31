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


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <pwd.h>
#define MAX_PATH FILENAME_MAX

#include "mysql.h"
#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"
#include "Ds_mgr.h"

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <time.h> 
#include <random>
#include <set>
#include <queue>

using namespace std;
using std::make_pair;
using std::pair;
using std::set;
using std::stoi;
using std::vector;

extern "C"
{
    my_bool range_join_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
    long long range_join(UDF_INIT *initid, UDF_ARGS *args,  char *is_null, char *err);
    void range_join_deinit(UDF_INIT* initid);
}

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(void)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    
    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }

    return 0;
}

extern const int NUM_PAGE_TOTAL;
extern const int PAGE_SIZE;

DataStorageMgr* ds_mgr;

void ocall_writepage(int page_id, char* buffer_field)
{
    ds_mgr->WritePage(page_id,buffer_field);

}

void ocall_readpage(int page_id,char* buffer_field)
{
    ds_mgr->ReadPage(page_id,buffer_field);
}


auto ha = [](const char *str) -> void {
    printf("%s\n", str);
};


void ds_init() {
    
    DataStorageMgr init_dsmgr = DataStorageMgr(true);
    char tmpbuffer[4096];

    for (int i = 0; i < NUM_PAGE_TOTAL; i++) {
        memset(tmpbuffer, 0, sizeof(tmpbuffer));
        for (int j = 0; j < PAGE_SIZE; j++)
            tmpbuffer[j] = 'a' + (j % 26);
        init_dsmgr.WriteNewPage(tmpbuffer);
    }
}

void RangeSearch()
{
    ds_mgr = new DataStorageMgr(false);
    ecall_kd_init(global_eid);

    MBuf_des mbuf_des[MBUFFER_DATA_NUM_MAX_SIZE];
    memset(mbuf_des, -1, sizeof(mbuf_des));
    char** mbuf_pool = new char*[6144];
    for(int i=0;i<6144;i++)
    {
        mbuf_pool[i] = new char[4096];
    }

    char** ein1 = new char*[bfnlen+1];
    char** ein2 = new char*[bfnlen+1];
    char** ekd1 = new char*[5*bfnlen+1];
    char** ekd2 = new char*[5*bfnlen+1];
    char* ibf1 = new char[bfmlen+1];
    char* ibf2 = new char[bfmlen+1];

    ecall_rangesearch(global_eid,ein1,ein2,ekd1,ekd2,ibf1,ibf2,mbuf_des,mbuf_pool);
}

my_bool range_join_init(UDF_INIT *initid, UDF_ARGS *args, char *message){

    return 0;
}

long long range_join(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *err)
{
    if(initialize_enclave() < 0)
    {
        return -1;
    }
    ds_init();
    RangeSearch();
    sgx_destroy_enclave(global_eid);
    return 1;
}

void range_join_deinit(UDF_INIT* initid){
}
