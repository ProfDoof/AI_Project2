import string

files = ["walden.txt","david_copperfield.txt","pride_and_prejudice.txt","shakespeare.txt"]
# files = ["abc.txt"]

def main():

    freq_1 = {}
    freq_2 = {}
    freq_3 = {}
    tot1 = 0
    tot2 = 0
    tot3 = 0

    for filename in files:

        # open file
        file = open(filename, "r")

        # strip all nonalphabetic characters from string
        file_contents = "".join([char.upper() for char in "".join((file.read()).split()) if char.isalpha()])
        file.close()
        # print(file_contents)

        # generate sub-strings of sizes (1,2,3)
        # iterate through sub-strings and add frequencies to dictionaries
        for i in range( len(file_contents) ):
            tot1 += 1
            sub_str = file_contents[i]
            if sub_str not in freq_1:
                freq_1[sub_str] = 1.0
            else:
                freq_1[sub_str] += 1.0

            if i > 0:
                tot2 += 1
                sub_str = file_contents[i-1:i+1]
                if sub_str not in freq_2:
                    freq_2[sub_str] = 1.0
                else:
                    freq_2[sub_str] += 1.0

            if i > 1:
                tot3 += 1
                sub_str = file_contents[i-2:i+1]
                if sub_str not in freq_3:
                    freq_3[sub_str] = 1.0
                else:
                    freq_3[sub_str] += 1.0

        print(filename, "complete")


    file = open("Frequency.txt", "w")
    for k, v in freq_1.items():
        v_output = "%.9f" % (v/tot1)
        file.write(k + " " + v_output + "\n")

    for k, v in freq_2.items():
        v_output = "%.9f" % (v/tot2)
        file.write(k + " " + v_output + "\n")

    for k, v in freq_3.items():
        v_output = "%.9f" % (v/tot3)
        file.write(k + " " + v_output + "\n")

    file.close()

main()
