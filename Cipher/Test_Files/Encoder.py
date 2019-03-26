import random

def main():

    # accept input
    filename = input("Enter a filename to store text in: ")
    string = (input("Enter a string to encode: ")).upper().replace(" ","")


    alphabet = [chr(i) for i in range(65,91)]
    ints = [i for i in range(65,91)]
    random.shuffle(ints)

    key = {}
    for i in range(26):
        key[alphabet[i]] = chr(ints[i])

    # encode string
    encoded = ""
    for char in string:
        encoded += key[char]

    values = list(char for char in key.values())
    values.sort()
    
    master_key = {}
    for k, v in key.items():
        for char in values:
            if char == v:
                master_key[char] = k

    # meta file
    file = open(filename[:-4] + "_meta.txt", "w")
    file.write("Output: " + encoded + '\n')
    file.write("Input:  " + string + '\n')
    file.write("Key:\n")

    for k in values:
        file.write(format(k, "2s"))
    file.write('\n')
    i = 0
    for v in master_key.keys():
        file.write(format(master_key[values[i]], "2s"))
        i += 1
    file.close()
    
    
    # test file
    file = open(filename, "w")
    file.write(encoded)
    file.close()

main()
