#!/usr/bin/awk -nf
#
# Process gcc binutils 'size -A' output to show more specific and summarized information about memory segment usage.
#
# This script expects some variables to be set on the command line with -v:
#  names : list of segment names ("Flash", "RAM" etc.)
#  addrs : list of segment start addresses in hex/oct/dec (in same order as names)
#  sizes : list of segment sizes in KB (in same order as names)
#  elf   : name of elf file (optional, for display only)
#  mcu   : name of MCU (optional, for display only)
#  skip_data : skip duplicate .data in Flash (0/1 default: 0) 
#
# NOTE: The memory segments should be specified in increasing address order (low to high).
#
# Usage example:
# arm-none-eabi-size -A -x firmware.elf | ./elf-size-report.awk                \
#                                            -v elf="firmware.elf"             \
#                                            -v mcu="STM32F401VG"              \
#                                            -v names="Flash RAM"              \
#                                            -v addrs "0x08000000 0x100000000" \
#                                            -v sizes "512 128"
#
# License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
# Copyright (c)2016 Maxim Paperno
# Copyright (c)2017 OpenTX
#


BEGIN {
  err = 0;
  len = split(names, a_names, " ");
  alen = split(addrs, a_addrs, " ");
  slen = split(sizes, a_sizes, " ");
  if (!len || len != alen || len != slen) {
    print "Error: No segment data provided or array sizes mismatch, exiting.";
    exit err = 1;
  }
  flash_name = "Flash";
  for (i in a_names) {
    n = a_names[i];
    a[i] = n; d[n]["addr"] = a_addrs[i]; d[n]["size"] = a_sizes[i]; d[n]["sum"] = 0;
    if (tolower(n) ~ /flash/)
      flash_name = n;
  }
  flmax = d[flash_name]["addr"];
  fmtstr = "%-20s %7d   0x%08x   %s\n";
  
  printf("\n---- Size report");
  if (length(elf)) printf(" for %s", elf);
  if (length(mcu)) printf(" (MCU: %s)", mcu);
  printf(" ----\n\n");
  printf("Section Details:\n");
  printf("%-20s %7s   %10s   %s\n", "section", "size(B)", "addr", "loc");
}
# Process each line of 'size' output listing the program's sections, 
#  skip line if sections size or address are zero.
NR > 2 && $2 != 0 && $3 != 0 {
  if ($1 == ".data") {
    # .data section is always in Flash but 'size' reports it only in RAM (sometimes twice)
    d[flash_name]["sum"] += $2;
    printf(fmtstr, $1 " (added)", $2, flmax, flash_name); 
    flmax += $3;
    if (skip_data) { next; }
  }
  # Walk over each specified memory segment looking for a match 
  #  note: (this assumes segments were passed in ascending memory order)
  PROCINFO["sorted_in"] = "@ind_num_desc"
  addr = d[a[len]]["addr"];
  for (i in a) {
    n = a[i];
    if ($3 >= addr) {
      # check for overflow into contigious region
      t = d[n]["sum"] + $2 - (d[n]["size"] * 1024); 
      if (t > 0 && i < len && d[a[i+1]]["addr"] = d[n]["addr"] + d[n]["size"] + 1) {
        d[n]["sum"] += $2 - t; 
        d[a[i+1]]["sum"] += t;
        seg = n "+" a[i+1];
      }
      else {
        d[n]["sum"] += $2;
        seg = n;
      }
      printf(fmtstr, $1, $2, $3, seg);
      # track maximum flash address used
      if (n == flash_name)
        flmax += $2;
      break;
    }
    else if (i > 1)
      addr = d[a[i-1]]["addr"];
  }
}
END {
  if (err) exit err;
  fmtstr = "%7s: %8.2f  %6.2f%%  %6d\t%7.2f\n";
  printf("\nTotals   %8s    usage  of ttl\tKB free\n", "KB");
  PROCINFO["sorted_in"] = "@ind_num_asc"
  for (i in a) {
    n = a[i];
    if (d[n]["size"] > 0)
      printf(fmtstr, n, d[n]["sum"]/1024, d[n]["sum"]/(d[n]["size"]*1024)*100, d[n]["size"], d[n]["size"] - d[n]["sum"]/1024 );
  }
  printf("\n");
}

