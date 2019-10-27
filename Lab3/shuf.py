#!/usr/bin/env python3
import random, sys
import argparse

class shuf:
    def __init__(self, inp, trigger=0): # 0=file, 1=`-i' flag, 2=stdin
        if trigger == 1:
            self.lines = inp.split("-")
            try:
                a1 = int(self.lines[0])
                a2 = int(self.lines[1])
            except ValueError:
                sys.exit('shuf.py: invalid input range: \'' + inp +"\'")
            if a1 - a2== 1:
                sys.exit(0)
            if (len(self.lines) != 2) or (a1 > a2):
                sys.exit('shuf.py: invalid input range: \'' + inp +"\'")
        elif trigger == 2:
            self.lines = inp
        else:
            f = open(inp, 'r')
            self.lines = f.readlines()
            f.close()

    def chooselines(self, n, r, iflag):
        if iflag:
            a1 = int(self.lines[0])
            a2 = int(self.lines[1])
            res = [i for i in range(a1, a2 +1)]
        else:
            res = self.lines
        if n == None:
            if r:
                br = res.copy()
                res.clear()
                for b in br:
                    res.append(random.choice(br))
            else:
                random.shuffle(res)
        else:
            try:
                n = int(n)
            except ValueError:
                sys.exit('shuf.py: invalid line count: \'' + str(n)+"\'")
            if n < 0 or n > len(res):
                sys.exit('shuf.py: invalid line count: \'' + str(n)+"\'")
            if r:
                br = res.copy()
                res.clear()
                for i in range(0, n):
                    res.append(random.choice(br))
            else:
                res = random.sample(res, n)
        return res

def main():
    lin = sys.argv[1:]
    iFlag = False
    fromStdin = True
    flag = True
    lenL = len(lin)
    for i in range(0,lenL):
        if flag:
            if lin[i] == '-i':
                fromStdin = False
                break
            elif '-' in lin[i] and lin[i] != '-r' and lin[i] != '-' and lin[i] != '--':
                flag = False
            elif lin[i] != '-r' and lin[i] != '-' and lin[i] != '--':
                fromStdin = False
        else:
            flag = True

    usage_msg = """%(prog)s [OPTION]... FILE
       %(prog)s -i LO-HI [OPTION]..."""
    desc="Output randomly selected lines from FILE or range of numbers LO-HI."
    parser = argparse.ArgumentParser(usage=usage_msg, description=desc)
    
    if fromStdin: # input from stdin
        # Parse
        parser.add_argument("-n", "--head-count", dest="COUNT",
                            help="output at most COUNT lines")
        parser.add_argument("-r", "--repeat",
                            help="output lines can be repeated",
                            action="store_true", dest="REP")
        TOT = parser.parse_known_args()
        args=TOT[0]
        err=TOT[1]
        if len(err) > 1:
            if (err[0] != '-' and err[0] != '--'):
                sys.exit('shuf.py: error: unrecognized arguments')
        a = vars(args)
        for comp in a:
            if comp == 'COUNT':
                nfl = a[comp]
            elif comp == 'REP':
                rfl = a[comp]
        try:
            nfl = int(nfl)
        except ValueError:
            sys.exit('shuf.py: invalid line count: \'' + str(nfl)+"\'")
        if nfl < 0:
            sys.exit('shuf.py: invalid line count: \'' + str(nfl)+"\'")
        # Shuffle
        try:
            msg = sys.stdin.readlines()
        except:
            sys.exit(130)
        generator = shuf(msg, 2)
        result = generator.chooselines(nfl, rfl, iFlag)
        for r in result:
            sys.stdout.write(str(r))
        
    else: # input from file or `-i' flag
        # Pre-parse
        for i in range(0,len(lin)):
            if(lin[i]=='-i'):
                iFlag = True
                break
        # Parse
        if iFlag:
            parser.add_argument("-i", "--input-range", dest="LO-HI",
                                help="treat each number between LO, HI as an input line")
        else:
            parser.add_argument(dest="FILE")
        parser.add_argument("-n", "--head-count", dest="COUNT",
                            help="output at most COUNT lines")
        parser.add_argument("-r", "--repeat",
                            help="output lines can be repeated",
                            action="store_true", dest="REP")
        args = parser.parse_args()
        fl = ""
        ifl = ""
        rfl = False
        nfl = -1
        # Process
        a = vars(args)
        for comp in a:
            if comp == 'FILE':
                fl = a[comp]
                try:
                    generator = shuf(fl)
                except IOError as ioerr:
                    sys.exit('shuf.py: \''+fl+"\'"+": No such file or directory")
            elif comp == 'LO-HI':
                ifl = a[comp]
                generator = shuf(ifl, 1)
            elif comp == 'COUNT':
                nfl = a[comp]
            elif comp == 'REP':
                rfl = a[comp]
        result = generator.chooselines(nfl, rfl, iFlag)
        if iFlag:
            for r in result:
                sys.stdout.write(str(r)+"\n")
        else:
            for r in result:
                sys.stdout.write(str(r))
        
if __name__ == "__main__":
    main()
    
