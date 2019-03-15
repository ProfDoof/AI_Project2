import random

def main():
    # create key
    key = {}
    alphabet = [chr(i) for i in range(65,91)]
    ints = [i for i in range(65,91)]
    random.shuffle(ints)

    for i in range(len(alphabet)):
        key[alphabet[i]] = ints[i]

    # accept input
    string = (input("Enter a string to encode: ")).upper().replace(" ","")

    # encode string
    encoded = ""
    for char in string:
        encoded += chr(key[char])

    # output results
    print("Input:  " + string)
    print("Output: " + encoded)

main()
