import os
from typing import List

def clean(clean_words:List[str], file_in:str, file_out:str):
    with open(file_in,'r') as f_in, open(file_out,'w') as f_out:
        for line in f_in:
            if not any(word in line for word in clean_words):
                f_out.write(line)

if __name__ == '__main__':
    file_list = ['ra2ob.cpp', 'ra2ob.hpp']
    clean_list = [
        'spdlog', 
        'logger', 
        'logFile', 
        'max_size', 
        'max_files',
    ]
    new_folder = './nologger'

    for f in file_list:
        if not os.path.exists(new_folder):
            os.mkdir(new_folder)
        clean(
            clean_words=clean_list,
            file_in=f,
            file_out=os.path.join(new_folder, f)
        )