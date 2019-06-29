#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (C) 2014 Savoir-Faire Linux Inc.
# Authors:
#   Sebastien Bourdelin <sebastien.bourdelin@savoirfairelinux.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License Version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import argparse
import csv


def build_transitions_array(f, column):
    f.seek(0)
    reader = csv.reader(f, delimiter=',')
    last = None
    transitions = []
    for i, row in enumerate(reader):
        if i == 0:
            continue
        value = row[column]
        if last is None or value != last:
            transitions.append((float(row[0]) * 1000, int(value)))
            last = value
    debounced = []
    i = 0
    while i < len(transitions):
        t, val = transitions[i]
        if i < len(transitions) - 1 and transitions[i+1][0] - t < 0.002:
            i += 2
            continue
        debounced.append((t, val))
        i += 1
    return debounced


class SBusFrame:
    def __init__(self):
        self.transitions = []

    def push(self, t, value):
        self.transitions.append((t, value))

    def start(self):
        return self.transitions[0][0]

    def end(self):
        return self.transitions[-1][0]

    def is_after(self, t):
        return self.start() >= t

    def value(self, time):
        value = 0
        for t, v in self.transitions:
            if t > time:
                return value
            value = v
        return value

    def byte(self, index):
        t = self.start() + 0.12 * index + 0.015
        value = 0
        for bit in range(8):
            value += (1 - self.value(t)) << bit
            t += 0.010
        return value

    def is_lost(self):
        return self.byte(23) != 0

    def __str__(self):
        return "%.03fms " % self.start() + " ".join(["%02X" % self.byte(i) for i in range(25)])

    @staticmethod
    def get_frames(transitions):
        result = []
        current_frame = None
        last = 0
        for t, value in transitions:
            if t - last > 5:
                if current_frame:
                    result.append(current_frame)
                current_frame = SBusFrame()
            if current_frame is not None:
                current_frame.push(t, value)
            last = t
        return result


def print_statistics(trigger_transitions, sbus_frames, highval, lowval):
    mini, maxi = None, None
    count = 0
    sum = 0
    for t0, val in trigger_transitions[1:]:
        byte = highval if val == 1 else lowval
        for frame in sbus_frames:
            if frame.is_after(t0) and frame.byte(1) == byte:
                delay = frame.end() - t0
                count += 1
                sum += delay
                if mini is None or delay < mini[0]:
                    mini = (delay, t0, frame, val, byte)
                if maxi is None or delay > maxi[0]:
                    maxi = (delay, t0, frame)
                break

    print("Count = %d transitions" % count)
    print("Average = %.1fms" % (sum / count))
    print("Mini = %.1fms @ %fs" % (mini[0], mini[1] / 1000))
    print("Maxi = %.1fms @ %fs" % (maxi[0], maxi[1] / 1000))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('file', help='File to parse', type=argparse.FileType('r'))
    parser.add_argument('--trigger', help='The column in the csv file where is your trigger', type=int, required=True)
    parser.add_argument('--pwm', help='The column in the csv file where is your PWM output', type=int)
    parser.add_argument('--sbus', help='The column in the csv file where is your SBUS output', type=int)
    parser.add_argument('--highval', help='The value of SBUS byte 2 when trigger=HIGH', type=int, default=0x13)
    parser.add_argument('--lowval', help='The value of SBUS byte 2 when trigger=LOW', type=int, default=0xAC)
    args = parser.parse_args()
    if not args.pwm and not args.sbus:
        print("Either a PWM or SBUS column in CSV must be specified")
        exit()

    trigger_transitions = build_transitions_array(args.file, args.trigger)

    sbus_transitions = build_transitions_array(args.file, args.sbus)
    sbus_frames = SBusFrame.get_frames(sbus_transitions)
    for frame in sbus_frames:
        if frame.is_lost():
            print("Frame lost bit @ %fs" % frame.start())

    print_statistics(trigger_transitions, sbus_frames, args.highval, args.lowval)

    # draw(args.file, args.trigger, args.pwm, args.sbus)


if __name__ == "__main__":
    main()
