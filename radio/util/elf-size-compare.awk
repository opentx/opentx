#!/usr/bin/awk -nf
#
# Parses the output of elf-size-report.awk and calculates segment memory usage deltas between the analyzed elf files.
#
# License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
# Copyright (c)2016 Maxim Paperno
# Copyright (c)2017 OpenTX
#


BEGIN {
  in_ttls = 0;
  elf_cnt = 0;
  sec_cnt = 0;
}
{
  if ($1 == "----" && NF > 5 && $5 ~ /\.elf$/) {
    fname[elf_cnt] = $5;
  }
  else if (tolower($1) ~ /totals/) {
    in_ttls = 1;
    elf_cnt++;
  }
  else if (in_ttls && NF != 5) {
    in_ttls = 0;
  }
  else if (in_ttls) {
    if (elf_cnt == 1)
      a[++sec_cnt] = $1;
    d[elf_cnt][$1]["total"] = $2;
  }
}
END {
  printf("\n---- Comparing results of %d files ----\n\n", elf_cnt);
  PROCINFO["sorted_in"] = "@ind_num_asc"
  for (j in d) {
    if (length(fname[j-1]))
      printf("file %d: %s\n", j, fname[j-1])
  }
  printf("\n%-8s ", "Section");
  for (j in d) {
    printf("%8s ", "file " j);
    if (j > 1)
      printf("(%7s) ", "\xCE\x94 vs f1");
    if (j > 2)
      printf("(%7s) ", "\xCE\x94 vs f" j-1);
  }
  printf("\n");
  for (i in a) {
    n = a[i];
    printf("%8s ", n);
    for (j in d) {
      printf("%8.2f ", d[j][n]["total"]);
      if (j > 1) 
        printDelta(d[j][n]["total"] - d[1][n]["total"]);
      if (j > 2)
        printDelta(d[j][n]["total"] - d[j-1][n]["total"]);
    }
    printf("\n");
  }
  printf("\n");
}

function printDelta(dlta) {
  if (dlta)
    printf("(%+7.2f) ", dlta);
  else
    printf("(%7s) ", "--.--");
}

