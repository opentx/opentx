#! /usr/bin/env ruby

require 'pp'

def main(header, xbm)
  base=xbm.sub(/\.xbm/,"")
  cols,rows=1,1
  if base.sub! /_(\d+)x(\d+)/,""
    rows,cols = $1.to_i,$2.to_i
  end

  File.read(xbm)=~/_width\s+(\d+).*_height\s+(\d+).*\{(.*)\}/m
  w,h,bits=$1.to_i,$2.to_i,$3
  bits=eval "["+bits+"]"
  #pp w,h,bits

  wb=(w+7)/8
  hb=(h+7)/8

  out=Array.new(w*hb,0)
  #pp out
  h.times{|yp| yb=yp/8; msk= 1 << (yp%8)
    w.times{|xp| xb=xp/8; msk2=1 << (xp%8)
      out[yb*w+xp]|=msk if bits[wb*yp+xb]&msk2 != 0
    }
  }
  #pp out

#prog_uchar img_4sticks_18x8[] = {
##include "sticks_lcd.h"
#};

  File.open("#{base}.lbm","w"){|f|
    col=0
    if header == "yes"
      f.printf("%d,%d,\n",w/cols,h/rows)
    end
    rows.times{|row|
      (hb/rows).times{|yb| yb
        w.times{|xp|
          f.printf("0x%02x,",out[row*w + yb*w + xp])
        }
        f.puts
      }
    }
  }
end


main(ARGV[0], ARGV[1])
