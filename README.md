# CmplxQ

We have implemented the code for CmplxQ, including `\client` and `\sgx_buffer` and can be integrated into MySQL using UDF, with a testing environment of Ubuntu 16.04 and Intel SGX SDK 2.6.

## Compilation

### client

    $ make clean && make
    $ ./db

Then follow the instructions to enter the range join, for example:

1. load csv files:

    load a table1.csv
    load b table2.csv
   
2. buildindex:

    buildindex a table1_key.txt eneq1.txt enkd1.txt ibf1.txt on eqA raA1 raA2
    buildindex b table2_key.txt eneq2.txt enkd2.txt ibf1.txt on eqB raB1 raB2
   
3. rangejoin:

    rangejoin a eqA on b eqB and 1 on xmin xmax ymin ymax
    or
    rangejoin a eqA on b eqB and 2 on rmin rmax nmin nmax
    or
    rangejoin a eqA on b eqB and 3 on xmin xmax ymin ymax rmin rmax nmin nmax
    or
    rangejoin a eqA on b eqB and 4 on +/- pmin pmax and +/- qmin qmax
   
4. exit:

    exit

### sgx_buffer

    $ make clean && make
    $ make install

This will generate the `app.so` file into the MySQL related directory, and then use `CREATE JUNCTION ...` in MySQL to create and use UDF functions.
