# BAYESIAN LEARNING

### Problem Statement
We are given a Bayesian Network that models the inter-relationship between some diseases and observed symptoms. We have to learn parameters for the network based on health records. Unfortunately, as it happens in the real world, certain records have missing values. We need to do our best to compute the parameters for the network, so that it can be used for diagnosis later on.

### Input Format
1. The **bayesian network** is represented in .bif format. Details of the input format can be found [here](http://sites.poli.usp.br/p/fabio.cozman/Research/InterchangeFormat/index.html). The network worked with in this assignment can be found [here](http://www.cse.iitd.ac.in/~mausam/courses/csl333/spring2015/A4/alarm.bif).
2. To learn the unknown values, a data file is provided whose each line will be a patient record. All features will be listed in exactly the same order as in the .bif network and will be comma-separated. If a feature value is unknown, the special symbol “?” for it and there will be no more than 1 unknown value per row.
3. A format checker to check that the output file adheres to alarm.bif format.

### Running the code
1. To compile
```
./compile.sh
```

2. To run (outputs solved_alarm.bif)
```
./run.sh alarm.bif sample_data.dat
```

3. To check if the output format is according to the specifications and also computes total learning error (wrt gold_alarm.bif)
```
python Format_checker.py
```

