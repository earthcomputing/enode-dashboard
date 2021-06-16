#!/usr/bin/python -u
#---------------------------------------------------------------------------------------------
 #  Copyright © 2016-present Earth Computing Corporation. All rights reserved.
 #  Licensed under the MIT License. See LICENSE.txt in the project root for license information.
#---------------------------------------------------------------------------------------------
import sys, os
import re
import subprocess


def main():
    global linkUp
    unbuffered = os.fdopen(sys.stdin.fileno(), 'r', 0)
    sys.stdin = unbuffered

    for line in sys.stdin:
        print line
        entl_f = re.search('false', line)
        
        print entl_f
        if entl_f is not None:
            os.system("curl -H \"Content-Type: application/json\" -X PUT --data \"{'linkState' : 'link-down'}\"  http://localhost:3000/earthDown > /dev/null")
            linkUp = False
            print linkUp
        elif entl_f is None:
            os.system("curl -H \"Content-Type: application/json\" -X PUT --data \"{'linkState' : 'link-up'}\" http://localhost:3000/earthUp > /dev/null")
            linkUp = True
            print linkUp

    return True

main()
        
    

