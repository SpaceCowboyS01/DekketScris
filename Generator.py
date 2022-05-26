#!/usr/bin/python3
from string import printable
from random import choice

alf = printable[:-5]

def main():
    for i in range(1, 110, 10):
        with open('input_'+str(i)+'.txt', 'w') as f:
            f.write(''.join([choice(alf) for _ in range(i)]))

if __name__ == '__main__':
    main()
