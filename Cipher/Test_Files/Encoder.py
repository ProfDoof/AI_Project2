import random

def main():
    # create key
    key_0 = {}
    alphabet = [chr(i) for i in range(65,91)]
    ints = [i for i in range(65,91)]
    random.shuffle(ints)

    for i in range(len(alphabet)):
        key_0[alphabet[i]] = ints[i]

    # accept input
    filename = input("Enter a filename to store text in: ")
    string = (input("Enter a string to encode: ")).upper().replace(" ","")

    # encode string
    encoded = ""
    for char in string:
        encoded += chr(key_0[char])

    # have key only contian chars in text
    key = {}
    for k,v in key_0.items():
        if k in string:
            key[k] = v

    values = list(chr(char) for char in key.values())
    
    other_key = {}
    values.sort()
    for k, v in key.items():
        for char in values:
            if char == chr(v):
                other_key[char] = k




    # write results

    # meta file
    file = open(filename[:-4] + "_meta.txt", "w")
    file.write("Output: " + encoded + '\n')
    file.write("Input:  " + string + '\n')
    file.write("Key:\n")
    for char in key.keys():
        file.write(format(char, "2s") + ' ')
    file.write(' \n')
    for k in key.keys():
        file.write(format(chr(key[k]), "2s") + ' ')
    file.write(' \n')
    #for char in key.keys():
    #    file.write(format(str(string.count(char)), "2s") + " ")
    file.write('\n')
    file.write('\n')
    
    for k in values:
        file.write(format(k, "2s") + ' ')
    file.write('\n')
    i = 0
    for k in other_key.keys():
        file.write(format(other_key[values[i]], "2s") + ' ')
        i += 1
    file.close()
    
    
    # test file
    file = open(filename, "w")
    file.write(encoded)
    file.close()

    print(values)

main()
