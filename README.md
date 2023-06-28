# CmplxQ

We have implemented the code for CmplxQ, including `\client` and `\sgx_buffer` and can be integrated into MySQL using UDF, with a testing environment of Ubuntu 16.04 and Intel SGX SDK 2.6.

## Compilation

### client

    $ make clean && make
    $ ./db

Then follow the instructions to enter the range join, for example:

    load a 1.csv
    load b 2.csv
    buildindex a table1_key.txt file/eneq1.txt file/enkd1.txt file/ibf1.txt on type1 id1 age1
    buildindex b table2_key.txt file/eneq2.txt file/enkd2.txt file/ibf1.txt on type2 id2 age2
    rangejoin a type1 on b type2 and 4 on + 0 20000 and + 0 200
    exit

### sgx_buffer

    $ make clean && make
    $ make install

This will generate the `app.so` file into the MySQL related directory, and then use `CREATE JUNCTION ...` in MySQL to create and use UDF functions.