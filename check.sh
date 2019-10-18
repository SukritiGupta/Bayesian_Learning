rm startup.out
rm out.txt
g++ -O3 startup_code.cpp -o startup.out
./startup.out >> out.txt
