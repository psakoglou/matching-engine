# Matching Engine 

This project is a mini version of a matching engine.

The binary receives input from stdin and prints matched trades
as well as the latest order book.

The input is received by a "Gateway" process, which checks it
for validity and creates a new order.

Then the Gateway forwards this new order to the matching engine,
which executes it. Depending on the input order, the matching engine
can execute the following:
BUY/SELL 
MODIFY
CANCEL
PRINT

More specifically, the matching engine can insert a new BUY/SELL order
to the order book for execution, can modify an existing order in the book 
(change a BUY to a SELL, or change price), can cancel an existing order,
and can print the latest order book.

The matching engine will check for trades every time an order is inserted
in the book or when an existing order is modified.


# Build

g++ -std=c++11 -O3 cpp/* include/* -o meng 


# Run

Bash: ./meng < input

Powershell: Get-Content input | ./meng


# Engine Input 

Only the following commands (in any combinations) with that specific
format are legal input. All other commands will be ignored by the engine

e.g.
SIDE ORDER_TYPE PRICE QUANTITY ORDER_ID
MODIFY ORDER_ID SIDE PRICE QUANTITY
CANCEL ORDER_ID
PRINT

where
SIDE: ["BUY", "SELL"]
ORDER_TYPE: ["IOC", "GFD"]
PRICE: [signed long long]
QUANTITY: [signed long long]
ORDER_ID: [string]

See test/*.in contents for input examples


# Rules

IOC orders cannot be canceled or modified. If they are not immidiately
executed they will be discarded.

A match happens when bid price >= ask price.

The book only holds unique ORDER_IDs. You can use an existing ORDER_ID
to cancel or modify, but not as a new BUY/SELL order

PRINT prints the latest order book