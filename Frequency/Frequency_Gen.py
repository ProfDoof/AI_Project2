import string

files = ["walden.txt", "pride_and_prejudice.txt"]

def write_file(filename, _dict):
    file = open(filename, "w")
    for k, v in _dict.items():
        file.write(k + " " + str(v) + "\n")

    file.close()

def main():

    freq_1 = {}
    freq_2 = {}
    freq_3 = {}

    for filename in files:

        # open file
        file = open(filename, "r", encoding = "utf-8")

        # strip all nonalphabetic characters from string
        file_contents = "".join([char.upper() for char in "".join((file.read()).split()) if char.isalpha()])
        file.close()

        # generate sub-strings of sizes (1,2,3)
        sub_strings = list(file_contents[i:k+1] for i in range(len(file_contents)) for k in range(i,len(file_contents)) if k - i < 3)
        
        # iterate through sub-strings and add frequencies to dictionaries
        for sub_str in sub_strings:
            
            if len(sub_str) == 1:
                if sub_str not in freq_1:
                    freq_1[sub_str] = 1
                else:
                    freq_1[sub_str] += 1 
      
            elif len(sub_str) == 2:
                    if sub_str not in freq_2:
                        freq_2[sub_str] = 1
                    else:
                        freq_2[sub_str] += 1

            elif len(sub_str) == 3:
                if sub_str not in freq_3:
                    freq_3[sub_str] = 1
                else:
                    freq_3[sub_str] += 1
        
        write_file("Frequency_1.txt", freq_1)
        write_file("Frequency_2.txt", freq_2)
        write_file("Frequency_3.txt", freq_3)

        break
        
main()