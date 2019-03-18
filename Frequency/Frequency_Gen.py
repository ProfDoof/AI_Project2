files = ["walden.txt","david_copperfield.txt","pride_and_prejudice.txt", "shakespeare.txt"]

def main():

    freq_1 = {}
    freq_2 = {}
    freq_3 = {}

    for filename in files:

        # open file
        file = open(filename, "r", encoding="utf-8")

        # strip all nonalphabetic characters from string
        file_contents = "".join([char.upper() for char in "".join((file.read()).split()) if char in "abcdefghihjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"])
        file.close()
        
        # generate sub-strings of sizes (1,2,3)
        file_contents = list(file_contents[i:k+1] for i in range(len(file_contents)) for k in range(i, i + 3) if k < len(file_contents))

        # iterate through sub-strings and add frequencies to dictionaries
        for string in file_contents:
            if len(string) == 1:
                if string not in freq_1:
                    freq_1[string] = 1.0
                else:
                    freq_1[string] += 1.0

            elif len(string) == 2:
                if string not in freq_2:
                    freq_2[string] = 1.0
                else:
                    freq_2[string] += 1.0

            else:
                if string not in freq_3:
                    freq_3[string] = 1.0
                else:
                    freq_3[string] += 1.0
        
        print(filename, "complete")

    
    # write results to file
    file = open("Frequency.txt", "w")
    for k, v in sorted(freq_1.items(), reverse = True, key = lambda x : x[1]):
        file.write(k + " " + str(v / sum(freq_1.values())) + "\n")
    #file.close()

    #file = open("Frequency_.txt", "w")
    i = 0
    for k, v in sorted(freq_2.items(), reverse = True, key = lambda x : x[1]):
        file.write(k + " " + str(v / sum(freq_2.values())) + "\n")
        if i == 56:
            break
        i += 1
    #file.close()

    i = 0
    #file = open("Frequency_3.txt", "w")
    for k, v in sorted(freq_3.items(), reverse = True, key = lambda x : x[1]):
        file.write(k + " " + str(v / sum(freq_3.values())) + "\n")
        if i == 114:
            break
        i += 1
    #file.close()

    file.close()

main()
