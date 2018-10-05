#!/usr/bin/env python
import re
import subprocess
import hashlib
import os
from os import path
import time

delete_old = True
add_hash = True

# OSX
# CPP_FILE = '/Users/javl/Downloads/of_v20180313_osx_release/apps/magicmiror/example-simple/src/ThreadedObject.h'
# AUDIO_PATH = '/Users/javl/Downloads/of_v20180313_osx_release/apps/magicmiror/example-simple/bin/data/audio'

# LINUX
# CPP_FILE = '/home/javl/Documents/of_v0.10.0_linux64gcc6_release/apps/magicmirror/magicmirror/src/ofApp.cpp'
# BACKUP_FILE = '/home/javl/Documents/of_v0.10.0_linux64gcc6_release/apps/magicmirror/magicmirror/src_backup/ofApp.cpp'
# AUDIO_PATH = '/home/javl/Documents/of_v0.10.0_linux64gcc6_release/apps/magicmirror/magicmirror/bin/data/audio'
# CPP_FILE = '/home/javl/Documents/of_v0.10.0_linux64gcc6_release/apps/magicmirror/face-the-interface/src/ofApp.cpp'
# CPP_FILE = '/home/javl/Documents/of_v0.10.0_linux64gcc6_release/apps/magicmirror/face-the-interface/convert.cpp'

CPP_FILE = '/Users/face/Documents/of_v0.10.0_osx_release/apps/magicmirror/face-the-interface/src/ofApp.cpp'

# BACKUP_FILE = '/home/javl/Documents/of_v0.10.0_linux64gcc6_release/apps/magicmirror/face-the-interface/src_backup/ofApp.cpp'
AUDIO_PATH = '/Users/face/Documents/of_v0.10.0_osx_release/apps/magicmirror/face-the-interface/bin/data/audio'

with open(CPP_FILE) as f:
    original_lines = f.read().splitlines()

# fo = open("{}.bk.{}".format(BACKUP_FILE, int(time.time())), "w+")
# fo.write("\n".join(original_lines))
# fo.close()

counter = 1
found_lines = []

# Go through the file and remove all playAudio("filename") lines
# by only copying lines that are NOT playAudio() lines)
use_lines = []
for i, line in enumerate(original_lines):
    searchObj = re.search(r'\s+playAudio\((.*)\);', line, re.M|re.I)
    # searchObj = re.search(r'\s+if\(\!playAudio\((.*)\)\{return;\};', line, re.M|re.I)
    if searchObj is None:
        use_lines.append(line)

# Go through the file and find all things to say. This first loop is just to get
# the total amount of lines we will have to process
total_lines = 0
for i, line in enumerate(use_lines):
    searchObj = re.search(r'(.*)//say (.*)', line, re.M|re.I)
    if searchObj is not None:
        total_lines += 1
print "found {} lines with text".format(total_lines)

used_files = []

feedback_index = 0

substate = 0
for i, line in enumerate(use_lines):

    # search for the 'case' in our draw loop
    searchObj = re.search(r'(.*)case (.*):', line, re.M|re.I)
    if searchObj is not None:
        print "caseline: ", line
        # found a new case, reset substate
        substate = 0

    regex = r'(.*)if \(substate == ([0-9]*)(.+)'
    searchObj = re.search(regex, line, re.M|re.I)
    if searchObj is not None:
        new_line = re.sub(regex, r"\1if (substate == {}\3".format(substate), line, re.M|re.I)
        use_lines[i] = new_line
        substate += 1

    searchObj = re.search(r'(.*)//say (.*)', line, re.M|re.I)
    if searchObj is not None:
        feedback_index+=1
        print "Generating {} of {}".format(feedback_index, total_lines)

        whitespace = searchObj.group(1)
        text = searchObj.group(2)
        hash = hashlib.md5(text).hexdigest()[:8]
        # filename = "{}.mp3".format(str(i+1).zfill(3))
        filename = re.sub('[^0-9a-zA-Z ]+', '', text.lower())
        filename = re.sub('\s+', '-', filename)
        if add_hash:
            filename = "{}_{}.mp3".format(filename[:64], hash)
        else:
            filename = "{}.mp3".format(filename[:64])

        used_files.append(filename)

        filepath = path.join(AUDIO_PATH, filename)
        if not path.isfile(filepath):
            # command = '/usr/local/bin/gtts-cli "{}" --lang nl-nl --nocheck --output "{}"'.format(text, filepath)
            command = 'say -v Claire "{}" -o "{}"'.format(text, filepath.replace('.mp3', '.aiff'))	
            subprocess.call([command], shell=True)
            
            command = 'lame -f {} {}'.format(filepath.replace('.mp3', '.aiff'), filepath)	
            subprocess.call([command], shell=True)
        use_lines[i] += "\n{}playAudio(\"{}\");".format(whitespace, filename, whitespace)

if delete_old:
    print "Deleting unused audio files"
    for filename in os.listdir(AUDIO_PATH):
        if filename.endswith(".aiff") or (filename.endswith(".mp3") and filename not in used_files):
                print "removing {}".format(filename)
                os.remove(path.join(AUDIO_PATH, filename))
        # print(os.path.join(directory, filename))

fo = open(CPP_FILE, "w")
fo.write("\n".join(use_lines))
fo.close()
