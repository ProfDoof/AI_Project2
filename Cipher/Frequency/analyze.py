def main():
    _dict ={}
    
    file = open("Frequency_2.txt", "r")
    for line in file:

        line = line.split()

        if line[0][0] == line[0][1]:
            if line[0] not in _dict:
                _dict[line[0]] = line[1]

    file.close()

    rev_dict = {}
    i = 0
    for k, v in _dict.items():
        print(k, v)
        i += 1
    print(i)
            


main()
