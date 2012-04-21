#!/usr/bin/env python

import os, sys, shutil, platform, getpass, subprocess, zipfile, ftplib, httplib, threading

BINARY_DIR = "../build/"

options_stock = [[("", "EXT=STD"), ("frsky", "EXT=FRSKY"), ("jeti", "EXT=JETI"), ("ardupilot", "EXT=ARDUPILOT"), ("nmea", "EXT=NMEA")],
                 [("", "HELI=NO"), ("heli", "HELI=YES")],
                 [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
                 [("", "AUDIO=NO"), ("audio", "AUDIO=YES")],
                 [("", "HAPTIC=NO"), ("haptic", "HAPTIC=YES")],
                 [("", "DSM2=NO"), ("DSM2", "DSM2=PPM")],
                 [("", "NAVIGATION=NO"), ("potscroll", "NAVIGATION=POTS")],
                 [("", "PPM_CENTER_ADJUSTABLE=NO"), ("ppmca", "PPM_CENTER_ADJUSTABLE=YES")],
                 [("", "UNITS=METRIC"), ("imperial", "UNITS=IMPERIAL")],
#                 ("PXX", "PXX=NO", "PXX=YES"),
                ]

options_v4 =    [[("", "EXT=FRSKY")],
                 [("", "HELI=NO"), ("heli", "HELI=YES")],
                 [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
                 [("", "SDCARD=NO"), ("sdcard", "SDCARD=YES")],
                 [("", "SOMO=NO"), ("SOMO", "SOMO=YES")],
                 [("", "PPM_CENTER_ADJUSTABLE=NO"), ("ppmca", "PPM_CENTER_ADJUSTABLE=YES")],
                 [("", "UNITS=METRIC"), ("imperial", "UNITS=IMPERIAL")],
                ]

options_arm =   [[("", "EXT=FRSKY")],
                 [("", "HELI=NO"), ("heli", "HELI=YES")],
                 [("", "TEMPLATES=NO"), ("templates", "TEMPLATES=YES")],
                 [("", "UNITS=METRIC"), ("imperial", "UNITS=IMPERIAL")],
                ]

languages = ["en", "fr", "se"]

#ftp_host = "ftpperso.free.fr"
#ftp_user = "open9x"
ftp_host = "open9x.freehosting.com"
ftp_user = "openxfre"
ftp_directory = "public_html/binaries/"
            
def upload(binaries, ext, stamp):
    if not ftp:
        return
    
    print "Zip creation..."
    zip_name = "release.zip"
    zip = zipfile.ZipFile(zip_name, "w", zipfile.ZIP_DEFLATED)
    for bin in binaries:
        zip.write(BINARY_DIR + bin + "." + ext, bin + "." + ext)
    zip.write(BINARY_DIR + stamp, stamp)
    zip.close()
    
    print "FTP transfer..."
    ftp_connection = ftplib.FTP(ftp_host, ftp_user, ftp_password)
    try:
        ftp_connection.delete(ftp_directory + zip_name)
    except:
        pass
    f = file(zip_name, 'rb')
    ftp_connection.storbinary('STOR ' + ftp_directory + zip_name, f)
    f.close()
    ftp_connection.quit()
    
    print "ZIP extraction...",
    http_connection = httplib.HTTPConnection("open9x.freehosting.com")
    http_connection.request("GET", "/binaries/uncompress.php")
    response = http_connection.getresponse()
    print response.status, response.reason
    os.remove(zip_name)

global_current = 0
global_count = 0 
def generate(hex, arg, extension, options, languages, maxsize):
    global global_current
    global global_count
    
    result = []
    states = [0] * len(options)
    
    count = len(languages)
    for option in options:
      count *= len(option)
    global_count += count
    
    while 1:
        # print index, states
    
        for language in languages:
            hex_file = hex
            make_args = ["make", arg]
            for i, option in enumerate(options):
                state = states[len(options) - 1 - i]
                if option[state][0]:
                    hex_file += "-" + option[state][0]
                make_args.append(option[state][1])
            hex_file += "-" + language
            make_args.append("TRANSLATIONS=" + language.upper())
            if mt:
                cwd = language
            else:
                cwd = "."
            subprocess.Popen(["make", "clean", arg], stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd).wait()
            p = subprocess.Popen(make_args, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd)
            ret = p.wait()
            stderr = p.stderr.read()
            global_current += 1
            print "[%d/%d]" % (global_current, global_count), hex_file
            if ret or "error" in stderr:
                print stderr
                exit()
            for line in stderr.split("\n"):
                if "warning" in line:
                    print "  ", line
            size = 0
            for line in p.stdout.readlines():
                if line.startswith("Program:"):
                    parts = line.split(" ")
                    while "" in parts:
                        parts.remove("")
                    size = int(parts[1])
                    if size > maxsize:
                        print "  ", line[:-1], "[NOT RELEASED]"
                    else:
                        print "  ", line,
                if line.startswith("Data:"):
                    print "  ", line,
            
            if size <= maxsize:
                binary_name = hex_file + "." + extension
                shutil.copyfile(cwd + "/open9x." + extension, BINARY_DIR + binary_name)
                result.append(hex_file)
        
        for index, state in enumerate(states):
            if state < len(options[len(options) - 1 - index]) - 1:
                for i in range(index):
                    states[i] = 0
                states[index] += 1
                break
        else:
            break
    
    # stamp
    stamp = "stamp-open9x-%s.txt" % hex.split("-")[1]
    subprocess.Popen(["make", hex.split("-")[1] + "-stamp"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd="../src").wait()
    shutil.copyfile("../src/" + stamp, BINARY_DIR + stamp)
        
    return result, stamp

class GenerateThread(threading.Thread):
   def __init__ (self, hex, arg, extension, options, language, maxsize):
      threading.Thread.__init__(self)
      self.hex = hex
      self.arg = arg
      self.extension = extension
      self.options = options
      self.language = language
      self.maxsize = maxsize
      
   def run(self):
      self.hexes, self.stamp = generate(self.hex, self.arg, self.extension, self.options, [self.language], self.maxsize)     
           
def multithread_generate(hex, arg, extension, options, languages, maxsize):
    if mt:
        result = []
        threads = []
        for language in languages:
            thread = GenerateThread(hex, arg, extension, options, language, maxsize)
            threads.append(thread)
            thread.start()
        for thread in threads:
            thread.join()
            result.extend(thread.hexes)
            stamp = thread.stamp
        return result, stamp
    else:
        return generate(hex, arg, extension, options, languages, maxsize)
        
        
def generate_c9x_list(filename, hexes, extension, stamp, board):
    f = file(filename, "w")
    for hex in hexes:
        f.write('open9x->add_option(new Open9xFirmware("%s", new Open9xInterface(%s), OPEN9X_BIN_URL "%s.%s", %s));\n' % (hex, board, hex, extension, stamp))

if __name__ == "__main__":
    
    ftp = "ftp" in sys.argv
    mt = "mt" in sys.argv and platform.system() != "Windows"
    
    if ftp:
        ftp_password = getpass.getpass()
        
    if mt:
        shutil.rmtree("util", True)
        shutil.copytree("../util", "util")
        for lang in languages:
            print "Directory %s creation..." % lang
            shutil.rmtree(lang, True)
            shutil.copytree("../src", lang)
    
    if platform.system() == "Windows":
        # arm board
        hexes, stamp = generate("open9x-arm", "PCB=ARM", "bin", options_arm, languages, 262000)
        generate_c9x_list("../../companion9x/src/open9x-arm-binaries.cpp", hexes, "bin", "OPEN9X_ARM_STAMP", "BOARD_ERSKY9X")
        upload(hexes, "bin", stamp)        
    
    else:
        # stock board
        hexes, stamp = multithread_generate("open9x-stock", "PCB=STD", "hex", options_stock, languages, 65530)
        generate_c9x_list("../../companion9x/src/open9x-stock-binaries.cpp", hexes, "hex", "OPEN9X_STAMP", "BOARD_STOCK")
        upload(hexes, "hex", stamp)
    
        # v4 board
        hexes, stamp = multithread_generate("open9x-v4", "PCB=V4", "hex", options_v4, languages, 262000)
        generate_c9x_list("../../companion9x/src/open9x-v4-binaries.cpp", hexes, "hex", "OPEN9X_STAMP", "BOARD_GRUVIN9X")
        upload(hexes, "hex", stamp)
            
