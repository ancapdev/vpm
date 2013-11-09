#!/usr/bin/python

import glob
import os
import subprocess
import sys

def usage():
    print "Usage:"
    print "\t", __file__, " <directory list or glob expression>"
    print "Example:"
    print "\t", __file__, " foo/trunk bar/*"

def main(argv):
    try:
        if len(argv) == 2 and argv[1] == "--help":
            usage()
            sys.exit(0)
        
        procs = []

        for arg in argv[1:]:
            dirs = glob.glob(arg)
            for d in dirs:

                # bzr repo handling
                if os.path.isdir(d) and os.path.isdir(os.path.join(d, ".bzr")):
                    print "Updating bzr repo", d
                    procs.append(
                        (d, subprocess.Popen(
                                ["bzr", "update", "--quiet", d],
                                0, # bufsize
                                None, # executable
                                None, # stdin
                                subprocess.PIPE, # stdout
                                subprocess.PIPE))) # stderr

                # git repo handling
                elif os.path.isdir(d) and os.path.isdir(os.path.join(d, ".git")):
                    print "Updating git repo", d
                    procs.append(
                        (d, subprocess.Popen(
                                ["git", "pull", "--ff-only", "--quiet"],
                                0, # bufsize
                                None, # executable
                                None, # stdin
                                subprocess.PIPE, # stdout
                                subprocess.PIPE, # stderr
                                cwd=d)))


        for (d, p) in procs:
            (output, errors) = p.communicate()
            print "Updated", d
            if len(output) > 0:
                for l in output.splitlines():
                    print "..", l
            if len(errors) > 0:
                for l in errors.splitlines():
                    print "..", l

    except Exception, error:
        print str(error)
        sys.exit(1)
                
if __name__ == "__main__":
    main(sys.argv)
