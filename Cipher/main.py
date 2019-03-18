import random


def read_file_freq(filename, size):
    i = 0
    freq = {}
    file = open(filename, "r")

    for line in file:
        line = line.split()
        freq[line[0]] = float(line[1])
        i += 1

        if i == size:
            break

    '''
    _sum = sum(freq.values())
    for key in freq.keys():
        freq[key] = freq[key] / _sum
    '''
    file.close()
    return freq

'''
RETURNS a list which contain the dicts (freq_1, freq_2, freq_3)
'''
def get_freq():
    freq = []
    freq.append(read_file_freq("Frequency/Frequency_1.txt", 26))
    freq.append(read_file_freq("Frequency/Frequency_2.txt", 200))
    freq.append(read_file_freq("Frequency/Frequency_3.txt", 200))

    return freq

def get_code(filename):
    file = open("Test_files/" + filename, "r")
    line = file.read()
    file.close()
    return line

class Chromosome:

    '''
    PARAMETERS
        freq : (list(dict)) the frequencies of strings of size (1,2,3)
        code : (string) the string we are trying to decode produces the translation
        key : (list) the "decoded" value of a character in code
        chode_chars : (list) all unique chars in code
    '''
    def __init__(self, freq, code, code_chars, key):
        self.key = key
        self.translation = self.get_trans(code, code_chars, key)
        self.fitness = self.calc_fit(freq, key)

    def __str__(self):
        string = ""
        return str(self.fitness) + " " + self.translation

    '''
    RETURN (string)
        uses key comparison with code_chars to "decode" the given string code
        returns the "decoded" string
    '''
    def get_trans(self, code, code_chars, key):
        trans = ""
        for i in range(len(code)):
            trans += key[code_chars.index(code[i])]

        return trans


    def calc_fit(self, freq, key):
        fit = 0

        # uni
        freq_1 = {}
        for char in key:
            freq_1[char] = self.translation.count(char)
        
        _sum = sum(freq_1.values())
        for char in freq_1:
            freq_1[char] /= _sum

        real_freq = 0
        for char in freq_1:
            real_freq += freq[0].get(char)

        # smaller is better
        uni_fit = real_freq / 10

        #print(uni_fit)
    

        # di
        freq_2 = {}
        two = list(self.translation[i:i+2] for i in range(len(self.translation)) if (i + 2) < len(self.translation))
        found = False
        di_fit = 0
        for sub_str in two:
            if sub_str in freq[1]:
                found = True
                di_fit += freq[1].get(sub_str)

        if not found:
            di_fit = 0

        #print(di_fit)

        # tri
        freq_3 = {}
        three = list(self.translation[i:i+3] for i in range(len(self.translation)) if (i + 3) < len(self.translation))
        found = False
        tri_fit = 0
        for sub_str in three:
            if sub_str in freq[2]:
                found = True
                tri_fit += freq[2].get(sub_str)

        if not found:
            tri_fit = 0
        tri_fit *= 10
        
        #print(tri_fit)

        return uni_fit + di_fit + tri_fit
    


def main():
    print("ENTER MAIN")

    gen = 0
    population_size = 10
    #filename = input("Enter a filename: ")

    # LOAD FREQUENCIES
    frequencies = get_freq()
   
    # LOAD TEST CASE
    #code = get_code(filename)
    code = "YSVXVXRYMXYQRXMVYVXFZYYSMFMXYYMXYQRXMAJRFJEMRFXAHYVYVXRYMXYQRXMFZFMYSMOMXXVLZFZYIFZDVNVYVXRGZZLZFM"

    code_chars = list(set(code)) # we might be able to keep this as a set

    key = random.sample("ABCDEFGHIJKLMNOPQRSTUVWXYZ", len(code_chars))

    # GENERATE INITIAL POPULATION (this is possilbe if i want to lug around less data)
    population = [Chromosome(frequencies, code, code_chars, random.sample("ABCDEFGHIJKLMNOPQRSTUVWXYZ", len(code_chars))) for i in range(population_size)]
    #Wprint(population)
    for elem in population:
        print(elem)
    chromo = Chromosome(frequencies, code, code_chars, key)
    print(chromo)
    print(chromo.fitness)
    print(chromo.translation)


    while gen < 10:
        # find current best

        # send those those with highest fit to new gen

        # send others to 



        gen += 1
    
    
    

    print("EXIT MAIN")



main()