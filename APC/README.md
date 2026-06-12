# Arbitrary Precision Calculator

## Build

```sh
make
```

Or compile directly:

```sh
gcc -Wall -Wextra -std=c99 main.c helpers.c addition.c subtraction.c multiplication.c division.c -o apc
```

## Run

```sh
./apc <number1> <operator> <number2>
```

Supported operators:

```text
+  -  '*'  /  %  ^
```

Examples:

```sh
./apc 762345678911232344455522 + 119835485936279949364
./apc 762345678911232344455522 / 119835485936279949364
./apc 123456 ^ 123
```

Division prints up to 20 decimal places and trims unnecessary trailing zeroes.
The APC (Arbitrary Precision Calculator) project is a C-based application designed to perform arithmetic operations on very large numbers that cannot be handled by standard data types like int or long long. The project uses a doubly linked list to store numbers in chunks of 4 digits using a base value of 10000, which improves efficiency and reduces memory usage. It supports operations such as addition, subtraction, multiplication, division, modulus, and power through separate modular source files. The program accepts command line arguments, converts input numbers into linked lists, performs arithmetic operations using carry and borrow logic similar to manual calculations, and prints the final result accurately. The project demonstrates important concepts such as dynamic memory allocation, linked lists, string manipulation, large number arithmetic, modular programming, and command line processing in C.
