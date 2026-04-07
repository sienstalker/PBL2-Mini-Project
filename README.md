# PBL2-Mini-Project
A Repository which contains all files pertaining to the source code for the pbl2 mini project SYBtech CSE(2026)

Synopsis:
The main logic is to compile the C++ code. The resulting executable recieves upto 3 command line arguments.
If the executable is run with 3 command line arguments the first is always the string knight written without quotations followed by the dimensions of the chess board.
eg
./exe knight 6 6. Generates all possible ways for a knight to traverse a 6x6 board's square exactly once
Omission of the keyword knight and simply specifying the dimensions of the cheess board
eg: ./exe 6 6. Generates the Nqueens puzzle and all possible combinations for the size of the chess board (here 6x6)

**Note: The knight tour algorithm which relies on backtracking and recurrsion is a very complex and time consuming algorithm. Therefore chessboard sizes greater than 6x6 are not recommended to be tried. 6x6 Requires approx. 2.2 minutes to execute.** 

Working: The code will run the algorithms and write the details to a json file. Index.html acts as the website which will display the contents of the json file i.e the solutions in a web interface(GUI/Frontend).

**How to Compile and Run**
1) Command to compile:$ g++ -O2 Nqueens.cpp -std=c++17 -o <name of executable> //Note -O3 can be used for aggresive optimization(Not recommended)
2) Command to run:$ ./<exe name> Knight(omit for Nqueens) <no of rows> <no of columns>

Finally
To display output use python3 to serve the output of the code on a website

$ python3 -m http.server 8080

Then type http://localhost:8080 as url on web browser
