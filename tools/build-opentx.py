#! /usr/bin/python
import cgitb
#cgitb.enable()


from pprint import pprint
from time import gmtime, strftime
from os.path import expanduser
import os
import os.path
import cgi
import sys
import re
import fcntl
import subprocess

gitdir="~/cgibuild/opentx"
pidfile="~/cgibuild/opentxbuild.pid"
outdir="~/Sites/builds/"
builddir="~/cgibuild/"

def main():
    print ("Content-Type: text/html\n")

    print ("""<!DOCTYPE html>
    <html>
    <body>
    <h1>OpenTX 2.2 Companion build...</h1>
    """)

    form = cgi.FieldStorage()
    suffix = None
    buildscript = "build-companion-release.sh"
    if 'suffix' in form:
        suffix = form.getfirst("suffix")
        if not re.match("^[a-zA-Z0-9]*$", suffix):
            status("Invalid suffix specified", True)
        if suffix and not suffix.startswith("rc"):
            buildscript = "build-companion-nightly.sh"

    if "branch" not in form or not re.match("^[a-z_\\-.A-Z0-9/]*$",form.getfirst("branch")):
        status ("No branch specified or invalid branch\n", True)

    branch = form.getfirst("branch")

    status ("Branch %s - Suffix %s" %(branch, suffix))
    status ("Trying to get lock")
    getlock_or_exit()

    logfile = open(os.path.join(expanduser(outdir),strftime("build-%Y-%m-%d %H:%M:%S.log", gmtime())),"w")
    run_cmd(["git", "fetch"], gitdir, logfile)
    run_cmd(["git", "checkout", "origin/%s" % branch], gitdir, logfile)
    run_cmd(["git", "reset", "--hard"], gitdir, logfile)

    buildcmd = [os.path.join(expanduser(gitdir), "tools", buildscript),  \
                    "-j4",
                    expanduser(gitdir), \
                    expanduser(outdir)]
    if suffix:
        buildcmd.append(suffix)


    run_cmd(buildcmd, builddir, logfile)
    status("Finished")

def run_cmd(cmd, wd, logfile):    
    env = os.environ.copy()
    env['PATH'] = env['PATH'] + ":/usr/local/bin/"
    env['PYTHONPATH']="/usr/local/lib/python2.7/site-packages"
    env['HOME']=expanduser("~")


    status("Running '[%s]'" % ", ".join(cmd))
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd=expanduser(wd),env=env, bufsize=00)

    print("<pre>\n")
    sys.stdout.flush()

    for line in p.stdout:
        sys.stdout.write(line)
        logfile.write(line)
        if '\n' in line:
            sys.stdout.flush()

    print("</pre>\n")
    p.wait()
    if(p.returncode != 0):
        status( "failed with status %d" % p.returncode, True)


def getlock_or_exit():
    global fp
    pid_file = expanduser(pidfile)
    fp = open(pid_file, 'w')
    try:
        fcntl.lockf(fp, fcntl.LOCK_EX | fcntl.LOCK_NB)
        fp.write("%s" % (os.getpid()))
    except IOError as e:
        # another instance is running
            
        status("could get lock, another instance running? %s " % str(e), True)

def status(msg, exit=False):
    print ("<p><b>%s - %s</b></p>\n" % (strftime("%Y-%m-%d %H:%M:%S", gmtime()),msg))
    if exit:
        sys.exit(0)

if __name__=="__main__":
    main()

