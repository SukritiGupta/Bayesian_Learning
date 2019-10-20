rm startup.out
rm out.txt
# rm solved_alarm.bif
time g++ -O3 startup_code.cpp -o startup.out
time ./startup.out alarm.bif records.dat >> out.txt
