#include "Ocall_implements.h"
#include "Enclave_u.h"

long ocall_sgx_clock(void)
{
	struct timespec tstart={0,0};
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	return tstart.tv_sec * 1000000 + tstart.tv_nsec/1000; // Return micro seconds
}

time_t ocall_sgx_time(time_t *timep, int t_len)
{
	return 	time(timep);
}

struct tm *ocall_sgx_localtime(const time_t *timep, int t_len)
{
	return localtime(timep);
}

struct tm *ocall_sgx_gmtime_r(const time_t *timep, int t_len, struct tm *tmp, int tmp_len)
{
	return gmtime_r(timep, tmp);
}

int ocall_sgx_gettimeofday(void *tv, int tv_size)
{
	return gettimeofday((struct timeval *)tv, NULL);
}

int ocall_sgx_getsockopt(int s, int level, int optname, char *optval, int optval_len, int* optlen)
{
	return getsockopt(s, level, optname, optval, (socklen_t *)optlen);
}

int ocall_sgx_setsockopt(int s, int level, int optname, const void *optval, int optlen)
{
	return setsockopt(s, level, optname, optval, optlen);
}

int ocall_sgx_socket(int af, int type, int protocol)
{
	int retv;
	retv = socket(af, type, protocol);
	return retv;
}

int ocall_sgx_bind(int s, const void *addr, int addr_size)
{
	return bind(s, (struct sockaddr *)addr, addr_size);
}

int ocall_sgx_listen(int s, int backlog)
{
	return listen(s, backlog);
}

int ocall_sgx_connect(int s, const void *addr, int addrlen)
{
	int retv = connect(s, (struct sockaddr *)addr, addrlen);
	return retv;
}

int ocall_sgx_accept(int s, void *addr, int addr_size, int *addrlen)
{
	return accept(s, (struct sockaddr *)addr, (socklen_t *)addrlen);
}

int ocall_sgx_shutdown(int fd, int how)
{
	return shutdown(fd, how);
}

int ocall_sgx_read(int fd, void *buf, int n)
{
	return read(fd, buf, n);
}

int ocall_sgx_write(int fd, const void *buf, int n)
{
	return write(fd, buf, n);
}

int ocall_sgx_close(int fd)
{
	return close(fd);
}

int ocall_sgx_getenv(const char *env, int envlen, char *ret_str,int ret_len)
{
	const char *env_val = getenv(env);
	if(env_val == NULL){
		return -1;
	}
	memcpy(ret_str, env_val, strlen(env_val)+1);
	return 0;
}

void ocall_print_string(const char *str)
{
	printf("%s", str);
}

void ocall_sgx_exit(int e)
{
	exit(e);
}

int ocall_read_eneq1(char** eneq)
{
    ifstream eneq1("/var/lib/mysql/file/eneq1.txt");
    if(eneq1.fail())
        cout<<"FAIL!"<<endl;
    int i = 0;
    while(!eneq1.eof())
    {
        string v;
        eneq[i]=(char*)malloc(25);
        eneq1 >> v;
        memcpy(eneq[i],v.c_str(),25);
        i++;
    }
    eneq1.close();
    return i;
}

int ocall_read_eneq2(char** eneq)
{
    ifstream eneq2("/var/lib/mysql/file/eneq2.txt");
    int i = 0;
    while(!eneq2.eof())
    {
        string v;
        eneq[i]=(char*)malloc(25);
        eneq2 >> v;
        memcpy(eneq[i],v.c_str(),25);
        i++;
    }
    eneq2.close();
    return i;
}

ofstream re1;
ofstream re2;
ifstream enquery;
void ocall_open_result1()
{
    re1.open("/var/lib/mysql/file/result1.txt");
}
void ocall_open_result2()
{
    re2.open("/var/lib/mysql/file/result2.txt");
}
void ocall_write_result1(double data)
{
    re1 << data << " ";
}
void ocall_write_result2(double data)
{
    re2 << data << " ";
}
void ocall_writeendl_result1()
{
    re1 << endl;
}
void ocall_writeendl_result2()
{
    re2 << endl;
}
void ocall_close_result1()
{
    re1.close();
}
void ocall_close_result2()
{
    re2.close();
}

void ocall_open_enquery()
{
    enquery.open("/var/lib/mysql/file/enquery.txt");
}

void ocall_read_s(char* s,int n)
{
    enquery >> s;
}

void ocall_close_enquery()
{
    enquery.close();
}


int ocall_read_enkd1(int* cn,char** enkd)
{
    ifstream enkd1("/var/lib/mysql/file/enkd1.txt");
    int t=0;
    int j=0;
    string line1;
    while (getline(enkd1, line1)){
            int len=0;
            stringstream ss(line1);
            string v;
            while(ss >> v)
            {
                enkd[j]=(char*)malloc(25);
                memcpy(enkd[j],v.c_str(),25);
                j++;
                len++;
            }
            cn[t] = len;
            t++;
    }
    enkd1.close();
    return t;
}

int ocall_read_enkd2(int* cn,char** enkd)
{
    ifstream enkd2("/var/lib/mysql/file/enkd2.txt");
    int t=0;
    int j=0;
    string line2;
    while (getline(enkd2, line2)){

            stringstream ss(line2);
            int len=0;
            string v;
            while(ss >> v)
            {
                enkd[j]=(char*)malloc(25);
                memcpy(enkd[j],v.c_str(),25);
                j++;
                len++;                
            }
            cn[t] = len;
            t++;
    }
    enkd2.close();
    return t;
}

void ocall_read_ibf1(char* ibf)
{
    ifstream ibf1("/var/lib/mysql/file/ibf1.txt");
    if(ibf1.fail())
        cout<<"FAIL!"<<endl;

    ibf1 >> ibf;
    ibf1.close();
}
void ocall_read_ibf2(char* ibf)
{
    ifstream ibf2("/var/lib/mysql/file/ibf2.txt");
    if(ibf2.fail())
        cout<<"FAIL!"<<endl;

    ibf2 >> ibf;
    ibf2.close();
}
