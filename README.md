# cs3013-project3

# Installation
## Semaphore
## Mutexes & condition variables
  If you haven't compiled the program, compile with following commands:

    cd mutex
    make

# Usage
## Semaphore
### Mutexes & condition variables

  To run the program, using following commands:

    ./mutex_cv numorders shiptime cooktime

  **numorders**: number of orders

  **shiptime** (in nanoseconds): time interval for order to arrive

  **cooktime** (in nanoseconds): time interval for stages

  To abort the program, type in **q** and hit **Enter**.

# Test inputs
### Mutexes & condition variables
  - Order arrives much faster than processing speed

        ./mutex_cv 200 1000 10000

  - Order arrives much slower than the processing speed

        ./mutex_cv 200 100000 100

  - Order arrives roughly the same as the processing speed

        ./mutex_cv 200 10000 1000

# Authors
# License
